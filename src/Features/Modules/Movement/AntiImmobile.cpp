//
// Created by vastrakai on 7/3/2024.
//

#include "AntiImmobile.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Network/Packets/Packet.hpp>
#include <SDK/Minecraft/Network/Packets/MovePlayerPacket.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>


void AntiImmobile::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AntiImmobile::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &AntiImmobile::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &AntiImmobile::onPacketOutEvent>(this);
}

void AntiImmobile::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AntiImmobile::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &AntiImmobile::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &AntiImmobile::onPacketOutEvent>(this);
}

void AntiImmobile::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();

    if (mMode.mValue == Mode::Normal) {
        player->setStatusFlag(ActorFlags::Noai, false);
        return;
    }

    // Clip mode
    glm::vec3* pos = player->getPos();
    if (!pos) return;

    if (NOW - mLastTeleport > 1000 || NOW - mLastDimensionChange > 1000 || !player->getStatusFlag(ActorFlags::Noai))
    {
        return;
    }

    // Continue if the mLastTeleport was within 200ms
    if (NOW - mLastTeleport < 200)
    {
        return;
    }



    bool isAir = true;
    glm::vec3 blockPos = glm::ivec3(floorf(pos->x), floorf(pos->y - PLAYER_HEIGHT), floorf(pos->z));
    bool found = false;

    while (isAir && blockPos.y > 0) {
        blockPos.y -= 1;
        const auto block = ClientInstance::get()->getBlockSource()->getBlock(blockPos);
        spdlog::info("Block at {}/{}/{}: {}", blockPos.x, blockPos.y, blockPos.z, block->mLegacy->getBlockId());
        isAir = block->mLegacy->getBlockId() == 0 || block->mLegacy->getBlockId() == 95;
        if (!isAir)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        spdlog::info("Failed to find a solid block below player");
        return;
    }

    blockPos.x += 0.5f;
    blockPos.y += 1.f;
    blockPos.z += 0.5f;

    player->setPosition(blockPos + glm::vec3(0.f, PLAYER_HEIGHT, 0.f));
    player->getStateVectorComponent()->mPos = blockPos + glm::vec3(0.f, PLAYER_HEIGHT, 0.f);
    player->getStateVectorComponent()->mPosOld = blockPos + glm::vec3(0.f, PLAYER_HEIGHT, 0.f);
    ChatUtils::displayClientMessage("§6AntiImmobile", "§aClipped!");
    spdlog::info("Clipped!");

    // Prevent from teleporting again
    mLastTeleport = 0;
    mLastDimensionChange = 0;
}

void AntiImmobile::onPacketInEvent(PacketInEvent& event)
{

    if (event.mPacket->getId() == PacketID::MovePlayer)
    {
        auto player = ClientInstance::get()->getLocalPlayer();

        auto mp = event.getPacket<MovePlayerPacket>();
        if (mp->mPlayerID == player->getRuntimeID())
            mLastTeleport = NOW;
    }

    if (event.mPacket->getId() == PacketID::ChangeDimension)
    {
        mLastDimensionChange = NOW;
    }

    /*auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::Text) {
        auto tp = event.getPacket<TextPacket>();
        std::string msg = tp->mMessage;

        // /connection の結果から接続サーバー判定
        if (!inSkyWars && msg.find("You are connected to server ") != std::string::npos) {
            std::string game = msg.substr(msg.find("server name ") + 12);
            if (game.find("sky") != std::string::npos) {
                inSkyWars = true;
                noPacketActive = false; // 接続直後はまだ NoPacket OFF
            }
        }

        // ゲーム開始チャット検知
        if (inSkyWars && noPacketActive) {
            std::string plainMsg;
            for (size_t i = 0; i < msg.size(); i++) {
                if (msg[i] == '§') { i++; continue; } // 色コードスキップ
                plainMsg += msg[i];
            }

            if (plainMsg.find("» SkyWars:") == 0) {
                noPacketActive = false; // ゲーム開始 → NoPacket解除
                gameStarted = true;
            }
        }
    }

    // ディメンションチェンジ後に NoPacket ON
    if (inSkyWars && !noPacketActive && event.mPacket->getId() == PacketID::ChangeDimension) {
        noPacketActive = true;
    }*/
}

void AntiImmobile::onPacketOutEvent(PacketOutEvent& event)
{
    /*if (noPacketActive) {
        event.mCancelled = true; // すべての送信パケット停止
    }*/
}
