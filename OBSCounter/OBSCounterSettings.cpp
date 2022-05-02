#include "pch.h"
#include "OBSCounter.h"

std::string OBSCounter::GetPluginName() {
    return "OBS Counter Plugin";
}

void OBSCounter::RenderSettings() {
    enableSettings();

    if (ImGui::CollapsingHeader("Color And Style", ImGuiTreeNodeFlags_None)) {
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
    auto overlayEnableVar = cvarManager->getCvar("counter_enable_overlay");
    if (!overlayEnableVar) { return; }
    overlayEnabled = overlayEnableVar.getBoolValue();
    if (ImGui::Checkbox("Enable Overlay", &overlayEnabled)) {
        overlayEnableVar.setValue(std::to_string(overlayEnabled));
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

    LinearColor textColor = textColorVar.getColorValue() / 255;

    if (ImGui::ColorEdit4("Text Color##button", &textColor.R, ImGuiColorEditFlags_NoInputs)) {
        textColorVar.setValue(textColor * 255);
    }

    ImGui::SameLine();

    CVarWrapper bgColorVar = cvarManager->getCvar("counter_color_background");

    if (!bgColorVar) {
        return;
    }

    LinearColor bgColor = bgColorVar.getColorValue() / 255;

    if (ImGui::ColorEdit4("Background Color##button", &bgColor.R, ImGuiColorEditFlags_NoInputs)) {
        bgColorVar.setValue(bgColor * 255);
    }

    ImGui::SameLine();

    CVarWrapper outlineColorVar = cvarManager->getCvar("counter_color_outline");

    if (!outlineColorVar) {
        return;
    }

    LinearColor outlineColor = outlineColorVar.getColorValue() / 255;

    if (ImGui::ColorEdit4("Outline Color##button", &outlineColor.R, ImGuiColorEditFlags_NoInputs)) {
        outlineColorVar.setValue(outlineColor * 255);
    }
}

void OBSCounter::locationAndScaleSettings() {
    // location dragger
    ImGui::Checkbox("Enable moving the overlay", &inDragMode);

    CVarWrapper textScaleCvar = cvarManager->getCvar("counter_ingame_scale");
    if (!textScaleCvar) { return; }
    float textScale = textScaleCvar.getFloatValue();
    if (ImGui::SliderFloat("Text Scale", &textScale, 0.0, 10.0)) {
        textScaleCvar.setValue(textScale);
    }

    CVarWrapper outlineRoundingCvar = cvarManager->getCvar("counter_outline_rounding");
    if (!outlineRoundingCvar) { return; }
    float outlineRounding = outlineRoundingCvar.getFloatValue();
    if (ImGui::SliderFloat("Outline Roundness", &outlineRounding, 0.0, 15.0)) {
        outlineRoundingCvar.setValue(outlineRounding);
    }

    CVarWrapper outlineSizeCvar = cvarManager->getCvar("counter_outline_size");
    if (!outlineSizeCvar) { return; }
    float outlineSize = outlineSizeCvar.getFloatValue();
    if (ImGui::SliderFloat("Outline Size", &outlineSize, 0.0, 10.0)) {
        outlineSizeCvar.setValue(outlineSize);
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