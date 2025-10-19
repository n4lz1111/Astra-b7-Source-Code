#pragma once
#include <Features/Modules/Module.hpp>

class Regen : public ModuleBase<Regen>
{
public:
    enum class Mode {
        Hive,
    };
    enum class CalcMode {
        Minecraft,
        Test // note for ssi: dont touch working code
    };
    enum class UncoverMode {
        Normal,
        Fast //please for the love of god COMPILE RELEASE BEFORE YOU FUCKING PUSH
    };
    enum class StealPriority {
        Mine,
        Steal
    };
    enum class ConfuseMode {
        Always,
        Auto
    };
    enum class AntiConfuseMode {
        RedstoneCheck,
        ExposedCheck
    };
    enum class OreSelectionMode {
        Normal,
        Closest
    };
    enum class ProgressBarStyle
    {
        Old,
        New
    };

    enum class RotationType
    {
        Old,
        Always,
        Percent
    };
    enum class MiningBypassMode {
        None,        // èÌÇ…å@ÇÈÅiè]óàí ÇËÅj
        Eclipse,   // ãÛíÜÇ≈ÇÕå@ÇÁÇ»Ç¢
		Astra // EclipseÇÃâ¸ó«î≈
        // SuspendInAir,  // è´óàí«â¡ Å® å@ÇËìríÜÇ≈é~ÇﬂÇÈ
        // SlowInAir      // è´óàí«â¡ Å® å@ÇÈë¨ìxóéÇ∆Ç∑
    };
    enum class SwingMode {
        Normal,
        Silent,
        Old,
        OldPacket
    };


    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The regen mode", Mode::Hive, "Hive");
    EnumSettingT<CalcMode> mCalcMode = EnumSettingT<CalcMode>("Calc Mode", "The calculation mode destroy speed", CalcMode::Minecraft, "Minecraft","Fast" );
    NumberSetting mRange = NumberSetting("Range", "The max range for destroying blocks", 7.7, 0, 10, 0.01);
    NumberSetting mDestroySpeed = NumberSetting("Destroy Speed", "The destroy speed for Regen", 0.67, 0.01, 1, 0.01);
    NumberSetting mOtherDestroySpeed = NumberSetting("Other Destroy Speed", "The other destroy speed for Regen", 0.71, 0.01, 1, 0.01);
    EnumSettingT<RotationType> mRotationType = EnumSettingT<RotationType>("RotationMode", "Change the mining rotation for ore", RotationType::Percent, "Old", "Always","Percent");
    NumberSetting mPercent = NumberSetting("RotationPercentage", "Minimum Destruction Progress", 0.6, 0.1, 1, 0.01);
	EnumSettingT<MiningBypassMode> mMiningBypassMode = EnumSettingT<MiningBypassMode>("Mining Mode", "The Mining bypass mode", MiningBypassMode::Eclipse, "None", "Eclipse", "Astra");
    //NumberSetting mDelayTicks = NumberSetting("DelayTicks", "Delay after breaking before next (ticks)", 3, 1, 30, 1);
    NumberSetting mDelayTicks = NumberSetting("DelayTicks", "Don't works (ugokimasenn)", 3, 1, 30, 1);

	EnumSettingT<SwingMode> mSwingMode = EnumSettingT<SwingMode>("Swing Mode", "The swing mode", SwingMode::Silent, "Normal", "Silent", "Old", "OldPacket");
    BoolSetting mSwing = BoolSetting("Swing", "Swings when destroying blocks", true);
    BoolSetting mHotbarOnly = BoolSetting("Hotbar Only", "Only switch to tools in the hotbar", true);
    BoolSetting mUncover = BoolSetting("Uncover", "Uncover redstone if nothing around you is already exposed", true);
    EnumSettingT<UncoverMode> mUncoverMode = EnumSettingT<UncoverMode>("Uncover Mode", "The uncover mode", UncoverMode::Normal, "Normal" ,"Fast");
    //NumberSetting mUncoverRange = NumberSetting("Uncover Range", "The max range for uncovering blocks", 3, 1, 8, 1);
    NumberSetting mUncoverRange = NumberSetting("Uncover Range", "The max range for uncovering blocks", 3, 1, 5, 1);
    BoolSetting mQueueRedstone = BoolSetting("Queue Redstone", "Queue redstone blocks to break when max absorption is reached", false);
    BoolSetting mSteal = BoolSetting("Steal", "Steal the enemy's ore", false);
    NumberSetting mStealerTimeout = NumberSetting("Stealer Timeout", "The max duration for stealer", 1500, 500, 5000, 250);
    EnumSettingT<StealPriority> mStealPriority = EnumSettingT<StealPriority>("Steal Priority", "Ore stealing priority", StealPriority::Mine, "Mine", "Steal");
    BoolSetting mAlwaysSteal = BoolSetting("Always Steal", "Steal the enemy's ore when max absorption is reached", false);
    BoolSetting mDelayedSteal = BoolSetting("Delayed Steal", "Add delay in steal", false);
    NumberSetting mOpponentDestroySpeed = NumberSetting("Opponent Speed", "Specify opponent's destroy speed", 1, 0.01, 1, 0.01);
    BoolSetting mAntiSteal = BoolSetting("Anti Steal", "Stop mining if enemy tried to steal ore", false);
    BoolSetting mConfuse = BoolSetting("Confuse", "Confuse stealer", false);
    BoolSetting mConfuse2 = BoolSetting("Confuse 2", "Confuse stealer", false);
    EnumSettingT<ConfuseMode> mConfuseMode = EnumSettingT<ConfuseMode>("Confuse Mode", "The mode for confuser", ConfuseMode::Always, "Always", "Auto");
    NumberSetting mConfuseDuration = NumberSetting("Confuse Duration", "The time for confuse", 3000, 1000, 10000, 500);
    BoolSetting mAntiConfuse = BoolSetting("Anti Confuse", "Ignore confused blocks due to false stealing", false);
    EnumSettingT<AntiConfuseMode> mAntiConfuseMode = EnumSettingT<AntiConfuseMode>("Anti Confuse Mode", "The anti confuser mode", AntiConfuseMode::RedstoneCheck, "Redstone","Exposed");
    BoolSetting mBlockOre = BoolSetting("Block Ore", "Cover opponent targetting ore", false);
    NumberSetting mBlockRange = NumberSetting("Block Range", "The max range for ore blocker", 5, 0, 10, 0.01);
    BoolSetting mMulti = BoolSetting("Multi", "Allows placing multiple blocks", false);
    BoolSetting mAntiCover = BoolSetting("Anti Cover", "Keep mining even if ore is covered", false);
    NumberSetting mCompensation = NumberSetting("Compensation", "The minium breaking progress percentage value for keep mining", 1, 0.01, 1, 0.01);
    BoolSetting mInfiniteDurability = BoolSetting("Infinite Durability", "Infinite durability for tools (may cause issues!)", false);
    EnumSettingT<OreSelectionMode> mOreSelectionMode = EnumSettingT<OreSelectionMode>("Ore Selection Mode", "The mode for ore selection", OreSelectionMode::Normal, "Normal", "Closest");
    BoolSetting mDynamicDestroySpeed = BoolSetting("Dynamic Destroy Speed", "use faster destroy speed to specified block", false);
    BoolSetting mOnGroundOnly = BoolSetting("OnGround Only", "use dynamic destroy speed only on ground", false);
    BoolSetting mNuke = BoolSetting("Nuke", "destroy block instantly", false);
    BoolSetting mAlwaysMine = BoolSetting("Always mine", "Keep mining ore", false);
    BoolSetting mDebug = BoolSetting("Debug", "Send debug message in chat", false);
    BoolSetting mConfuseNotify = BoolSetting("Confuse Notify", "Send message in chat when confused stealer", true);
    BoolSetting mBlockNotify = BoolSetting("Block Notify", "Send message in chat when you blocked ore/ore got covered", true);
    BoolSetting mFastOreNotify = BoolSetting("Fast Ore Notify", "Send message in chat when fast ore found", true);
    BoolSetting mSyncSpeedNotify = BoolSetting("Sync Speed Notify", "Send message in chat when broke block faster", true);
    BoolSetting mNukeNotify = BoolSetting("Nuke Notify", "Send message in chat when nuked block", true);
    BoolSetting mStealNotify = BoolSetting("Steal Notify", "Send message in chat when u stole ore / ur ore was stolen", true);
    BoolSetting mDynamicUncoverNotify = BoolSetting("Dynamic Uncover Notify", "Debug info abt dynamic uncover", true);
    BoolSetting mRaperNotify = BoolSetting("Raper Notify", "Send message in chat when u raping enemy", true);
    BoolSetting mStealerDetectorNotify = BoolSetting("StealerDetector Notify", "Send message in chat when u raping enemy", true);
    BoolSetting mRenderBlock = BoolSetting("Render Block", "Renders the block you are currently breaking", true);
    EnumSettingT<ProgressBarStyle> mProgressBarStyle = EnumSettingT<ProgressBarStyle>("Progress Bar Style", "The render progress mode", ProgressBarStyle::New, "Simple", "Normal");
    NumberSetting mOffset = NumberSetting("Offeset From Center", "render pos offset from center", 20, -150, 200, 1);
    BoolSetting mRenderProgressBar = BoolSetting("Render Progress Bar", "Renders the progress bar", true);
    BoolSetting mOreFaker = BoolSetting("Ore Faker", "Fakes targetting ore", false);
    BoolSetting mExposed = BoolSetting("Exposed", "Include exposed ore", false);
    BoolSetting mUnexposed = BoolSetting("Unexposed", "Include unexposed ore", false);
    BoolSetting mCovering = BoolSetting("Trigger Stealer", "Include covering block", false);
    BoolSetting mRenderFakeOre = BoolSetting("Render Fake Ore", "Renders the ore you are currenty faking", false);
    BoolSetting mReplace = BoolSetting("Raper", "kicks other hackers while hvh", false);
    BoolSetting mChecker = BoolSetting("Checker", "Checks if the block replacement was successful", false);
    BoolSetting mDynamicUncover = BoolSetting("Dynamic Uncover", "Disables uncover if enemy uncovering ores", false);
    NumberSetting mDisableDuration = NumberSetting("Disable Duration", "The time for dynamic uncover", 3, 1, 10, 1);
    BoolSetting mNoUncoverWhileStealing = BoolSetting("No Uncover While Stealing", "Disables uncover for 5 secs everytime u stole someone's ore", false);
    BoolSetting mStealerDetecter = BoolSetting("Stealer Detector", "Does some funnies if stealer detected :>", false);
    NumberSetting mAmountOfBlocksToDetect = NumberSetting("Stolen Blocks to Detect", "amount of blocks that should be stolen in past 5 seconds to detect stealer", 4, 1, 10, 1);
    BoolSetting mDisableUncover = BoolSetting("Disable Uncover", "Disables uncover for some seconds", false);
    NumberSetting mDisableSeconds = NumberSetting("Disable Duration 2", "amount of seconds uncover will be disabled for", 5, 1, 15, 1);
    BoolSetting mEnableAntiSteal = BoolSetting("Enable AntiSteal", "Enables anti-steal, disables if enemy didn't steal / didn't try to steal ur ore for past 5 seconds", false);
    BoolSetting mAvoidEnemyOre = BoolSetting("Avoid Enemy Ore", "Avoid ore that enemy is targetting", false);


    Regen() : ModuleBase("Regen", "Automatically breaks redstone", ModuleCategory::Player, 0, false) {
        addSettings(
            &mProgressBarStyle,
            &mOffset,
            &mRenderProgressBar,
            &mRenderBlock,
            &mMode,
            &mCalcMode,
            &mRange,
            &mDestroySpeed,
            &mOtherDestroySpeed,
            &mRotationType,
            &mPercent,
			&mMiningBypassMode,
            &mDelayTicks,
            //&mSwing,
			&mSwingMode,
            &mHotbarOnly,
            &mUncover,
            &mUncoverMode,
            &mUncoverRange,
            &mQueueRedstone,
            &mSteal,
            &mStealerTimeout,
            &mStealPriority,
            &mAlwaysSteal,
            &mDelayedSteal,
            &mOpponentDestroySpeed,
            &mReplace,
            &mAntiSteal,
            &mConfuse,
            &mConfuse2,
            &mConfuseMode,
            &mConfuseDuration,
            &mAntiConfuse,
            &mAntiConfuseMode,
            &mBlockOre,
            &mBlockRange,
            &mMulti,
            &mAntiCover,
            &mCompensation,
            &mAvoidEnemyOre,
            &mInfiniteDurability,
            &mOreSelectionMode,
            &mDynamicDestroySpeed,
            &mOnGroundOnly,
            &mNuke,
            &mAlwaysMine,
            &mDebug,
            &mDynamicUncoverNotify,
            &mStealerDetectorNotify,
            &mRaperNotify,
            &mConfuseNotify,
            &mBlockNotify,
            &mSyncSpeedNotify,
            &mNukeNotify,
            &mStealNotify,
            &mFastOreNotify
        );

        VISIBILITY_CONDITION(mReplace, mSteal.mValue);

        addSetting(&mOreFaker);
        addSettings(&mExposed, &mUnexposed);

        addSetting(&mCovering);
        VISIBILITY_CONDITION(mCovering, mOreFaker.mValue);

        addSetting(&mRenderFakeOre);
        VISIBILITY_CONDITION(mExposed, mOreFaker.mValue);
        VISIBILITY_CONDITION(mUnexposed, mOreFaker.mValue);
        VISIBILITY_CONDITION(mRenderFakeOre, mOreFaker.mValue);
        addSetting(&mNoUncoverWhileStealing);
        addSettings(&mDynamicUncover, &mDisableDuration);
        VISIBILITY_CONDITION(mDynamicUncover, mUncover.mValue);
        VISIBILITY_CONDITION(mDisableDuration, mUncover.mValue && mDynamicUncover.mValue);
        addSettings(&mStealerDetecter, &mAmountOfBlocksToDetect);
        VISIBILITY_CONDITION(mAmountOfBlocksToDetect, mStealerDetecter.mValue);
        addSetting(&mDisableUncover);
        addSetting(&mDisableSeconds);
        addSetting(&mEnableAntiSteal);
        VISIBILITY_CONDITION(mDisableUncover, mStealerDetecter.mValue);
        VISIBILITY_CONDITION(mDisableSeconds, mDisableUncover.mValue && mStealerDetecter.mValue);
        VISIBILITY_CONDITION(mEnableAntiSteal, mStealerDetecter.mValue);

        VISIBILITY_CONDITION(mDelayedSteal, mSteal.mValue);
        VISIBILITY_CONDITION(mOpponentDestroySpeed, mSteal.mValue && mDelayedSteal.mValue);


        VISIBILITY_CONDITION(mDestroySpeed, mCalcMode.mValue == CalcMode::Minecraft);
        VISIBILITY_CONDITION(mOtherDestroySpeed, mCalcMode.mValue == CalcMode::Minecraft);

        VISIBILITY_CONDITION(mPercent, mRotationType.mValue == RotationType::Percent);


        VISIBILITY_CONDITION(mUncoverMode, mUncover.mValue);
        VISIBILITY_CONDITION(mUncoverRange, mUncover.mValue && mUncoverMode.mValue == UncoverMode::Normal);

        VISIBILITY_CONDITION(mStealerTimeout, mSteal.mValue);
        VISIBILITY_CONDITION(mStealPriority, mSteal.mValue);
        VISIBILITY_CONDITION(mAlwaysSteal, mSteal.mValue);

        VISIBILITY_CONDITION(mConfuseMode, mConfuse.mValue || mConfuse2.mValue);
        VISIBILITY_CONDITION(mConfuseDuration, (mConfuse.mValue || mConfuse2.mValue) && mConfuseMode.mValue == ConfuseMode::Auto);

        VISIBILITY_CONDITION(mAntiConfuseMode, mAntiConfuse.mValue);

        VISIBILITY_CONDITION(mBlockRange, mBlockOre.mValue);
        VISIBILITY_CONDITION(mMulti, mBlockOre.mValue);

        VISIBILITY_CONDITION(mCompensation, mAntiCover.mValue);

        VISIBILITY_CONDITION(mOnGroundOnly, mDynamicDestroySpeed.mValue);
        VISIBILITY_CONDITION(mNuke, mDynamicDestroySpeed.mValue && mOnGroundOnly.mValue);

        // Debug
        VISIBILITY_CONDITION(mFastOreNotify, mDebug.mValue);

        VISIBILITY_CONDITION(mConfuseNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mBlockNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mSyncSpeedNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mStealNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mNukeNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mDynamicUncoverNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mStealerDetectorNotify, mDebug.mValue);

        VISIBILITY_CONDITION(mRaperNotify, mDebug.mValue);

        VISIBILITY_CONDITION(mOffset, mProgressBarStyle.mValue == ProgressBarStyle::New);

        mNames = {
            {Lowercase, "regen"},
            {LowercaseSpaced, "regen"},
            {Normal, "Regen"},
            {NormalSpaced, "Regen"}
        };

        gFeatureManager->mDispatcher->listen<RenderEvent, &Regen::onRenderEvent, nes::event_priority::LAST>(this);
    }

    bool stealEnabled = false;

    uint64_t lastStoleTime = 0;
    bool antiStealerEnabled = false;
    bool stealerDetected = false;
    int amountOfStolenBlocks = 0;
    uint64_t stealerDetectionStartTime = 0;
    bool startedStealerDetection = false;
    uint64_t uncoverDisabledTime = 0;
    bool uncoverEnabled = true;
    uint64_t lastStealerDetected = 0;

    struct PathFindingResult {
        glm::ivec3 blockPos;
        float time;
    };

    static inline glm::ivec3 mCurrentBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mLastTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mEnemyTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mLastEnemyLayerBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    bool mCanSteal = false;
    bool mIsStealing = false;
    bool mCurrentUncover = false;
    int mCurrentBlockFace = -1;
    float mBreakingProgress = 0.f;
    float mCurrentDestroySpeed = 1.f;
    static inline bool mIsMiningBlock = false;
    static inline bool mWasMiningBlock = false;
    bool mIsUncovering = false;
    bool mWasUncovering = false;
    float mLastTargettingBlockPosDestroySpeed = 1.f;
    int mLastToolSlot = 0;
    bool mIsConfuserActivated = false;
    glm::ivec3 mLastConfusedPos = { INT_MAX, INT_MAX, INT_MAX };
    bool mOffGround = false;

    bool mShouldRotate = false;
    bool mShouldRotateToPlacePos = false;
    bool mShouldSpoofSlot = false;
    bool mShouldSetbackSlot = false;
    glm::ivec3 mBlackListedOrePos = { INT_MAX, INT_MAX, INT_MAX };
    int mPreviousSlot = -1;
    int mToolSlot = -1;

    std::vector<glm::ivec3> miningRedstones;
    glm::ivec3 mCurrentPlacePos = { INT_MAX, INT_MAX, INT_MAX };

    uint64_t mLastBlockPlace = 0;
    uint64_t mLastStealerUpdate = 0;
    uint64_t mLastStealerDetected = 0;
    uint64_t mLastConfuse = 0;
    uint64_t mLastUncoverDetected = 0;
    uint64_t mLastReplaced = 0;
    int mLastPlacedBlockSlot = 0;

    // Replacer
    bool mCanReplace = false;
    int mStartDestroyCount = 0;
    glm::ivec3 mLastReplacedPos = { INT_MAX, INT_MAX, INT_MAX };

    uint64_t mPing = 100;
    uint64_t mEventDelay = 0;

    std::vector<glm::ivec3> mFakePositions;
    //std::vector<glm::ivec3> mLastUpdatedBlockPositions;
    std::vector<glm::ivec3> mLastBrokenOrePos;
    std::vector<glm::ivec3> mLastBrokenCoveringBlockPos;
    std::vector<glm::ivec3> mOreBlackList;

    std::vector<glm::ivec3> mOffsetList = {
        glm::ivec3(0, -1, 0),
        glm::ivec3(0, 1, 0),
        glm::ivec3(0, 0, -1),
        glm::ivec3(0, 0, 1),
        glm::ivec3(-1, 0, 0),
        glm::ivec3(1, 0, 0),
    };

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
    void renderProgressBar();
    void renderNewProgressBar();
    void renderBlock();
    void renderFakeOres();
    void onPacketOutEvent(class PacketOutEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void onSendImmediateEvent(class SendImmediateEvent& event);
    void onPingUpdateEvent(class PingUpdateEvent& event);
    void initializeRegen();
    void resetSyncSpeed();
    void queueBlock(glm::ivec3 blockPos);
    bool isValidBlock(glm::ivec3 blockPos, bool redstoneOnly, bool exposedOnly, bool isStealing = false);
    bool isValidRedstone(glm::ivec3 blockPos);
    //bool mIsMiningBlock = false;
    //bool mWasMiningBlock = false;

    PathFindingResult getBestPathToBlock(glm::ivec3 blockPos);

    std::string getSettingDisplay() override {
        return mMode.mValues[mMode.as<int>()];
    }
};

//if (static_cast<MiningBypassMode>(mMiningBypassMode.mValue) == MiningBypassMode::StopInAir) {
//}

// ÉwÉbÉ_ë§Ç…ïKóvÇ»ÉÅÉìÉoÇí«â¡ÇµÇƒÇ®Ç≠
// std::chrono ÇégÇ¶ÇÈÇÊÇ§Ç…ÇµÇƒÇ®Ç≠
//std::chrono::steady_clock::time_point mLastAbortTime = std::chrono::steady_clock::now() - std::chrono::seconds(1);
//bool mShouldPauseBreaking = false; // optional (égÇÌÇ»Ç¢Ç»ÇÁè¡Ç∑)
//const std::chrono::milliseconds ABORT_COOLDOWN = std::chrono::milliseconds(150); // ìKãXí≤êÆ

// ñ≥å¯ç¿ïWíËã`
//static const glm::ivec3 INVALID_BLOCK_POS = { INT_MAX, INT_MAX, INT_MAX };