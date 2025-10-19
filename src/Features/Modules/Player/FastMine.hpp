#pragma once
//
// Created by dark on 9/23/2024.
//

#include <Features/Modules/Module.hpp>

class FastMine : public ModuleBase<FastMine> {
public:
    enum class Mode 
    {
        Hive,
        BDS,
        //Packet
        PMMP,
    };

    EnumSettingT<Mode> mMode = EnumSettingT("Mode", "The mode of the fast mine", Mode::Hive, "Hive", "BDS", "PMMP");
    NumberSetting mDestroySpeed = NumberSetting("Destroy Speed", "The destroy speed for fast mine", 1, 0.01, 1, 0.01);
    BoolSetting mInfiniteDurability = BoolSetting("Infinite Durability", "Infinite durability for tools (may cause issues!)", false);
    FastMine() : ModuleBase("FastMine", "Increases your mining speed", ModuleCategory::Player, 0, false)
    {
        addSettings(&mMode, &mDestroySpeed, &mInfiniteDurability);

        VISIBILITY_CONDITION(mInfiniteDurability, mMode.mValue == Mode::Hive);

        mNames = {
              {Lowercase, "fastmine"},
                {LowercaseSpaced, "fast mine"},
                {Normal, "FastMine"},
                {NormalSpaced, "Fast Mine"}
        };
    };

private:
    glm::ivec3 mCurrentBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    float mBreakingProgress = 0.f;
    void onEnable();
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
//    void onBaseTickEventPMMP();
//    void sendFakeBreakEndPacket(const glm::ivec3& pos);
//    glm::ivec3 mCurrentBlockPos = glm::ivec3(INT_MAX);
//    float mBreakingProgress = 0.f;
};