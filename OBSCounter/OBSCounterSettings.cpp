#include "pch.h"
#include "OBSCounter.h"
#include "Maps.h"

std::string OBSCounter::GetPluginName() {
	return "OBS Counter Plugin";
}

void OBSCounter::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void OBSCounter::RenderSettings() {
    enableSettings();

    if (ImGui::CollapsingHeader("Color And Style", ImGuiTreeNodeFlags_None))
    {
        colorSettings();
        locationAndScaleSettings();
    }

    ImGui::Separator();

    ImGui::TextUnformatted("Displayed stats");

    CVarWrapper numStatsVar = cvarManager->getCvar("counter_ingame_numStats");

    if (!numStatsVar) {
        return;
    }

    int numStatsShown = numStatsVar.getIntValue();

    for (int i = 0; i < numStatsShown; i++) {
        statSettings(i);
    }

    addRemoveStatSettings();

    ImGui::Separator();

    ImGui::TextUnformatted("Time on offense/defense added thanks to a commission by samstlaurent");
    ImGui::TextUnformatted("Plugin made by JerryTheBee#1117 - DM me on discord for custom plugin commissions!");
}

void OBSCounter::enableSettings() {
    CVarWrapper enableVar = cvarManager->getCvar("counter_enable_ingame");

    if (!enableVar) {
        return;
    }

    bool enabled = enableVar.getBoolValue();

    if (ImGui::Checkbox("Enable overlay", &enabled)) {
        enableVar.setValue(enabled);
    }
}

void OBSCounter::colorSettings() {
    CVarWrapper enablebgVar = cvarManager->getCvar("counter_enable_background");

    if (!enablebgVar) {
        return;
    }

    bool enabledbg = enablebgVar.getBoolValue();

    if (ImGui::Checkbox("Enable Background", &enabledbg)) {
        enablebgVar.setValue(enabledbg);
    }

    CVarWrapper textColorVar = cvarManager->getCvar("counter_color");

    if (!textColorVar) {
        return;
    }

    float textColors[4];

    LinearColor textColor = textColorVar.getColorValue();

    ImVec4 colorVec;

    textColors[0] = textColor.R / 255;
    textColors[1] = textColor.G / 255;
    textColors[2] = textColor.B / 255;
    textColors[3] = textColor.A / 255;

    colorVec.x = textColor.R / 255;
    colorVec.y = textColor.G / 255;
    colorVec.z = textColor.B / 255;
    colorVec.w = textColor.A / 255;
    if (ImGui::ColorButton("Text Color##button", colorVec)) {
        ImGui::OpenPopup("Text Color selector");
    }

    ImGui::SameLine();

    ImGui::Text("Text Color");

    if (ImGui::BeginPopup("Text Color selector")) {
        if (ImGui::ColorPicker4("Text Color##selector", textColors)) {
            textColor.R = textColors[0] * 255;
            textColor.G = textColors[1] * 255;
            textColor.B = textColors[2] * 255;
            textColor.A = textColors[3] * 255;
            textColorVar.setValue(textColor);
        }

        ImGui::EndPopup();
    }

    if (enabledbg) {
        ImGui::SameLine();

        CVarWrapper bgColorVar = cvarManager->getCvar("counter_color_background");

        if (!bgColorVar) {
            return;
        }

        float bgColors[4];

        LinearColor bgColor = bgColorVar.getColorValue();

        ImVec4 bgColorVec;

        bgColors[0] = bgColor.R / 255;
        bgColors[1] = bgColor.G / 255;
        bgColors[2] = bgColor.B / 255;
        bgColors[3] = bgColor.A / 255;

        bgColorVec.x = bgColor.R / 255;
        bgColorVec.y = bgColor.G / 255;
        bgColorVec.z = bgColor.B / 255;
        bgColorVec.w = bgColor.A / 255;
        if (ImGui::ColorButton("Background Color##button", bgColorVec)) {
            ImGui::OpenPopup("Background Color selector");
        }

        ImGui::SameLine();

        ImGui::Text("Background Color");

        if (ImGui::BeginPopup("Background Color selector")) {
            if (ImGui::ColorPicker4("Background Color##selector", bgColors)) {
                bgColor.R = bgColors[0] * 255;
                bgColor.G = bgColors[1] * 255;
                bgColor.B = bgColors[2] * 255;
                bgColor.A = bgColors[3] * 255;
                bgColorVar.setValue(bgColor);
            }

            ImGui::EndPopup();
        }
    }
}

void OBSCounter::locationAndScaleSettings() {
    CVarWrapper xLocCvar = cvarManager->getCvar("counter_ingame_x_location");
    if (!xLocCvar) { return; }
    float xLoc = xLocCvar.getFloatValue();
    if (ImGui::SliderFloat("X Location", &xLoc, 0.0, 1.0)) {
        xLocCvar.setValue(xLoc);
    }
    CVarWrapper yLocCvar = cvarManager->getCvar("counter_ingame_y_location");
    if (!yLocCvar) { return; }
    float yLoc = yLocCvar.getFloatValue();
    if (ImGui::SliderFloat("Y Location", &yLoc, 0.0, 1.0)) {
        yLocCvar.setValue(yLoc);
    }
    CVarWrapper textScaleCvar = cvarManager->getCvar("counter_ingame_scale");
    if (!textScaleCvar) { return; }
    float textScale = textScaleCvar.getFloatValue();
    if (ImGui::SliderFloat("Text Scale", &textScale, 0.0, 10.0)) {
        textScaleCvar.setValue(textScale);
    }
    CVarWrapper decimalsCvar = cvarManager->getCvar("counter_decimals");
    if (!decimalsCvar) { return; }
    int decimals = decimalsCvar.getIntValue();
    if (ImGui::SliderInt("Counter Decimals", &decimals, 1, 10)) {
        decimalsCvar.setValue(decimals);
    }
}

void OBSCounter::statSettings(int renderIndex) {
    std::string renderIndexStr = std::to_string(renderIndex);

    CVarWrapper statIndexCvar = cvarManager->getCvar("counter_ingame_stat_" + renderIndexStr);
    if (!statIndexCvar) { return; }
    int statIndex = statIndexCvar.getIntValue();

    const char* statString = indexStringMapChar[statIndex];

    std::string headerName("Stat " + renderIndexStr);

    if (ImGui::CollapsingHeader(headerName.c_str())) {
        std::string statTopString = "Selected: " + indexStringMap[statIndex];
        ImGui::TextUnformatted(statTopString.c_str());

        CVarWrapper overlayStateCvar = cvarManager->getCvar("counter_ingame_stat_render_state_" + renderIndexStr);
        if (!overlayStateCvar) { return; }
        int overlayState = overlayStateCvar.getIntValue();

        bool overlayAverage = overlayState == RENDERSTATE_AVERAGE;

        std::string checkboxAverageLabel = "Average##stat" + renderIndexStr;

        if (ImGui::Checkbox(checkboxAverageLabel.c_str(), &overlayAverage)) {
            overlayStateCvar.setValue(overlayAverage & RENDERSTATE_AVERAGE);
        }

        ImGui::SameLine();

        bool overlayGame = overlayState == RENDERSTATE_GAME;

        std::string checkboxGameLabel = "Game##stat" + renderIndexStr;

        if (ImGui::Checkbox(checkboxGameLabel.c_str(), &overlayGame)) {
            if (overlayGame) {
                overlayStateCvar.setValue(RENDERSTATE_GAME);
            }
            else {
                overlayStateCvar.setValue(RENDERSTATE_DEFAULT);
            }

        }

        static int item_current_idx = 0; // Here we store our selection data as an index.
        std::string listBoxName("##Select stat" + renderIndexStr);
        if (ImGui::ListBoxHeader(listBoxName.c_str())) {
            for (int n = 0; n < IM_ARRAYSIZE(indexStringMapChar); n++) {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(indexStringMapChar[n], is_selected)) {
                    item_current_idx = n;
                    statIndexCvar.setValue(n);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::ListBoxFooter();
        }

        std::string statRenderName = "";

        if (overlayState == RENDERSTATE_DEFAULT) {
            statRenderName = indexStringMap[statIndex];
        } else if (overlayState == RENDERSTATE_AVERAGE) {
            statRenderName = averageStrings[statIndex];
        } else if (overlayState == RENDERSTATE_GAME) {
            statRenderName = indexStringMapGame[statIndex];
        }
        
        CVarWrapper overlayStatStringCvar = cvarManager->getCvar("counter_set_render_string_" + statRenderName);
        if (!overlayStatStringCvar) { return; }
        std::string overlayStatString = overlayStatStringCvar.getStringValue();

        char buffer[255] = "";

        if (ImGui::InputTextWithHint("Set New Render String", overlayStatString.c_str(), buffer, 255)) {
            overlayStatStringCvar.setValue(buffer);
        }
    }
}

void OBSCounter::addRemoveStatSettings() {
    CVarWrapper overlayNumCvar = cvarManager->getCvar("counter_ingame_numStats");
    if (!overlayNumCvar) { return; }
    int overlayNumStats = overlayNumCvar.getIntValue();

    if (ImGui::Button("Add Stat")) {
        overlayNumCvar.setValue(overlayNumStats + 1);
    }

    ImGui::SameLine();

    if (ImGui::Button("Remove Stat")) {
        overlayNumCvar.setValue(overlayNumStats - 1);
    }

    ImGui::SameLine();

    ImGui::TextUnformatted("Maximum 10 Stats");
}