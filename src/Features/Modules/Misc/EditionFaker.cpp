//
// Created by ChatGPT on 2025-10-09.
//

#include "EditionFaker.hpp"
#include <SDK/SigManager.hpp>
#include <Utils/Buffer.hpp>
#include <Features/Events/ConnectionRequestEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/Network/ConnectionRequest.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>

static uintptr_t func;
static uintptr_t func2;
static uintptr_t func3;
static uintptr_t func4;
static uintptr_t func5;

void EditionFaker::onInit() {
    func = SigManager::ConnectionRequest_create_DeviceOS;
    func2 = SigManager::ConnectionRequest_create_DefaultInputMode;
    func3 = SigManager::ConnectionRequest_create_CurrentInputMode;
    func4 = SigManager::InputModeBypass;
    func5 = SigManager::InputModeBypassFix;
}

void EditionFaker::inject() {
    int32_t DefaultInputMode = mInputMethod.as<int>();

    // default & current input mode spoof
    MemUtils::ReadBytes((void*)func2, mOriginalInputData1, sizeof(mOriginalInputData1));
    MemUtils::ReadBytes((void*)func3, mOriginalInputData2, sizeof(mOriginalInputData2));
    MemUtils::writeBytes(func2, "\xBF", 1);
    MemUtils::writeBytes(func2 + 1, &DefaultInputMode, sizeof(uint32_t));
    MemUtils::NopBytes(func2 + 5, 3);
    MemUtils::writeBytes(func3, "\xBF", 1);
    MemUtils::writeBytes(func3 + 1, &DefaultInputMode, sizeof(uint32_t));
    MemUtils::NopBytes(func3 + 5, 3);

    // OS spoof
    MemUtils::ReadBytes((void*)func, mOriginalData, sizeof(mOriginalData));
    MemUtils::writeBytes(func + 1, &DefaultInputMode, sizeof(uint32_t));

    // input mode bypass
    MemUtils::ReadBytes((void*)func4, mOriginalData1, sizeof(mOriginalData1));
    MemUtils::ReadBytes((void*)func5, mOriginalData2, sizeof(mOriginalData2));
    mDetour1 = AllocateBuffer((void*)func4);
    mDetour2 = AllocateBuffer((void*)func5);
    MemUtils::writeBytes((uintptr_t)mDetour1, mDetourBytes1, sizeof(mDetourBytes1));
    MemUtils::writeBytes((uintptr_t)mDetour2, mDetourBytes2, sizeof(mDetourBytes2));

    int32_t rip1 = MemUtils::GetRelativeAddress((uintptr_t)mDetour1 + sizeof(mDetourBytes1) + 1, func4 + 5);
    int32_t rip2 = MemUtils::GetRelativeAddress((uintptr_t)mDetour2 + sizeof(mDetourBytes2) + 1, func5 + 5);

    MemUtils::writeBytes((uintptr_t)mDetour1 + sizeof(mDetourBytes1), "\xE9", 1);
    MemUtils::writeBytes((uintptr_t)mDetour1 + sizeof(mDetourBytes1) + 1, &rip1, sizeof(int32_t));
    MemUtils::writeBytes((uintptr_t)mDetour2 + sizeof(mDetourBytes2), "\xE9", 1);
    MemUtils::writeBytes((uintptr_t)mDetour2 + sizeof(mDetourBytes2) + 1, &rip2, sizeof(int32_t));

    int32_t newRip1 = MemUtils::GetRelativeAddress(func4 + 1, (uintptr_t)mDetour1);
    int32_t newRip2 = MemUtils::GetRelativeAddress(func5 + 1, (uintptr_t)mDetour2);
    MemUtils::writeBytes(func4, "\xE9", 1);
    MemUtils::writeBytes(func4 + 1, &newRip1, sizeof(int32_t));
    MemUtils::writeBytes(func5, "\xE9", 1);
    MemUtils::writeBytes(func5 + 1, &newRip2, sizeof(int32_t));
}

void EditionFaker::eject() {
    MemUtils::writeBytes(func2, mOriginalInputData1, sizeof(mOriginalInputData1));
    MemUtils::writeBytes(func3, mOriginalInputData2, sizeof(mOriginalInputData2));
    MemUtils::writeBytes(func + 1, &mOriginalData, sizeof(uint32_t));
    MemUtils::writeBytes(func4, mOriginalData1, sizeof(mOriginalData1));
    MemUtils::writeBytes(func5, mOriginalData2, sizeof(mOriginalData2));
    FreeBuffer(mDetour1);
    FreeBuffer(mDetour2);
}

// JSON を経由して安全に DeviceSpoof（引数ありバージョン）
void EditionFaker::spoofEdition(ConnectionRequest* request) {
    if (!request) return;

    try {
        nlohmann::json json = request->toJson();

        // OS / InputMode / DeviceModel を安全に設定
        int os = mOs.as<int>();
        int inputMode = mInputMethod.as<int>();

        // DeviceModel を動的に生成して埋める（lifetime 問題回避）
        std::string deviceModel = StringUtils::generateMboard(os);

        // 書き換え（存在しないキーでも追加される）
        json["DeviceModel"] = deviceModel;       // 例: "System Product Name ASUS"
        json["DeviceOS"] = os;                   // 数字
        json["CurrentInputMode"] = inputMode;    // 現在の入力モード
        json["DefaultInputMode"] = inputMode;    // デフォルト入力モードも合わせる

        request->fromJson(json);
    }
    catch (const std::exception& e) {
        spdlog::warn("[EditionFaker] spoofEdition JSON error: {}", e.what());
    }
}


void EditionFaker::onEnable() {
    inject();
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &EditionFaker::onPacketOutEvent, nes::event_priority::ABSOLUTE_LAST>(this);
    gFeatureManager->mDispatcher->listen<ConnectionRequestEvent, &EditionFaker::onConnectionRequestEvent, nes::event_priority::ABSOLUTE_LAST>(this);
}

void EditionFaker::onDisable() {
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &EditionFaker::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<ConnectionRequestEvent, &EditionFaker::onConnectionRequestEvent>(this);
    eject();
}

void EditionFaker::onConnectionRequestEvent(ConnectionRequestEvent& event) {
    // event.mResult は ConnectionRequest* のはず（ヘッダでチェック）
    spoofEdition(event.mResult);
    // もし event.mResult が null の可能性があるならチェック済みなので安全
}

void EditionFaker::onPacketOutEvent(PacketOutEvent& event) {
    if (event.mPacket->getId() == PacketID::PlayerAuthInput) {
        auto paip = event.getPacket<PlayerAuthInputPacket>();
        paip->mInputMode = (InputMode)mInputMethod.as<int>();
    }
}
