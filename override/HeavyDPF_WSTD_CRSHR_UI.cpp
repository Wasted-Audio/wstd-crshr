/**
 * Copyright (c) Wasted Audio 2023 - GPL-3.0-or-later
 */

#include "DistrhoUI.hpp"
#include "ResizeHandle.hpp"
#include "veramobd.hpp"
#include "wstdcolors.hpp"

START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------
class ImGuiPluginUI : public UI
{
    int fcrshr = 512;
    float fmix = 50.0f;

    ResizeHandle fResizeHandle;

    // ----------------------------------------------------------------------------------------------------------------

public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
    ImGuiPluginUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true),
          fResizeHandle(this)
    {
        setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);

        ImGuiIO& io(ImGui::GetIO());

        ImFontConfig fc;
        fc.FontDataOwnedByAtlas = true;
        fc.OversampleH = 1;
        fc.OversampleV = 1;
        fc.PixelSnapH = true;

        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 16.0f * getScaleFactor(), &fc);
        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 21.0f * getScaleFactor(), &fc);
        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 12.5f * getScaleFactor(), &fc);
        io.Fonts->Build();
        io.FontDefault = io.Fonts->Fonts[1];

        fResizeHandle.hide();
    }

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

   /**
      A parameter has changed on the plugin side.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
        switch (index) {
            case 0:
                fcrshr = value;
                break;
            case 1:
                fmix = value;
                break;

            default: return;
        }

        repaint();
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks

   /**
      ImGui specific onDisplay function.
    */
    void onImGuiDisplay() override
    {
        const float width = getWidth();
        const float height = getHeight();
        const float margin = 0.0f;
        auto scaleFactor = getScaleFactor();

        ImGui::SetNextWindowPos(ImVec2(margin, margin));
        ImGui::SetNextWindowSize(ImVec2(width - 2 * margin, height - 2 * margin));

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.ChildBorderSize = 1.0f * scaleFactor;

        style.Colors[ImGuiCol_TitleBgActive] = (ImVec4)WstdTitleBgActive;
        style.Colors[ImGuiCol_WindowBg] = (ImVec4)WstdWindowBg;

        ImGuiIO& io(ImGui::GetIO());
        ImFont* defaultFont = ImGui::GetFont();
        ImFont* titleBarFont = io.Fonts->Fonts[2];
        ImFont* mediumFont = io.Fonts->Fonts[3];

        auto intense        = (pow((float)fcrshr * -1.0f + 514.0f, 2.0f) / 2500.0f - 20.0f) / 5.0f;
        auto CrshrActive    = ColorBright(Green, intense);
        auto CrshrHovered   = ColorBright(GreenBr, intense);
        auto MixActive      = ColorMix(Green, Yellow, intense, fmix);
        auto MixHovered     = ColorMix(GreenBr, YellowBr, intense, fmix);

        const float hundred = 100 * scaleFactor;

        auto crshstep = 8;
        auto percstep = 1.0f;

        if (io.KeyShift)
        {
            crshstep = 1;
            percstep = 0.1f;
        }

        ImGui::PushFont(titleBarFont);
        if (ImGui::Begin("WSTD CRSHR", nullptr, ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoCollapse + ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::Dummy(ImVec2(0.0f, 6.0f * scaleFactor));
            ImGui::PushFont(defaultFont);
            auto ImGuiKnob_Flags = ImGuiKnobFlags_DoubleClickReset + ImGuiKnobFlags_ValueTooltip + ImGuiKnobFlags_NoInput + ImGuiKnobFlags_ValueTooltipHideOnClick;
            auto ImGuiKnob_FlagsLog = ImGuiKnob_Flags + ImGuiKnobFlags_Logarithmic;

            ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)CrshrActive);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)CrshrHovered);
            if (ImGuiKnobs::KnobInt(
                "Crshr", &fcrshr, 2, 512, crshstep, "%i",
                ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_FlagsLog, 9))
            {
                if (ImGui::IsItemActivated())
                {
                    editParameter(0, true);
                    if (ImGui::IsMouseDoubleClicked(0))
                        fcrshr = 512;
                }
                setParameterValue(0, fcrshr);
            }
            ImGui::PopStyleColor(2);
            ImGui::SameLine();


            ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)MixActive);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)MixHovered);
            if (ImGuiKnobs::Knob(
                "Mix", &fmix, 0.0f, 100.0f, percstep, "%.1f%%", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_Flags, 11))
            {
                if (ImGui::IsItemActivated())
                {
                    editParameter(1, true);
                    if (ImGui::IsMouseDoubleClicked(0))
                        fmix = 50.0f;
                }
                setParameterValue(1, fmix);
            }
            ImGui::PopStyleColor(2);

            if (ImGui::IsItemDeactivated())
            {
                editParameter(0, false);
                editParameter(1, false);
            }
            ImGui::PopFont();
        }
        ImGui::PopFont();
        ImGui::End();
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiPluginUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new ImGuiPluginUI();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
