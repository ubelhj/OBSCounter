#include "pch.h"
#include "OBSCounter.h"

bool inDragMode = false;

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

    LinearColor textColor = textColorVar.getColorValue();

    float textColors[4] = { textColor.R / 255, textColor.G / 255, textColor.B / 255, textColor.A / 255 };
    ImVec4 colorVec = { textColor.R / 255, textColor.G / 255, textColor.B / 255, textColor.A / 255 };
    
    if (ImGui::ColorButton("Text Color##button", colorVec)) {
        ImGui::OpenPopup("Text Color selector");
    }

    ImGui::SameLine();

    ImGui::Text("Text Color");

    if (ImGui::BeginPopup("Text Color selector")) {
        if (ImGui::ColorPicker4("Text Color##selector", textColors)) {
            textColor = { textColors[0] * 255, textColors[1] * 255, textColors[2] * 255, textColors[3] * 255 };
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

    // location dragger
    if (!enabledOverlay) return;
    ImGui::Checkbox("Drag Mode", &inDragMode);

    if (inDragMode) {
        ImVec2 mousePos = ImGui::GetMousePos();

        if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()) {
            return;
        }
        ImGui::SetMouseCursor(2);
        if (ImGui::IsMouseDown(0)) {
            float newX = mousePos.x / screenSize.X;
            float newY = mousePos.y / screenSize.Y;

            xLocCvar.setValue(newX);
            yLocCvar.setValue(newY);
        }
    }
   
}

void OBSCounter::statSettings(int renderIndex) {
    std::string renderIndexStr = std::to_string(renderIndex);

    CVarWrapper statIndexCvar = cvarManager->getCvar("counter_ingame_stat_" + renderIndexStr);
    if (!statIndexCvar) { return; }
    int statIndex = statIndexCvar.getIntValue();

    CVarWrapper statIndexCareerCvar = cvarManager->getCvar("counter_ingame_stat_career_" + renderIndexStr);
    if (!statIndexCareerCvar) { return; }
    int statIndexCareer = statIndexCareerCvar.getIntValue();

    CVarWrapper overlayStateCvar = cvarManager->getCvar("counter_ingame_stat_render_state_" + renderIndexStr);
    if (!overlayStateCvar) { return; }
    int overlayState = overlayStateCvar.getIntValue();

    std::string statRenderName = "";
    const char* statString;

    switch (overlayState) {
    case STAT_DEFAULT:
        statRenderName = indexStringMap[statIndex];
        statString = indexStringMapChar[statIndex];
        break;
    case STAT_AVERAGE:
        statRenderName = averageStrings[statIndex];
        statString = indexStringMapChar[statIndex];
        break;
    case STAT_GAME:
        statRenderName = indexStringMapGame[statIndex];
        statString = indexStringMapChar[statIndex];
        break;
    case STAT_OTHER:
        statRenderName = indexStringMapOther[statIndex];
        statString = indexStringMapChar[statIndex];
        break;
    case STAT_CAREER_TOTAL:
        statRenderName = "total" + indexStringMapCareer[statIndexCareer];
        statString = indexStringMapCareerChar[statIndexCareer];
        break;
    case STAT_CAREER_PRIVATE:
        statRenderName = "private" + indexStringMapCareer[statIndexCareer];
        statString = indexStringMapCareerChar[statIndexCareer];
        break;
    case STAT_CAREER_RANKED:
        statRenderName = "ranked" + indexStringMapCareer[statIndexCareer];
        statString = indexStringMapCareerChar[statIndexCareer];
        break;
    case STAT_CAREER_CASUAL:
        statRenderName = "casual" + indexStringMapCareer[statIndexCareer];
        statString = indexStringMapCareerChar[statIndexCareer];
        break;
    case STAT_CAREER_AVERAGE:
        statRenderName = "average" + indexStringMapCareer[statIndexCareer];
        statString = indexStringMapCareerChar[statIndexCareer];
        break;
    case STAT_TEAM:
        statRenderName = "team" + indexStringMap[statIndex];
        statString = indexStringMapChar[statIndex];
        break;
    case STAT_TEAM_OPPONENT:
        statRenderName = "opponent" + indexStringMap[statIndex];
        statString = indexStringMapChar[statIndex];
        break;
    default:
        break;
    }

    std::string headerName("Stat " + statRenderName + "###statheader" + renderIndexStr);

    if (ImGui::CollapsingHeader(headerName.c_str())) {
        std::string checkboxDefaultLabel = "Session Total##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxDefaultLabel.c_str(), &overlayState, STAT_DEFAULT)) {
            overlayStateCvar.setValue(STAT_DEFAULT);
        }
        ImGui::SameLine();

        std::string checkboxAverageLabel = "Session Average##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxAverageLabel.c_str(), &overlayState, STAT_AVERAGE)) {
            overlayStateCvar.setValue(STAT_AVERAGE);
        }
        ImGui::SameLine();


        std::string checkboxGameLabel = "Current Game##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxGameLabel.c_str(), &overlayState, STAT_GAME)) {
            overlayStateCvar.setValue(STAT_GAME);
        }
        ImGui::SameLine();

        std::string checkboxOtherLabel = "Other##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxOtherLabel.c_str(), &overlayState, STAT_OTHER)) {
            overlayStateCvar.setValue(STAT_OTHER);
        }

        std::string checkboxCareerTotalLabel = "Career Total##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxCareerTotalLabel.c_str(), &overlayState, STAT_CAREER_TOTAL)) {
            overlayStateCvar.setValue(STAT_CAREER_TOTAL);
        }
        ImGui::SameLine();
        std::string checkboxCareerPrivateLabel = "Career Private##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxCareerPrivateLabel.c_str(), &overlayState, STAT_CAREER_PRIVATE)) {
            overlayStateCvar.setValue(STAT_CAREER_PRIVATE);
        }
        ImGui::SameLine();
        std::string checkboxCareerRankedLabel = "Career Ranked##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxCareerRankedLabel.c_str(), &overlayState, STAT_CAREER_RANKED)) {
            overlayStateCvar.setValue(STAT_CAREER_RANKED);
        }
        ImGui::SameLine();
        std::string checkboxCareerCasualLabel = "Career Casual##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxCareerCasualLabel.c_str(), &overlayState, STAT_CAREER_CASUAL)) {
            overlayStateCvar.setValue(STAT_CAREER_CASUAL);
        }
        ImGui::SameLine();
        std::string checkboxCareerAverageLabel = "Career Average##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxCareerAverageLabel.c_str(), &overlayState, STAT_CAREER_AVERAGE)) {
            overlayStateCvar.setValue(STAT_CAREER_AVERAGE);
        }
        std::string checkboxCareerTeamLabel = "Session My Team##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxCareerTeamLabel.c_str(), &overlayState, STAT_TEAM)) {
            overlayStateCvar.setValue(STAT_TEAM);
        }
        ImGui::SameLine();
        std::string checkboxCareerOpponentLabel = "Session Opponent Team##stat" + renderIndexStr;
        if (ImGui::RadioButton(checkboxCareerOpponentLabel.c_str(), &overlayState, STAT_TEAM_OPPONENT)) {
            overlayStateCvar.setValue(STAT_TEAM_OPPONENT);
        }

        std::string listBoxName("##Select stat" + renderIndexStr);
        if (ImGui::ListBoxHeader(listBoxName.c_str())) {
            int maxSize = 0;
            const char** statStrings = nullptr;
            int indexUsed = statIndex;
            CVarWrapper cvarIndexUsed = statIndexCvar;

            switch (overlayState) {
                case STAT_OTHER:
                    maxSize = numOtherStats;
                    statStrings = indexStringMapOtherChar;
                    break;
                case STAT_CAREER_TOTAL:
                case STAT_CAREER_PRIVATE:
                case STAT_CAREER_RANKED:
                case STAT_CAREER_CASUAL:
                case STAT_CAREER_AVERAGE:
                    maxSize = NUMCAREERSTATS;
                    statStrings = indexStringMapCareerChar;
                    indexUsed = statIndexCareer;
                    cvarIndexUsed = statIndexCareerCvar;
                    break;
                case STAT_TEAM:
                case STAT_TEAM_OPPONENT:
                    maxSize = endNormalStats;
                    statStrings = indexStringMapChar;
                    break;
                default:
                    maxSize = numStats;
                    statStrings = indexStringMapChar;
                    break;
            }

            for (int n = 0; n < maxSize; n++) {
                const bool is_selected = (indexUsed == n);
                
                if (ImGui::Selectable(statStrings[n], is_selected)) {
                    cvarIndexUsed.setValue(n);
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

        std::string renderStringLabel = "Set New Render String##" + renderIndexStr;
        if (ImGui::InputTextWithHint(renderStringLabel.c_str(), overlayStatString.c_str(), buffer, 255)) {
            overlayStatStringCvar.setValue(buffer);
        }

        if (overlayState == STAT_CAREER_TOTAL) {
            CVarWrapper offsetCvar = cvarManager->getCvar("counter_career_offset_" + indexStringMapCareer[statIndexCareer]);
            if (!offsetCvar) { return; }
            int offset = offsetCvar.getIntValue();

            std::string offsetLabel = "Add offset to total##" + renderIndexStr;

            if (ImGui::InputInt(offsetLabel.c_str(), &offset)) {
                offsetCvar.setValue(offset);
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Use this to add career stats from other platforms if you want");
            }
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

    if (ImGui::Button("Copy Demolition leaderboard update message. Join at discord.gg/kNnDxuC")) {
        std::string clipboardText( "/update demolitions: " + std::to_string(careerStatTotal[Demolish]) 
            + " exterminations: " + std::to_string(careerStatTotal[Demolition]));
        ImGui::SetClipboardText(clipboardText.c_str());
    }
}