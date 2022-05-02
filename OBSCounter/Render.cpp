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

    ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColorVec);
    ImGui::PushStyleColor(ImGuiCol_Text, textColorVec);
    ImGui::PushStyleColor(ImGuiCol_Border, textColorVec);

    if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, WindowFlags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::PopStyleColor(3);
        ImGui::End();
        return;
    }

    renderAllStrings();

    ImGui::SetWindowFontScale(scale);

    for (int i = 0; i < overlayLines; i++) {
        // locates based on screen and font size
        ImGui::Text(overlayStrings[i].c_str());
    }

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
        if (index >= numStats) {
            return "INVALID STATE";
        }
        // writes time stats
        if (index > endNormalStats) {
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

        return indexStringMapRender[index] + strStream.str();
    case STAT_AVERAGE:
        if (index >= numStats) {
            return "INVALID STATE";
        }
        if (index > endNormalStats) {
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
        return averageStringsRender[index] + strStream.str();
    case STAT_GAME:
        if (index >= numStats) {
            return "INVALID STATE";
        }
        // writes time stats
        if (index > endNormalStats) {
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

        return indexStringMapRenderGame[index] + strStream.str();
    case STAT_OTHER:
        if (index >= numOtherStats) {
            return indexStringMapRenderOther[0] + statArrayOther[0];
        } else {
            return indexStringMapRenderOther[index] + statArrayOther[index];
        }
    case STAT_CAREER_TOTAL:
        if (index >= NUMCAREERSTATS) {
            return "INVALID STATE";
        }
        strStream << careerStatTotal[index];

        return indexStringMapRenderCareerTotal[index] + strStream.str();
    case STAT_CAREER_PRIVATE:
        if (index >= NUMCAREERSTATS) {
            return "INVALID STATE";
        }
        strStream << careerStatPrivate[index];

        return indexStringMapRenderCareerPrivate[index] + strStream.str();
    case STAT_CAREER_RANKED:
        if (index >= NUMCAREERSTATS) {
            return "INVALID STATE";
        }
        strStream << careerStatRanked[index];

        return indexStringMapRenderCareerRanked[index] + strStream.str();
    case STAT_CAREER_CASUAL:
        if (index >= NUMCAREERSTATS) {
            return "INVALID STATE";
        }
        strStream << careerStatCasual[index];

        return indexStringMapRenderCareerCasual[index] + strStream.str();
    case STAT_CAREER_AVERAGE:
        if (index >= NUMCAREERSTATS) {
            return "INVALID STATE";
        }
        strStream << std::fixed << std::setprecision(decimalPlaces);
        strStream << careerStatAverage[index];

        return indexStringMapRenderCareerAverage[index] + strStream.str();
    case STAT_TEAM:
        if (index >= numStats) {
            return "INVALID STATE";
        }

        // writes time stats
        if (index >= endNormalStats) {
            // time stats don't exist for teams
        } else {
            strStream << statArrayTeam[index];
        }

        return indexStringMapRenderTeam[index] + strStream.str();
    case STAT_TEAM_OPPONENT:
        if (index >= numStats) {
            return "INVALID STATE";
        }

        // writes time stats
        if (index >= endNormalStats) {
            // time stats don't exist for teams
        } else {
            strStream << statArrayOpponent[index];
        }

        return indexStringMapRenderOpponent[index] + strStream.str();
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
    return true;
}

// Called when window is opened
void OBSCounter::OnOpen()
{
    isWindowOpen_ = true;
}

// Called when window is closed
void OBSCounter::OnClose()
{
    isWindowOpen_ = false;
}