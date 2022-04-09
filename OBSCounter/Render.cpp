#include "pch.h"
#include "OBSCounter.h"

// Renders in game overlay
void OBSCounter::render(CanvasWrapper canvas) {
    if (!enabledOverlay) {
        return;
    }

    renderAllStrings();

    screenSize = canvas.GetSize();

    float fontSize = scale;
    int xValue = int((float)screenSize.X * xLocation);
    int yValue = int((float)screenSize.Y * yLocation);

    Vector2F maxStringSize = { -1.0, -1.0 };
    for (int i = 0; i < overlayLines; i++) {
        Vector2F newStringSize = canvas.GetStringSize(overlayStrings[i], fontSize, fontSize);

        if (newStringSize.X > maxStringSize.X) {
            maxStringSize.X = newStringSize.X;
        }

        if (newStringSize.Y > maxStringSize.Y) {
            maxStringSize.Y = newStringSize.Y;
        }
    }

    if (enabledOverlayBackground) {
        canvas.SetColor(overlayBackgroundColor);
        canvas.SetPosition(Vector2 { xValue, yValue });
        canvas.FillBox(Vector2({ int(maxStringSize.X), int(maxStringSize.Y * overlayLines) }));
    }

    // sets to user-chosen color
    canvas.SetColor(overlayColor);

    for (int i = 0; i < overlayLines; i++) {
        // locates based on screen and font size
        canvas.SetPosition(Vector2({ xValue, int((maxStringSize.Y * i) + yValue) }));
        canvas.DrawString(overlayStrings[i], fontSize, fontSize, false);
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
        if (overlayStates[i] > STAT_CAREER_START) {
            overlayStrings[i] = statToRenderString(overlayStatsCareer[i], overlayStates[i]);
        } else {
            overlayStrings[i] = statToRenderString(overlayStats[i], overlayStates[i]);
        }
    }
}