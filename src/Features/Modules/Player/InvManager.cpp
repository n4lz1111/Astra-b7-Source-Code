//
// Created by vastrakai on 7/6/2024.
//

#include "InvManager.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/PingUpdateEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/InteractPacket.hpp>
#include <SDK/Minecraft/Network/Packets/ContainerClosePacket.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>

static const char* ItemTypeName(SItemType t) {
    switch (t) {
        case SItemType::Helmet:   return "Helmet";
        case SItemType::Chestplate:return "Chestplate";
        case SItemType::Leggings: return "Leggings";
        case SItemType::Boots:    return "Boots";
        case SItemType::Sword:    return "Sword";
        case SItemType::Pickaxe:  return "Pickaxe";
        case SItemType::Axe:      return "Axe";
        case SItemType::Shovel:   return "Shovel";
        default:                  return "Other";
    }
}

static inline bool shouldPauseForUI() {
    auto ci = ClientInstance::get();
    return ci && ci->getMouseGrabbed();
}

static inline bool isHotbarIndex(int idx) {
    return (idx >= 0  && idx <= 8)  ||
           (idx >= 9  && idx <= 17) ||
           (idx >= 27 && idx <= 35);
}

static inline int hotbarBaseOf(int idx) {
    if (idx >= 0  && idx <= 8)  return 0;
    if (idx >= 9  && idx <= 17) return 9;
    if (idx >= 27 && idx <= 35) return 27;
    return -1;
}

static inline int chooseTargetBaseForPref(int fromSlot) {
    int b = hotbarBaseOf(fromSlot);
    return (b >= 0) ? b : 0;
}

void InvManager::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &InvManager::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &InvManager::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &InvManager::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<PingUpdateEvent, &InvManager::onPingUpdateEvent>(this);
}

void InvManager::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &InvManager::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &InvManager::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &InvManager::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<PingUpdateEvent, &InvManager::onPingUpdateEvent>(this);
}

void InvManager::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    auto armorContainer = player->getArmorContainer();
    auto supplies = player->getSupplies();
    auto container = supplies->getContainer();

    if (mManagementMode.mValue != ManagementMode::Always && !mHasOpenContainer)
        return;

    // Check free slots
    int freeSlots = 0;
    for (int i = 0; i < 36; i++)
        if (!container->getItem(i)->mItem) freeSlots++;

    if (ClientInstance::get()->getMouseGrabbed() && player && freeSlots > 0 && mManagementMode.mValue == ManagementMode::Always)
        return;

    bool isInstant = mMode.mValue == Mode::Instant;
    if (mLastAction + static_cast<uint64_t>(mDelay.mValue) > NOW)
        return;

    std::vector<int> itemsToEquip;

    // --- Find best items ---
    int bestHelmetSlot = -1, bestChestplateSlot = -1, bestLeggingsSlot = -1, bestBootsSlot = -1;
    int bestSwordSlot = -1, bestPickaxeSlot = -1, bestAxeSlot = -1, bestShovelSlot = -1;
    int bestHelmetValue = 0, bestChestplateValue = 0, bestLeggingsValue = 0, bestBootsValue = 0;
    int bestSwordValue = 0, bestPickaxeValue = 0, bestAxeValue = 0, bestShovelValue = 0;

    int equippedHelmetValue = ItemUtils::getItemValue(armorContainer->getItem(0));
    int equippedChestplateValue = ItemUtils::getItemValue(armorContainer->getItem(1));
    int equippedLeggingsValue = ItemUtils::getItemValue(armorContainer->getItem(2));
    int equippedBootsValue = ItemUtils::getItemValue(armorContainer->getItem(3));

    int firstBowSlot = -1;
    int fireSwordSlot = ItemUtils::getFireSword(false);

    for (int i = 0; i < 36; i++)
    {
        auto item = container->getItem(i);
        if (!item->mItem) continue;
        auto itemType = item->getItem()->getItemType();

        // Drop extra bows
        if (item->getItem()->mName.contains("bow") && firstBowSlot == -1 && mDropExtraBows.mValue)
            firstBowSlot = i;
        else if (firstBowSlot != -1 && mDropExtraBows.mValue && item->getItem()->mName.contains("bow"))
        {
            supplies->getContainer()->dropSlot(i);
            mLastAction = NOW;
            if (!isInstant) return;
        }

        if (mIgnoreFireSword.mValue && fireSwordSlot != -1 && fireSwordSlot == i) continue;

        int itemValue = ItemUtils::getItemValue(item);

        switch (itemType)
        {
        case SItemType::Helmet:
            if (itemValue > bestHelmetValue && equippedHelmetValue < itemValue)
            {
                bestHelmetSlot = i; bestHelmetValue = itemValue;
            }
            break;
        case SItemType::Chestplate:
            if (itemValue > bestChestplateValue && equippedChestplateValue < itemValue)
            {
                bestChestplateSlot = i; bestChestplateValue = itemValue;
            }
            break;
        case SItemType::Leggings:
            if (itemValue > bestLeggingsValue && equippedLeggingsValue < itemValue)
            {
                bestLeggingsSlot = i; bestLeggingsValue = itemValue;
            }
            break;
        case SItemType::Boots:
            if (itemValue > bestBootsValue && equippedBootsValue < itemValue)
            {
                bestBootsSlot = i; bestBootsValue = itemValue;
            }
            break;
        case SItemType::Sword:
            if (itemValue > bestSwordValue) { bestSwordSlot = i; bestSwordValue = itemValue; }
            break;
        case SItemType::Pickaxe:
            if (itemValue > bestPickaxeValue) { bestPickaxeSlot = i; bestPickaxeValue = itemValue; }
            break;
        case SItemType::Axe:
            if (itemValue > bestAxeValue) { bestAxeSlot = i; bestAxeValue = itemValue; }
            break;
        case SItemType::Shovel:
            if (itemValue > bestShovelValue) { bestShovelSlot = i; bestShovelValue = itemValue; }
            break;
        default: break;
        }
    }

    // --- Drop unnecessary items ---
    for (int i = 0; i < 36; i++)
    {
        auto item = container->getItem(i);
        if (!item->mItem) continue;
        if (mIgnoreFireSword.mValue && fireSwordSlot != -1 && fireSwordSlot == i) continue;

        auto itemType = item->getItem()->getItemType();
        int itemValue = ItemUtils::getItemValue(item);
        bool hasFireProtection = item->getEnchantValue(Enchant::FIRE_PROTECTION) > 0;

        if (mStealFireProtection.mValue && hasFireProtection) continue;

        bool toDrop = false;
        switch (itemType)
        {
        case SItemType::Sword: toDrop = (i != bestSwordSlot); break;
        case SItemType::Pickaxe: toDrop = (i != bestPickaxeSlot); break;
        case SItemType::Axe: toDrop = (i != bestAxeSlot); break;
        case SItemType::Shovel: toDrop = (i != bestShovelSlot); break;
        case SItemType::Helmet: toDrop = (i != bestHelmetSlot && !(mStealFireProtection.mValue && hasFireProtection)); break;
        case SItemType::Chestplate: toDrop = (i != bestChestplateSlot && !(mStealFireProtection.mValue && hasFireProtection)); break;
        case SItemType::Leggings: toDrop = (i != bestLeggingsSlot && !(mStealFireProtection.mValue && hasFireProtection)); break;
        case SItemType::Boots: toDrop = (i != bestBootsSlot && !(mStealFireProtection.mValue && hasFireProtection)); break;
        default: break;
        }

        if (toDrop)
        {
            supplies->getContainer()->dropSlot(i);
            mLastAction = NOW;
            if (!isInstant) return;
        }
    }

    // --- Preferred slots (hotbar移動込み) ---
    auto moveToPreferredSlot = [&](int& bestSlot, int preferred) {
        if (bestSlot == -1 || preferred == 0) return;
        int target = preferred - 1;

        // インベントリ内ならまず空きホットバーに移動
        if (bestSlot >= 9) {
            int emptyHotbar = ItemUtils::getEmptyHotbarSlot(); // ←ここを置き換え
            if (emptyHotbar != -1) {
                supplies->getContainer()->swapSlots(bestSlot, emptyHotbar);
                bestSlot = emptyHotbar;
            }
        }

        if (bestSlot != target) {
            supplies->getContainer()->swapSlots(bestSlot, target);
            mLastAction = NOW;
            if (!isInstant) return;
        }
        };

    moveToPreferredSlot(bestSwordSlot, mPreferredSwordSlot.mValue);
    moveToPreferredSlot(bestPickaxeSlot, mPreferredPickaxeSlot.mValue);
    moveToPreferredSlot(bestAxeSlot, mPreferredAxeSlot.mValue);
    moveToPreferredSlot(bestShovelSlot, mPreferredShovelSlot.mValue);
    moveToPreferredSlot(fireSwordSlot, mPreferredFireSwordSlot.mValue);

    if (mPreferredBlocksSlot.mValue != 0) {
        ItemStack* item = container->getItem(mPreferredBlocksSlot.mValue - 1);
        if (!ItemUtils::isUsableBlock(item)) {
            int firstPlaceable = ItemUtils::getFirstPlaceable(false);
            if (firstPlaceable != -1) {
                supplies->getContainer()->swapSlots(firstPlaceable, mPreferredBlocksSlot.mValue - 1);
                mLastAction = NOW;
                if (!isInstant) return;
            }
        }
    }

    // --- Equip armor ---
    if (bestHelmetSlot != -1) itemsToEquip.push_back(bestHelmetSlot);
    if (bestChestplateSlot != -1) itemsToEquip.push_back(bestChestplateSlot);
    if (bestLeggingsSlot != -1) itemsToEquip.push_back(bestLeggingsSlot);
    if (bestBootsSlot != -1) itemsToEquip.push_back(bestBootsSlot);

    for (auto& slot : itemsToEquip) {
        supplies->getContainer()->equipArmor(slot);
        mLastAction = NOW;
        if (!isInstant) break;
    }
}


void InvManager::onPacketInEvent(PacketInEvent& event)
{
    if (event.mPacket->getId() == PacketID::ContainerOpen)
    {
        auto packet = event.getPacket<ContainerOpenPacket>();
        if (mManagementMode.mValue == ManagementMode::ContainerOnly || mManagementMode.mValue == ManagementMode::InvOnly && packet->mType == ContainerType::Inventory)
        {
            mHasOpenContainer = true;
        }
    }
    if (event.mPacket->getId() == PacketID::ContainerClose)
    {
        mHasOpenContainer = false;
    }
}

void InvManager::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.mPacket->getId() == PacketID::ContainerClose)
    {
        mHasOpenContainer = false;
    }
    else if (event.mPacket->getId() == PacketID::ContainerOpen)
    {
        auto packet = event.getPacket<ContainerOpenPacket>();
        if (mManagementMode.mValue == ManagementMode::ContainerOnly || mManagementMode.mValue == ManagementMode::InvOnly && packet->mType == ContainerType::Inventory)
        {
            mHasOpenContainer = true;
        }
    }
}

void InvManager::onPingUpdateEvent(PingUpdateEvent& event)
{
    mLastPing = event.mPing;
}

bool InvManager::isItemUseless(ItemStack* item, int slot)
{
    if (!item->mItem) return true;
    auto player = ClientInstance::get()->getLocalPlayer();
    SItemType itemType = item->getItem()->getItemType();
    auto itemValue = ItemUtils::getItemValue(item);
    auto Inv_Manager = gFeatureManager->mModuleManager->getModule<InvManager>();

    if (itemType == SItemType::Helmet || itemType == SItemType::Chestplate || itemType == SItemType::Leggings || itemType == SItemType::Boots)
    {
        int equippedItemValue = ItemUtils::getItemValue(player->getArmorContainer()->getItem(static_cast<int>(itemType)));
        bool hasFireProtection = item->getEnchantValue(Enchant::FIRE_PROTECTION) > 0;

        if (Inv_Manager->mStealFireProtection.mValue && hasFireProtection) {
            return false;
        }

        return equippedItemValue >= itemValue;
    }

    if (itemType == SItemType::Sword || itemType == SItemType::Pickaxe || itemType == SItemType::Axe || itemType == SItemType::Shovel)
    {
        int bestSlot = ItemUtils::getBestItem(itemType);
        int bestValue = ItemUtils::getItemValue(player->getSupplies()->getContainer()->getItem(bestSlot));

        return bestValue >= itemValue && bestSlot != slot;
    }

    return false;
}