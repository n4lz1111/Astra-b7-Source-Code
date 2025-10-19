//
// Created by vastrakai on 7/18/2024.
//

#include "Animations.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/BobHurtEvent.hpp>
#include <Features/Events/BoneRenderEvent.hpp>
#include <Features/Events/SwingDurationEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <SDK/Minecraft/Actor/ActorPartModel.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>

std::vector<unsigned char> gNoSwitchAnimation = {
    0x0F, 0x57, 0xC0, // xorps xmm0, xmm0
    0x90, 0x90, 0x90, 0x90, 0x90 // nop
};

//DEFINE_PATCH_FUNC(patchNoSwitchAnimation, SigManager::ItemInHandRenderer_render_bytepatch, gNoSwitchAnimation);
DEFINE_NOP_PATCH_FUNC(patchFluxSwing, SigManager::FluxSwing, 0x5);

void Animations::onEnable()
{
    gFeatureManager->mDispatcher->listen<SwingDurationEvent, &Animations::onSwingDurationEvent>(this);
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Animations::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<BoneRenderEvent, &Animations::onBoneRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<BobHurtEvent, &Animations::onBobHurtEvent>(this);

    if (!mSwingAngle)
    {
        mSwingAngle = reinterpret_cast<float*>(SigManager::TapSwingAnim);
        MemUtils::setProtection(reinterpret_cast<uintptr_t>(mSwingAngle), sizeof(float), PAGE_READWRITE);
    }

    //patchNoSwitchAnimation(mNoSwitchAnimation.mValue);
    patchFluxSwing(mFluxSwing.mValue);
}

void Animations::onDisable()
{
    gFeatureManager->mDispatcher->deafen<SwingDurationEvent, &Animations::onSwingDurationEvent>(this);
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Animations::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<BoneRenderEvent, &Animations::onBoneRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<BobHurtEvent, &Animations::onBobHurtEvent>(this);
    //patchNoSwitchAnimation(false);
    patchFluxSwing(false);

    if (mSwingAngle) *mSwingAngle = glm::radians(-80.f);
}


void Animations::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;

    // 盾を持ってBlock中ならFluxSwingを強制ON
    if (mShouldBlock) {
        patchFluxSwing(true);
    }
    else {
        patchFluxSwing(mFluxSwing.mValue);
    }

    if (mOnlyOnBlock.mValue && mSwingAngle)
    {
        if (mShouldBlock)
        {
            *mSwingAngle = glm::radians(mCustomSwingAngle.mValue ? mSwingAngleSetting.as<float>() : -80.f);
        }
        else
        {
            *mSwingAngle = glm::radians(-80.f);
        }
    }
    else if (mSwingAngle)
    {
        *mSwingAngle = glm::radians(mCustomSwingAngle.mValue ? mSwingAngleSetting.as<float>() : -80.f);
    }

    mOldSwingDuration = mSwingDuration;
    mSwingDuration = player->getSwingProgress();
}


void Animations::onSwingDurationEvent(SwingDurationEvent& event)
{
    event.mSwingDuration = mSwingTime.as<int>();
}

void Animations::onBoneRenderEvent(BoneRenderEvent& event)
{
    auto ent = event.mActor;
    auto player = ClientInstance::get()->getLocalPlayer();
    auto bone = event.mBone;
    auto partModel = event.mPartModel;

    if (ent != ClientInstance::get()->getLocalPlayer()) return;

    if (bone->mBoneStr == "rightarm" && mThirdPersonBlock.mValue)
    {
        auto heldItem = player->getSupplies()->getContainer()->getItem(player->getSupplies()->mInHandSlot);
        bool isHoldingSword = heldItem && heldItem->mItem && heldItem->getItem()->isSword();
        if ((!ClientInstance::get()->getMouseGrabbed() && ImGui::IsMouseDown(ImGuiMouseButton_Right) && isHoldingSword || event.mDoBlockAnimation && isHoldingSword) && mAnimation.mValue != Animation::Default)
        {
            float xRot = mXRot.mValue;
            float yRot = mYRot.mValue;
            float zRot = mZRot.mValue;

            Actor* player = ClientInstance::get()->getLocalPlayer();

            if (player->getSwingProgress() > 0) {
                xRot = MathUtils::animate(-30, xRot, ImRenderUtils::getDeltaTime() * 3.f);
                yRot = MathUtils::animate(0, yRot, ImRenderUtils::getDeltaTime() * 3.f);
            }
            else {
                xRot = MathUtils::animate(-65, xRot, ImRenderUtils::getDeltaTime() * 3.f);
                yRot = MathUtils::animate(-17, yRot, ImRenderUtils::getDeltaTime() * 3.f);
            }

            partModel->mRot.x = xRot;
            partModel->mRot.y = yRot;
            partModel->mRot.z = zRot;
            partModel->mSize.z = 0.9177761;
        }
    }
}

void Animations::onBobHurtEvent(BobHurtEvent& event)
{
    auto matrix = event.mMatrix;

    if (mSmallItems.mValue)
        *matrix = glm::translate(*matrix, glm::vec3(0.5f, -0.2f, -0.6f));

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    auto heldItem = player->getSupplies()->getContainer()->getItem(player->getSupplies()->mInHandSlot);
    bool isHoldingSword = heldItem && heldItem->mItem && heldItem->getItem()->isSword();

    mShouldBlock = (!ClientInstance::get()->getMouseGrabbed() && ImGui::IsMouseDown(ImGuiMouseButton_Right) && isHoldingSword) || event.mDoBlockAnimation;

    if (mShouldBlock)
        patchFluxSwing(true);
    else
        patchFluxSwing(mFluxSwing.mValue);

    if ((!ClientInstance::get()->getMouseGrabbed() && ImGui::IsMouseDown(ImGuiMouseButton_Right) && isHoldingSword || event.mDoBlockAnimation && isHoldingSword) && mAnimation.mValue != Animation::Default)
    {
        *matrix = glm::translate(*matrix, glm::vec3(0.4, 0.0, -0.15));
        *matrix = glm::translate(*matrix, glm::vec3(-0.1f, 0.15f, -0.2f));
        *matrix = glm::translate(*matrix, glm::vec3(-0.24F, 0.25f, -0.20F));
        *matrix = glm::rotate(*matrix, -1.98F, glm::vec3(0.0F, 1.0F, 0.0F));
        *matrix = glm::rotate(*matrix, 1.30F, glm::vec3(4.0F, 0.0F, 0.0F));
        *matrix = glm::rotate(*matrix, 59.9F, glm::vec3(0.0F, 1.0F, 0.0F));
        *matrix = glm::translate(*matrix, glm::vec3(0.0f, -0.1f, 0.15f));
        *matrix = glm::translate(*matrix, glm::vec3(0.08f, 0.0f, 0.0f));
        *matrix = glm::scale(*matrix, glm::vec3(1.05f, 1.05f, 1.05f));
    }

    if (mAnimation.mValue == Animation::Test && mShouldBlock)
    {
        int swingProgress = mSwingDuration;
        int oldSwingProgress = mOldSwingDuration;
        float lerpedSwing = MathUtils::lerp(oldSwingProgress, swingProgress, ClientInstance::get()->getMinecraftSim()->getGameSim()->mDeltaTime);
        float percent = lerpedSwing / mSwingTime.as<int>();

        // 三角波的に中央で最大になるように変換
        float wave = percent < 0.5f ? percent * 2.0f : (1.0f - percent) * 2.0f;

        float zOffset = -0.2f * wave;
        float scale = 1.0f - 0.3f * wave;

        // 新しい行列を作らず、既存の行列に対して transform する
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f, 0.2f, -0.12f + zOffset));
        transform = glm::scale(transform, glm::vec3(scale, scale, scale));
        transform = glm::rotate(transform, -glm::radians(45.0f * wave), glm::vec3(1, 0, 0));
        transform = glm::rotate(transform, glm::radians(10.0f * wave), glm::vec3(0, 1, 0));

        *matrix = (*matrix) * transform; // 既存の matrix に累積
    }

}
