//
// Created by vastrakai on 6/29/2024.
//

#include "Watermark.hpp"

#include <Hook/Hooks/RenderHooks/D3DHook.hpp>
#include <Features/Modules/Visual/Interface.hpp>
#include <Features/FeatureManager.hpp>

void Watermark::onEnable()
{
    mElement->mVisible = true;
}

void Watermark::onDisable()
{
    mElement->mVisible = false;
}

void Watermark::onRenderEvent(RenderEvent& event)
{
    static float anim = 0.f;
    anim = MathUtils::lerp(anim, mEnabled ? 1.f : 0.f, ImGui::GetIO().DeltaTime * 10.f);
    anim = MathUtils::clamp(anim, 0.f, 1.f);

    if (anim < 0.01f) return;

    auto renderPosition = mElement->getPos();
    renderPosition.x = MathUtils::lerp(-200.f, renderPosition.x, anim);
    renderPosition.y = MathUtils::lerp(-200.f, renderPosition.y, anim);


    if (mStyle.mValue == Style::SevenDays)
    {
        static std::string filePath = "seven_days.png";
        static ID3D11ShaderResourceView* texture;
        static bool loaded = false;
        static int width, height;
        if (!loaded)
        {
            D3DHook::loadTextureFromEmbeddedResource(filePath.c_str(), &texture, &width, &height);
            loaded = true;
            width /= 10;
            height /= 10;
        }

        mElement->mSize = { width, height };

        // Get the exact center-point of the image
        ImVec2 centeredImgPos = ImVec2(renderPosition.x + width / 2, renderPosition.y + height / 2);

        // Add a red shadow circle
        if (mGlow.mValue)
            ImGui::GetBackgroundDrawList()->AddShadowCircle(centeredImgPos, (width / 2) * 1.05f, ImColor(1.f, 0.f, 0.f, anim), 100, ImVec2(0.f, 0.f), 0, 100);

        ImGui::GetBackgroundDrawList()->AddImage(texture, renderPosition, ImVec2(renderPosition.x + width, renderPosition.y + height));
        return;
    }

    /*if (mStyle.mValue == Style::AstraVersion)
    {
        // --- �E�H�[�^�[�}�[�N�`�� ---
        static auto daInterface = gFeatureManager->mModuleManager->getModule<Interface>();
        if (!daInterface) return;

        auto fontSel = daInterface->mFont.as<Interface::FontType>();
        if (fontSel == Interface::FontType::ProductSans) {
            FontHelper::pushPrefFont(true, true);
        }
        else {
            FontHelper::pushPrefFont(true);
        }

        static std::string watermarkText = "Astra Client";
        static float size = 45.0f;

        ImVec2 renderPosition = mElement->getPos();

        for (int i = 0; i < watermarkText.length(); i++)
        {
            char c = watermarkText[i];
            ImVec2 charSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, &c, &c + 1);
            float charWidth = charSize.x;

            ImColor color = ColorUtils::getThemedColor(i * 100);

            if (mGlow.mValue)
                ImGui::GetBackgroundDrawList()->AddShadowCircle(
                    ImVec2(renderPosition.x + (charSize.x / 2), renderPosition.y + (charSize.y / 2)),
                    size / 3,
                    ImColor(color.Value.x, color.Value.y, color.Value.z, anim * 0.75f),
                    100, ImVec2(0.f, 0.f), 0, 12
                );

            ImColor shadowColor = ImColor(color.Value.x * 0.25f, color.Value.y * 0.25f, color.Value.z * 0.25f, 0.925f);
            ImVec2 shadowPos = renderPosition;
            shadowPos.x += 1.f;
            shadowPos.y += 1.f;

            ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, shadowPos, shadowColor, &c, &c + 1);
            ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, color, &c, &c + 1);

            renderPosition.x += charWidth;
        }

        ImGui::PopFont(); // �E�H�[�^�[�}�[�N�p�̃t�H���g��߂�

        // --- �E��o�[�W�����`�� ---
        {
            std::string versionText = "v1.0.0";
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            ImVec2 pos(displaySize.x - 10.0f, 10.0f);

            ImVec2 textSize = ImGui::CalcTextSize(versionText.c_str());
            pos.x -= textSize.x;

            // �t�H���g�w��Ȃ��� AddText ���g��
            ImGui::GetForegroundDrawList()->AddText(pos, IM_COL32(255, 255, 255, 255), versionText.c_str());
        }
        return;
    }*/

    static std::string versionText = "b8";
    static float versionScale = 0.7f;
    ImColor versionColor = ImColor(255, 255, 255, 255);

    auto drawWatermark = [&](std::string watermarkText, float size)
        {
            static auto daInterface = gFeatureManager->mModuleManager->getModule<Interface>();
            if (!daInterface) return;

            auto fontSel = daInterface->mFont.as<Interface::FontType>();
            if (fontSel == Interface::FontType::ProductSans)
                FontHelper::pushPrefFont(true, true);
            else
                FontHelper::pushPrefFont(true);

            ImVec2 pos = renderPosition;
            float totalWidth = 0.f;

            for (int i = 0; i < watermarkText.length(); i++)
            {
                char c = watermarkText[i];
                ImVec2 charSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.f, &c, &c + 1);
                float charWidth = charSize.x;

                ImColor color = ColorUtils::getThemedColor(i * 100);

                if (mGlow.mValue)
                    ImGui::GetBackgroundDrawList()->AddShadowCircle(
                        ImVec2(pos.x + charWidth / 2, pos.y + charSize.y / 2),
                        size / 3,
                        ImColor(color.Value.x, color.Value.y, color.Value.z, anim * 0.75f),
                        100, ImVec2(0.f, 0.f), 0, 12
                    );

                ImColor shadowColor = ImColor(color.Value.x * 0.25f, color.Value.y * 0.25f, color.Value.z * 0.25f, 0.925f);
                ImVec2 shadowPos = ImVec2(pos.x + 1.f, pos.y + 1.f);

                ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, shadowPos, shadowColor, &c, &c + 1);
                ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, pos, color, &c, &c + 1);

                pos.x += charWidth;
                totalWidth += charWidth;
            }

            float versionSize = size * versionScale;
            ImVec2 versionSizeVec = ImGui::GetFont()->CalcTextSizeA(versionSize, FLT_MAX, 0.f, versionText.c_str(), nullptr);

            ImVec2 versionPos = renderPosition;
            versionPos.x += totalWidth + 6.f;
            versionPos.y -= versionSizeVec.y * 0.2f;

            ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), versionSize, versionPos, versionColor, versionText.c_str());

            mElement->mSize = { totalWidth + versionSizeVec.x + 6.f, size };
            ImGui::PopFont();
        };

    switch (mStyle.mValue)
    {
    case Style::AstraVersion:
        drawWatermark("Astra Client", 45.f);
        return;
    }


    if (mStyle.mValue == Style::Packet)
{
    static auto daInterface = gFeatureManager->mModuleManager->getModule<Interface>();
    if (!daInterface) return;

    auto fontSel = daInterface->mFont.as<Interface::FontType>();
    if (fontSel == Interface::FontType::ProductSans) {
        FontHelper::pushPrefFont(true, true);
    } else {
        FontHelper::pushPrefFont(true);
    }

    static std::string watermarkText = "Packet"; // �� ������ Packet
    static float size = 45;

    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, watermarkText.c_str(), nullptr);
    mElement->mSize = { textSize.x, textSize.y };

    for (int i = 0; i < watermarkText.length(); i++)
    {
        char c = watermarkText[i];
        ImVec2 charSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, &c, &c + 1);
        float charWidth = charSize.x;

        ImColor color = ColorUtils::getThemedColor(i * 100);

        if (mGlow.mValue)
            ImGui::GetBackgroundDrawList()->AddShadowCircle(
                ImVec2(renderPosition.x + (charSize.x / 2), renderPosition.y + (charSize.y / 2)),
                size / 3,
                ImColor(color.Value.x, color.Value.y, color.Value.z, anim * 0.75f),
                100, ImVec2(0.f, 0.f), 0, 12
            );

        ImColor shadowColor = ImColor(color.Value.x * 0.25f, color.Value.y * 0.25f, color.Value.z * 0.25f, 0.925f);
        ImVec2 shadowPos = renderPosition;
        shadowPos.x += 1.f;
        shadowPos.y += 1.f;

        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, shadowPos, shadowColor, &c, &c + 1);
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, color, &c, &c + 1);

        renderPosition.x += charWidth;
    }

    ImGui::PopFont();
    return;
}


    if (mStyle.mValue == Style::PacketClient)
{
    static auto daInterface = gFeatureManager->mModuleManager->getModule<Interface>();
    if (!daInterface) return;

    auto fontSel = daInterface->mFont.as<Interface::FontType>();
    if (fontSel == Interface::FontType::ProductSans) {
        FontHelper::pushPrefFont(true, true);
    }
    else {
        FontHelper::pushPrefFont(true);
    }

    static std::string watermarkText = "Packet Client"; // �� ������ύX
    static float size = 45;

    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, watermarkText.c_str(), nullptr);
    mElement->mSize = { textSize.x, textSize.y };

    for (int i = 0; i < watermarkText.length(); i++)
    {
        char c = watermarkText[i];
        ImVec2 charSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, &c, &c + 1);
        float charWidth = charSize.x;

        ImColor color = ColorUtils::getThemedColor(i * 100);

        if (mGlow.mValue)
            ImGui::GetBackgroundDrawList()->AddShadowCircle(
                ImVec2(renderPosition.x + (charSize.x / 2), renderPosition.y + (charSize.y / 2)),
                size / 3,
                ImColor(color.Value.x, color.Value.y, color.Value.z, anim * 0.75f),
                100, ImVec2(0.f, 0.f), 0, 12
            );

        ImColor shadowColor = ImColor(color.Value.x * 0.25f, color.Value.y * 0.25f, color.Value.z * 0.25f, 0.925f);
        ImVec2 shadowPos = renderPosition;
        shadowPos.x += 1.f;
        shadowPos.y += 1.f;

        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, shadowPos, shadowColor, &c, &c + 1);
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, color, &c, &c + 1);

        renderPosition.x += charWidth;
    }

    ImGui::PopFont();
    return;
}

    if (mStyle.mValue == Style::aaaclient)
    {
        static auto daInterface = gFeatureManager->mModuleManager->getModule<Interface>();
        if (!daInterface) return;

        auto fontSel = daInterface->mFont.as<Interface::FontType>();
        if (fontSel == Interface::FontType::ProductSans) {
            FontHelper::pushPrefFont(true, true);
        }
        else {
            FontHelper::pushPrefFont(true);
        }

        static std::string watermarkText = "aaa client"; // �� ������ Packet
        static float size = 45;

        ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, watermarkText.c_str(), nullptr);
        mElement->mSize = { textSize.x, textSize.y };

        for (int i = 0; i < watermarkText.length(); i++)
        {
            char c = watermarkText[i];
            ImVec2 charSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, &c, &c + 1);
            float charWidth = charSize.x;

            ImColor color = ColorUtils::getThemedColor(i * 100);

            if (mGlow.mValue)
                ImGui::GetBackgroundDrawList()->AddShadowCircle(
                    ImVec2(renderPosition.x + (charSize.x / 2), renderPosition.y + (charSize.y / 2)),
                    size / 3,
                    ImColor(color.Value.x, color.Value.y, color.Value.z, anim * 0.75f),
                    100, ImVec2(0.f, 0.f), 0, 12
                );

            ImColor shadowColor = ImColor(color.Value.x * 0.25f, color.Value.y * 0.25f, color.Value.z * 0.25f, 0.925f);
            ImVec2 shadowPos = renderPosition;
            shadowPos.x += 1.f;
            shadowPos.y += 1.f;

            ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, shadowPos, shadowColor, &c, &c + 1);
            ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, color, &c, &c + 1);

            renderPosition.x += charWidth;
        }

        ImGui::PopFont();
        return;
    }

    if (mStyle.mValue == Style::Solstice)
    {
        static auto daInterface = gFeatureManager->mModuleManager->getModule<Interface>();
        if (!daInterface) return;

        auto fontSel = daInterface->mFont.as<Interface::FontType>();
        if (fontSel == Interface::FontType::ProductSans) {
            FontHelper::pushPrefFont(true, true);
        }
        else {
            FontHelper::pushPrefFont(true);
        }

        static std::string watermarkText = "solstice"; // �� ������ Packet
        static float size = 45;

        ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, watermarkText.c_str(), nullptr);
        mElement->mSize = { textSize.x, textSize.y };

        for (int i = 0; i < watermarkText.length(); i++)
        {
            char c = watermarkText[i];
            ImVec2 charSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, &c, &c + 1);
            float charWidth = charSize.x;

            ImColor color = ColorUtils::getThemedColor(i * 100);

            if (mGlow.mValue)
                ImGui::GetBackgroundDrawList()->AddShadowCircle(
                    ImVec2(renderPosition.x + (charSize.x / 2), renderPosition.y + (charSize.y / 2)),
                    size / 3,
                    ImColor(color.Value.x, color.Value.y, color.Value.z, anim * 0.75f),
                    100, ImVec2(0.f, 0.f), 0, 12
                );

            ImColor shadowColor = ImColor(color.Value.x * 0.25f, color.Value.y * 0.25f, color.Value.z * 0.25f, 0.925f);
            ImVec2 shadowPos = renderPosition;
            shadowPos.x += 1.f;
            shadowPos.y += 1.f;

            ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, shadowPos, shadowColor, &c, &c + 1);
            ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, color, &c, &c + 1);

            renderPosition.x += charWidth;
        }

        ImGui::PopFont();
        return;
    }


    static auto daInterface = gFeatureManager->mModuleManager->getModule<Interface>();
    if (!daInterface) return;

    auto fontSel = daInterface->mFont.as<Interface::FontType>();
    if (fontSel == Interface::FontType::ProductSans) {
        FontHelper::pushPrefFont(true, true);
    }
    else {
        FontHelper::pushPrefFont(true);
    }

    static std::string watermarkText = "Astra Client";
    static float size = 45;

    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, watermarkText.c_str(), nullptr);
    mElement->mSize = { textSize.x, textSize.y };




    for (int i = 0; i < watermarkText.length(); i++)
    {
        // get the character
        char c = watermarkText[i];
        // get the width of the character
        ImVec2 charSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, &c, &c + 1);
        float charWidth = charSize.x;

        ImColor color = ColorUtils::getThemedColor(i * 100);

        if (mGlow.mValue)
            ImGui::GetBackgroundDrawList()->AddShadowCircle(ImVec2(renderPosition.x + (charSize.x / 2), renderPosition.y + (charSize.y / 2)),
                                                            size / 3, ImColor(color.Value.x, color.Value.y, color.Value.z, anim * 0.75f), 100, ImVec2(0.f, 0.f), 0, 12);
        // draw a shadow
        ImColor shadowColor = ImColor(color.Value.x * 0.25f, color.Value.y * 0.25f, color.Value.z * 0.25f, 0.925f);
        ImVec2 shadowPos = renderPosition;

        shadowPos.x += 1.f;
        shadowPos.y += 1.f;
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, shadowPos, shadowColor, &c, &c + 1);

        // draw the character
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, color, &c, &c + 1);
        // move the render position to the right
        renderPosition.x += charWidth;
    }

    ImGui::PopFont();
}
