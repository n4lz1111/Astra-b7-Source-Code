//
// Created by vastrakai on 7/4/2024.
//

#include "Notifications.hpp"

#include <Features/Events/ConnectionRequestEvent.hpp>
#include <Features/Events/NotifyEvent.hpp>
#include <Features/Events/RenderEvent.hpp>

void Notifications::onEnable()
{
    gFeatureManager->mDispatcher->listen<NotifyEvent, &Notifications::onNotifyEvent>(this);
    gFeatureManager->mDispatcher->listen<ModuleStateChangeEvent, &Notifications::onModuleStateChange>(this);
    gFeatureManager->mDispatcher->listen<ModuleScriptStateChangeEvent, &Notifications::onModuleScriptStateChange>(this);
    gFeatureManager->mDispatcher->listen<ConnectionRequestEvent, &Notifications::onConnectionRequestEvent>(this);
}

void Notifications::onDisable()
{
    gFeatureManager->mDispatcher->deafen<NotifyEvent, &Notifications::onNotifyEvent>(this);
    gFeatureManager->mDispatcher->deafen<ModuleStateChangeEvent, &Notifications::onModuleStateChange>(this);
    gFeatureManager->mDispatcher->deafen<ModuleScriptStateChangeEvent, &Notifications::onModuleScriptStateChange>(this);
    gFeatureManager->mDispatcher->deafen<ConnectionRequestEvent, &Notifications::onConnectionRequestEvent>(this);
}

bool CalcSize(ImVec2& boxSize, float& yOff, float& x, ImVec2 screenSize, Notification* notification) {
    // skidding, i don't even care i just want this to work :sob:
    float beginX = screenSize.x - boxSize.x - 10.f;
    float endX = screenSize.x + boxSize.x;

    x = MathUtils::lerp(endX, beginX, notification->mCurrentDuration);
    yOff = MathUtils::lerp(yOff , yOff - boxSize.y, notification->mCurrentDuration);

    if (x > screenSize.x + boxSize.x && yOff > screenSize.y + boxSize.y) return true;

    return false;
}

void Notifications::onRenderEvent(RenderEvent& event)
{
    FontHelper::pushPrefFont(true);
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();
    float delta = ImGui::GetIO().DeltaTime;

    // 古い通知を削除
    std::erase_if(mNotifications, [](const Notification& notification) {
        return notification.mIsTimeUp && notification.mTimeShown > notification.mDuration + 3.0f;
        });

    // モードごとの描画
    if (mStyle.mValue == Style::Solaris)
    {
        float y = displaySize.y - 10.0f;
        int colIndex = mNotifications.size() - 1;
        int i = 0;

        for (auto& notification : mNotifications)
        {
            if (i >= mMaxNotifications.mValue && mLimitNotifications.mValue) break;

            notification.mTimeShown += delta;
            notification.mIsTimeUp = notification.mTimeShown >= notification.mDuration;
            notification.mCurrentDuration = MathUtils::lerp(notification.mCurrentDuration,
                notification.mIsTimeUp ? 0.0f : 1.0f,
                delta * 5.0f);

            float percentDone = std::clamp(notification.mTimeShown / notification.mDuration, 0.0f, 1.0f);
            constexpr float fontSize = 20.0f;

            ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, notification.mMessage.c_str());
            ImVec2 boxSize(fmax(200.0f, 50 + textSize.x), textSize.y + 30.0f);

            float beginX = displaySize.x - boxSize.x - 10.f;
            float endX = displaySize.x + boxSize.x;
            float x = MathUtils::lerp(endX, beginX, notification.mCurrentDuration);
            y = MathUtils::lerp(y, y - boxSize.y, notification.mCurrentDuration);

            if (x > displaySize.x + boxSize.x && y > displaySize.y + boxSize.y) continue;

            ImColor themeColor = ColorUtils::getThemedColor(y * 2);
            if (notification.mType == Notification::Type::Warning)
                themeColor = ImColor(1.f, 0.8f, 0.f, 1.f);
            else if (notification.mType == Notification::Type::Error)
                themeColor = ImColor(1.f, 0.f, 0.f, 1.f);
            themeColor.Value.w = 0.7f;

            ImVec2 progMax(x + (boxSize.x * percentDone + 2.f), y + (boxSize.y - 10.f));
            progMax.x = std::clamp(progMax.x, x, x + boxSize.x);
            ImVec2 bgMin(x + boxSize.x * percentDone, y);
            ImVec2 bgMax(x + boxSize.x, y + (boxSize.y - 10.f));

            drawList->AddRectFilled(ImVec2(x, y), progMax, themeColor, 5.f);
            drawList->AddRectFilled(ImVec2(x + boxSize.x * percentDone - 2, y), bgMax,
                ImColor(0.f, 0.f, 0.f, 0.7f), 5.f);

            drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 10.f, y + 10.f),
                ImColor(255, 255, 255, 255), notification.mMessage.c_str());

            colIndex--;
            if (!notification.mIsTimeUp) i++;
        }
    }

    else if (mStyle.mValue == Style::Astra)
    {
        float y = displaySize.y - 10.0f;
        int i = 0;

        for (auto& notification : mNotifications)
        {
            if (i >= mMaxNotifications.mValue && mLimitNotifications.mValue) break;

            notification.mTimeShown += delta;
            notification.mIsTimeUp = notification.mTimeShown >= notification.mDuration;
            notification.mCurrentDuration = MathUtils::lerp(notification.mCurrentDuration,
                notification.mIsTimeUp ? 0.0f : 1.0f,
                delta * 5.0f);

            float percentDone = std::clamp(notification.mTimeShown / notification.mDuration, 0.0f, 1.0f);
            constexpr float fontSize = 20.0f;

            ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, notification.mMessage.c_str());
            ImVec2 boxSize(fmax(220.0f, 50 + textSize.x), textSize.y + 55.0f); // 高さ確保

            float targetX = displaySize.x - boxSize.x - 10.0f;
            float x = MathUtils::lerp(displaySize.x + boxSize.x, targetX, notification.mCurrentDuration);
            float spacing = 7.0f; // 通知同士の余白
            y = MathUtils::lerp(y, y - (boxSize.y + spacing), notification.mCurrentDuration);


            if (x > displaySize.x + boxSize.x && y > displaySize.y + boxSize.y) continue;

            // ---- 色設定 ----
            ImColor astraThemeColor = ColorUtils::getThemedColor(y * 2);
            if (notification.mType == Notification::Type::Warning)
                astraThemeColor = ImColor(1.f, 0.8f, 0.f, 1.f);
            else if (notification.mType == Notification::Type::Error)
                astraThemeColor = ImColor(1.f, 0.f, 0.f, 1.f);
            astraThemeColor.Value.w = 0.7f;

            // ---- 背景 ----
            ImVec2 bgMin(x, y);
            ImVec2 bgMax(x + boxSize.x, y + boxSize.y);
            drawList->AddRectFilled(bgMin, bgMax, ImColor(0.f, 0.f, 0.f, 0.9f), 0.f);

            // ---- 上のバー（固定装飾）----
            float topBarHeight = 5.0f;
            drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + boxSize.x, y + topBarHeight),
                astraThemeColor, 0.f, ImDrawFlags_RoundCornersTop);

            // ---- 固定タイトル ----
            drawList->AddText(ImGui::GetFont(), fontSize,
                ImVec2(x + 10.f, y + 10.f),
                ImColor(200, 200, 255, 255), "Module Toggled");

            // ---- プログレスバー（下部）----
            float barHeight = 5.0f; // プログレスバーの高さ
            ImVec2 progMin(x, y + boxSize.y - barHeight);
            ImVec2 progMax(x + boxSize.x * percentDone, y + boxSize.y);

            drawList->AddRectFilled(progMin, progMax, astraThemeColor, 0.f);


            // ---- テキスト ----
            drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 10.f, y + 35.f),
                ImColor(255, 255, 255, 255), notification.mMessage.c_str());

            i++;
        }
    }


    ImGui::PopFont();
}


void Notifications::onModuleStateChange(ModuleStateChangeEvent& event)
{
    if (event.isCancelled()) return;
    const auto notification = Notification(event.mModule->getName() + " was " + (event.mEnabled ? "enabled" : "disabled"), Notification::Type::Info, 3.0f);
    mNotifications.push_back(notification);
}

void Notifications::onModuleScriptStateChange(ModuleScriptStateChangeEvent& event)
{
    if (event.isCancelled()) return;
    const auto notification = Notification(event.mModule->moduleName + " was " + (event.mEnabled ? "enabled" : "disabled"), Notification::Type::Info, 3.0f);
    mNotifications.push_back(notification);
}

void Notifications::onConnectionRequestEvent(ConnectionRequestEvent& event)
{
  //  const auto notification = Notification("Connecting to " + *event.mServerAddress + "...", Notification::Type::Info, 6.0f);
//    mNotifications.push_back(notification);
}

void Notifications::onNotifyEvent(NotifyEvent& event)
{
    mNotifications.push_back(event.mNotification);
}
