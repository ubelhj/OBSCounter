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

    if (ImGui::Button("Add a Game")) {
        cvarManager->executeCommand("counter_add_game");
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Adds a game and resets gameStats, useful if games tracking gets out of sync");
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset")) {
        cvarManager->executeCommand("counter_reset");
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Resets all stats, including games to 0");
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
    if (ImGui::SliderInt("Decimals for averages", &decimals, 1, 5)) {
        decimalsCvar.setValue(decimals);
    }
}

void OBSCounter::statSettings(int renderIndex) {
    std::string renderIndexStr = std::to_string(renderIndex);

    CVarWrapper statIndexCvar = cvarManager->getCvar("counter_ingame_stat_" + renderIndexStr);
    if (!statIndexCvar) { return; }
    int statIndex = statIndexCvar.getIntValue();

    const char* statString = indexStringMapChar[statIndex];

    CVarWrapper overlayStateCvar = cvarManager->getCvar("counter_ingame_stat_render_state_" + renderIndexStr);
    if (!overlayStateCvar) { return; }
    int overlayState = overlayStateCvar.getIntValue();

    std::string statRenderName = "";

    switch (overlayState) {
    case RENDERSTATE_DEFAULT:
        statRenderName = indexStringMap[statIndex];
        break;
    case RENDERSTATE_AVERAGE:
        statRenderName = averageStrings[statIndex];
        break;
    case RENDERSTATE_GAME:
        statRenderName = indexStringMapGame[statIndex];
        break;
    case RENDERSTATE_OTHER:
        statRenderName = indexStringMapOther[statIndex];
        break;
    default:
        break;
    }

    std::string headerName("Stat " + statRenderName + "###statheader" + renderIndexStr);

    if (ImGui::CollapsingHeader(headerName.c_str())) {
        std::string checkboxDefaultLabel = "Default##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxDefaultLabel.c_str(), &overlayState, RENDERSTATE_DEFAULT)) {
            overlayStateCvar.setValue(RENDERSTATE_DEFAULT);
        }
        ImGui::SameLine();

        std::string checkboxAverageLabel = "Average##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxAverageLabel.c_str(), &overlayState, RENDERSTATE_AVERAGE)) {
            overlayStateCvar.setValue(RENDERSTATE_AVERAGE);
        }
        ImGui::SameLine();


        std::string checkboxGameLabel = "Game##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxGameLabel.c_str(), &overlayState, RENDERSTATE_GAME)) {
            overlayStateCvar.setValue(RENDERSTATE_GAME);
        }
        ImGui::SameLine();

        std::string checkboxOtherLabel = "Other##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxOtherLabel.c_str(), &overlayState, RENDERSTATE_OTHER)) {
            overlayStateCvar.setValue(RENDERSTATE_OTHER);
        }

        std::string listBoxName("##Select stat" + renderIndexStr);
        if (ImGui::ListBoxHeader(listBoxName.c_str())) {
            int maxSize = 0;
            const char** statStrings = nullptr;

            switch (overlayState) {
            case RENDERSTATE_OTHER:
                maxSize = numOtherStats;
                statStrings = indexStringMapOtherChar;
                break;
            default:
                maxSize = numStats;
                statStrings = indexStringMapChar;
                break;
            }

            for (int n = 0; n < maxSize; n++) {
                const bool is_selected = (statIndex == n);
                
                if (ImGui::Selectable(statStrings[n], is_selected)) {
                    statIndexCvar.setValue(n);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            
            ImGui::ListBoxFooter();
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