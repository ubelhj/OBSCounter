#include "pch.h"
#include "OBSCounter.h"

// Renders in game overlay
void OBSCounter::Render() {
    ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoFocusOnAppearing;

    if (!inDragMode) {
        WindowFlags |= ImGuiWindowFlags_NoInputs;
    }

    if (!enabledOverlayBackground) {
        WindowFlags |= ImGuiWindowFlags_NoBackground;
    }

    ImVec4 bgColorVec = { overlayBackgroundColor.R / 255, overlayBackgroundColor.G / 255,
        overlayBackgroundColor.B / 255, overlayBackgroundColor.A / 255 };
    ImVec4 textColorVec = { overlayColor.R / 255, overlayColor.G / 255,
        overlayColor.B / 255, overlayColor.A / 255 };
    ImVec4 outlineColorVec = { overlayOutlineColor.R / 255, overlayOutlineColor.G / 255,
        overlayOutlineColor.B / 255, overlayOutlineColor.A / 255 };

    ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColorVec);
    ImGui::PushStyleColor(ImGuiCol_Text, textColorVec);
    ImGui::PushStyleColor(ImGuiCol_Border, outlineColorVec);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, outlineSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, outlineRounding);

    ImGui::SetNextWindowBgAlpha(bgColorVec.w);

    if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, WindowFlags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
        return;
    }

    renderAllStrings();

    ImGui::SetWindowFontScale(scale);

    // First ensure the font is actually loaded
    if (!font) {
        auto gui = gameWrapper->GetGUIManager();
        font = gui.GetFont("OBSCounter");
    }

    // Now use the font
    if (font) {
        ImGui::PushFont(font);
    }

    for (int i = 0; i < overlayLines; i++) {
        // locates based on screen and font size
        ImGui::Text(overlayStrings[i].c_str());
    }

    if (font) {
        ImGui::PopFont();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    ImGui::End();

    if (!isWindowOpen_)
    {
        cvarManager->executeCommand("togglemenu " + GetMenuName());
    }
}

std::string OBSCounter::statToRenderString(int index, int state) {
    if (state >= STAT_END || state < 0 || index < 0) {
        return "INVALID STATE";
    }

    std::ostringstream strStream;

    switch (state) {
    case STAT_DEFAULT:
        if (index >= NUMSTATS) {
            return "INVALID STATE";
        }
        // writes time stats
        if (index > ENDNORMALSTATS) {
            int totalSeconds = statArray[index];
            // writes the stat
            strStream << totalSeconds / 60;
            strStream << ":";
            int remSeconds = totalSeconds % 60;
            if (remSeconds < 10) {
                strStream << "0";
            }
            strStream << remSeconds;
        } else {
            strStream << statArray[index];
        }

        return renderStringsStandard[index] + strStream.str();
    case STAT_AVERAGE:
        if (index >= NUMSTATS) {
            return "INVALID STATE";
        }
        if (index > ENDNORMALSTATS) {
            int totalSeconds = averages[index];
            // writes the stat
            strStream << totalSeconds / 60;
            strStream << ":";
            int remSeconds = totalSeconds % 60;
            if (remSeconds < 10) {
                strStream << "0";
            }
            strStream << remSeconds;
        } else {
            strStream << std::fixed << std::setprecision(decimalPlaces);
            strStream << averages[index];
        }
        return renderStringsAverage[index] + strStream.str();
    case STAT_GAME:
        if (index >= NUMSTATS) {
            return "INVALID STATE";
        }
        // writes time stats
        if (index > ENDNORMALSTATS) {
            int totalSeconds = statArrayGame[index];
            // writes the stat
            strStream << totalSeconds / 60;
            strStream << ":";
            int remSeconds = totalSeconds % 60;
            if (remSeconds < 10) {
                strStream << "0";
            }
            strStream << remSeconds;
        } else {
            strStream << statArrayGame[index];
        }

        return renderStringsGame[index] + strStream.str();
    case STAT_OTHER:
        if (index >= NUMOTHERSTATS) {
            return renderStringsOther[0] + statArrayOther[0];
        } else {
            return renderStringsOther[index] + statArrayOther[index];
        }
    case STAT_CAREER_TOTAL:
        if (index >= NUMCAREERSTATS) {
            return "INVALID STATE";
        }
        strStream << careerStatTotal[index];

        return renderStringsCareerTotal[index] + strStream.str();
    case STAT_CAREER_PRIVATE:
        if (index >= NUMCAREERSTATS) {
            return "INVALID STATE";
        }
        strStream << careerStatPrivate[index];

        return renderStringsCareerPrivate[index] + strStream.str();
    case STAT_CAREER_RANKED:
        if (index >= NUMCAREERSTATS) {
            return "INVALID STATE";
        }
        strStream << careerStatRanked[index];

        return renderStringsCareerRanked[index] + strStream.str();
    case STAT_CAREER_CASUAL:
        if (index >= NUMCAREERSTATS) {
            return "INVALID STATE";
        }
        strStream << careerStatCasual[index];

        return renderStringsCareerCasual[index] + strStream.str();
    case STAT_CAREER_AVERAGE:
        if (index >= NUMCAREERSTATS) {
            return "INVALID STATE";
        }
        strStream << std::fixed << std::setprecision(decimalPlaces);
        strStream << careerStatAverage[index];

        return renderStringsCareerAverage[index] + strStream.str();
    case STAT_TEAM:
        // writes time stats
        if (index >= ENDNORMALSTATS) {
            // time stats don't exist for teams
            return "INVALID STATE";
        } else {
            strStream << statArrayTeam[index];
        }

        return renderStringsTeam[index] + strStream.str();
    case STAT_TEAM_OPPONENT:
        // writes time stats
        if (index >= ENDNORMALSTATS) {
            // time stats don't exist for teams
            return "INVALID STATE";
        } else {
            strStream << statArrayOpponent[index];
        }

        return renderStringsOpponent[index] + strStream.str();
    default:
        return "INVALID STATE";
    }
}

void OBSCounter::renderAllStrings() {
    for (int i = 0; i < overlayStrings.size(); i++) {
        auto state = overlayStates[i];
        if (state > STAT_CAREER_START && state <= STAT_TEAM_START) {
            overlayStrings[i] = statToRenderString(overlayStatsCareer[i], state);
        } else {
            overlayStrings[i] = statToRenderString(overlayStats[i], state);
        }
    }
}

// Name of the menu that is used to toggle the window.
std::string OBSCounter::GetMenuName()
{
    return "OBSCounter";
}

// Title to give the menu
std::string OBSCounter::GetMenuTitle()
{
    return menuTitle_;
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool OBSCounter::ShouldBlockInput()
{
    return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool OBSCounter::IsActiveOverlay()
{
    return false;
}

// Called when window is opened
void OBSCounter::OnOpen()
{
    isWindowOpen_ = true;
}

// Called when window is closed
void OBSCounter::OnClose() {
    isWindowOpen_ = false;
}

void OBSCounter::SetImGuiContext(uintptr_t ctx) {
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));

    auto gui = gameWrapper->GetGUIManager();

    // This syntax requires c++17
    // Pick any name you want for the font here
    auto [res, newFont] = gui.LoadFont("OBSCounter", "OBSCounter.ttf", 40);

    if (res == 2 && newFont) {
        font = newFont;
    }
}