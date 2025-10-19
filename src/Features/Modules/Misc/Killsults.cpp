//
// Created by vastrakai on 7/19/2024.
//

#pragma execution_character_set("utf-8")


#include "Killsults.hpp"

#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <SDK/Minecraft/Network/Packets/RemoveActorPacket.hpp>
#include <SDK/Minecraft/Network/Packets/ActorEventPacket.hpp>

void Killsults::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketInEvent, &Killsults::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Killsults::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Killsults::onBaseTickEvent>(this);
}

void Killsults::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &Killsults::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Killsults::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Killsults::onBaseTickEvent>(this);
}

void Killsults::onBaseTickEvent(BaseTickEvent& event)
{
    if (mLastAttacked.empty()) return;

    const int64_t now = NOW;


    if (mLastAttacked.size() > 0 && now - mLastAttacked.begin()->second > 8000)
    {
        mLastAttacked.erase(mLastAttacked.begin());
    }
    if (mLastHurt.size() > 0 && now - mLastHurt.begin()->second > 8000)
    {
        mLastHurt.erase(mLastHurt.begin());
    }
}

void Killsults::sendDelayedChatSafe(const std::string& msg, int delay)
{
    std::string copyMsg = msg; // スレッド内で使うコピー
    std::thread([copyMsg, delay]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        PacketUtils::sendChatMessage(copyMsg);
        }).detach();
}


void Killsults::onPacketInEvent(PacketInEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::ActorEvent)
    {
        auto packet = event.getPacket<ActorEventPacket>();

        if (packet->mEvent != ActorEvent::HURT) return;

        Actor* target = ActorUtils::getActorFromRuntimeID(packet->mRuntimeID);
        if (!target) return;

        // if the target is too far away, don't add it to the last attacked list
        if (player->distanceTo(target) > 18.f) return;

        mLastHurt[packet->mRuntimeID] = NOW;
    }

    if (event.mPacket->getId() == PacketID::Text)
    {
        auto tp = event.getPacket<TextPacket>();

        mLastChatMessages.push_back(tp->mMessage);
        while (mLastChatMessages.size() > 5) mLastChatMessages.erase(mLastChatMessages.begin());

        if (tp->mMessage == "§c§l» §r§c§lGame OVER!" && mAutoGG.mValue)
        {
            mAutoGGTemplate.setVariableValue("killCount", std::to_string(mKillCount));
            mAutoGGTemplate.setVariableValue("localPlayer", player->getRawName());
            PacketUtils::sendChatMessage(mAutoGGTemplate.getEntry());
        }
    }

    if (event.mPacket->getId() == PacketID::RemoveActor)
    {
        auto packet = event.getPacket<RemoveActorPacket>();
        if (mLastAttacked.contains(packet->mRuntimeID) && mLastHurt.contains(packet->mRuntimeID))
        {
            auto actor = ActorUtils::getActorFromRuntimeID(packet->mRuntimeID);
            if (!actor) return;
            std::string targetName = actor->getRawName();
            if (std::ranges::none_of(mLastChatMessages, [&targetName](const std::string& msg) { return msg.find(targetName) != std::string::npos; })) return;
            if (targetName.empty()) return;

            mKillCount++;
            spdlog::info("Killed {}, kill count: {}", targetName, mKillCount);


            if (mKillSound.mValue == KillSound::PopDing)
            {
                ClientInstance::get()->playUi("random.orb", mSoundVolume.mValue, 1.0f);
                ClientInstance::get()->playUi("firework.blast", mSoundVolume.mValue, 1.0f);
            }
            else if (mKillSound.mValue == KillSound::Cash)
            {
                SoundUtils::playSoundFromEmbeddedResource("cash_register_sound.wav", mSoundVolume.mValue);
            }

            if (mShowNotification.mValue)
            {
                NotifyUtils::notify("You killed " + targetName + "!", 5.f, Notification::Type::Info);
            }

            if (mMode.mValue == Mode::None) return;

            // if the target's name contains any spaces, put it in quotes
            if (targetName.find(' ') != std::string::npos)
            {
                targetName = "\"" + targetName + "\"";
            }

            targetName = "@" + targetName; // mention the target

            mCustomKillsults.setVariableValue("target", targetName);
            mCustomKillsults.setVariableValue("localPlayer", player->getRawName());
            mCustomKillsults.setVariableValue("killCount", std::to_string(mKillCount));

            if (mMode.mValue == Mode::OldSaba)
            {
                static std::vector<std::string> lines = {
                    "auau",
                    "あうあうあー！",
                    "み、みはりぃそれはだめ...////",
                    "お兄ちゃんここが弱いんだぁ♡",
                    "!target!が悪いんだぞ..",
                    "出る..出ちゃう....///",
                    "そんなぁ....出る...////",
                    "だめだめえっちすぎます！"
                };

                static int index = 0;
                std::string onimaiMsg;

                std::vector<int> selectableIndices;
                for (int i = 0; i < static_cast<int>(lines.size()); i++) {
                    if (mOnimaiOnly.mValue && (i == 0 || i == 1)) continue;
                    selectableIndices.push_back(i);
                }

                static int lastIndex = -1;
                if (mRandomize.mValue)
                {
                    if (selectableIndices.size() <= 1) {
                        index = 0;
                    }
                    else {
                        int newIndex;
                        do {
                            newIndex = selectableIndices[rand() % selectableIndices.size()];
                        } while (newIndex == lastIndex);
                        index = newIndex;
                    }
                    lastIndex = index;
                }
                else
                {
                    index++;
                    if (index >= static_cast<int>(lines.size())) index = 0;
                    if (mOnimaiOnly.mValue && (index == 0 || index == 1)) index = 2;
                }

                onimaiMsg = lines[index];

                const std::string placeholder = "!target!";
                size_t pos = onimaiMsg.find(placeholder);
                if (pos != std::string::npos)
                    onimaiMsg.replace(pos, placeholder.length(), targetName);

                // 💬 Endingを付与
                switch (mEnding.mValue) {
                case Ending::Astra:
                    onimaiMsg += " | ᴀꜱᴛʀᴀ";
                    break;
                case Ending::Saba:
                    onimaiMsg += " | ꜱᴀʙᴀ";
                    break;
                case Ending::None:
                default:
                    break;
                }

                // 💥 Anniモード時は先頭に「!」を付ける
                if (mAnni.mValue)
                    onimaiMsg = "!" + onimaiMsg;

                std::string onimaiMsgCopy = onimaiMsg; // index や targetName の影響を受けないコピー
                sendDelayedChatSafe(onimaiMsgCopy, static_cast<int>(mChatDelay.mValue));

                return;
            }


            // 💬 Ending（語尾）を追加する
            std::string message = mCustomKillsults.getEntry();
            switch (mEnding.mValue) {
            case Ending::Astra:
                message += " | ᴀꜱᴛʀᴀ";
                break;
            case Ending::Saba:
                message += " | ꜱᴀʙᴀ";
                break;
            case Ending::None:
            default:
                break;
            }

            // 💥 Anniモード時は先頭に「!」を付ける
            if (mAnni.mValue)
                message = "!" + message;

            std::string customMsgCopy = mCustomKillsults.getEntry();
            sendDelayedChatSafe(customMsgCopy, static_cast<int>(mChatDelay.mValue));


        }
    }

    if (event.mPacket->getId() == PacketID::ChangeDimension)
    {
        mLastAttacked.clear();
        mLastChatMessages.clear();
        mKillCount = 0;
    }
}

void Killsults::onPacketOutEvent(PacketOutEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::InventoryTransaction)
    {
        auto packet = event.getPacket<InventoryTransactionPacket>();

        auto* ait = reinterpret_cast<ItemUseOnActorInventoryTransaction*>(packet->mTransaction.get());
        if (ait->mActionType == ItemUseOnActorInventoryTransaction::ActionType::Attack)
        {
            auto actor = ActorUtils::getActorFromUniqueId(ait->mActorId);
            if (!actor) return;

            mLastAttacked[actor->getRuntimeID()] = NOW;
        }
    }
}
