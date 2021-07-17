#include "pch.h"
#include "OBSCounter.h"

const char * indexStringMap2[] = {
    "wins",
    "losses",
    "mvps",
    "games",
    "goals",
    "demolitions",
    "deaths",
    "exterminations",
    "aerialGoals",
    "backwardsGoals",
    "bicycleGoals",
    "longGoals",
    "turtleGoals",
    "poolShots",
    "overtimeGoals",
    "hatTricks",
    "assists",
    "playmakers",
    "saves",
    "epicSaves",
    "saviors",
    "shots",
    "centers",
    "clears",
    "firstTouchs",
    "damages",
    "ultraDamages",
    "lowFives",
    "highFives",
    "swishs",
    "bicycleHits",
    "points",
    "timePlayed",
    "offenseTime",
    "defenseTime",
    "gameGoals",
    "gameDemolitions",
    "gameDeaths",
    "gameExterminations",
    "gameAerialGoals",
    "gameBackwardsGoals",
    "gameBicycleGoals",
    "gameLongGoals",
    "gameTurtleGoals",
    "gamePoolShots",
    "gameOvertimeGoals",
    "gameHatTricks",
    "gameAssists",
    "gamePlaymakers",
    "gameSaves",
    "gameEpicSaves",
    "gameSaviors",
    "gameShots",
    "gameCenters",
    "gameClears",
    "gameFirstTouchs",
    "gameDamages",
    "gameUltraDamages",
    "gameLowFives",
    "gameHighFives",
    "gameSwishs",
    "gameBicycleHits",
    "gamePoints",
    "gameTimePlayed",
    "gameOffenseTime",
    "gameDefenseTime"
};

std::string OBSCounter::GetPluginName() {
	return "OBSCounter";
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

    ImGui::TextUnformatted("Displayed stats");

    CVarWrapper numStatsVar = cvarManager->getCvar("counter_ingame_numStats");

    if (!numStatsVar) {
        return;
    }

    int numStatsShown = numStatsVar.getIntValue();

    for (int i = 0; i < numStatsShown; i++) {
        statSettings(i);
    }
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
    CVarWrapper statIndexCvar = cvarManager->getCvar("counter_ingame_stat_" + std::to_string(renderIndex));
    if (!statIndexCvar) { return; }
    int statIndex = statIndexCvar.getIntValue();

    const char* statString = indexStringMap2[statIndex];

    std::string headerName("Stat " + std::to_string(renderIndex));

    if (ImGui::CollapsingHeader(headerName.c_str())) {
        ImGui::TextUnformatted("select stat");

        static int item_current_idx = 0; // Here we store our selection data as an index.
        std::string listBoxName("##Select stat" + std::to_string(renderIndex));
        if (ImGui::ListBoxHeader(listBoxName.c_str()))
        {
            for (int n = 0; n < IM_ARRAYSIZE(indexStringMap2); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(indexStringMap2[n], is_selected)) {
                    item_current_idx = n;
                    statIndexCvar.setValue(n);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::ListBoxFooter();
        }
    }
}