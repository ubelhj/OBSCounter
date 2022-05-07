#include "pch.h"
#include "Maps.h"

std::string statStringsGame[NUMSTATS] = { "ERR" };
std::string statStringsAverage[NUMSTATS] = { "ERR" };

// Render Strings
std::string renderStringsStandard[NUMSTATS] = { "ERR" };
std::string renderStringsGame[NUMSTATS] = { "ERR" };
std::string renderStringsAverage[NUMSTATS] = { "ERR" };

std::string renderStringsOther[NUMOTHERSTATS] = { "ERR" };

std::string renderStringsCareerTotal[NUMCAREERSTATS] = { "ERR" };
std::string renderStringsCareerPrivate[NUMCAREERSTATS] = { "ERR" };
std::string renderStringsCareerCasual[NUMCAREERSTATS] = { "ERR" };
std::string renderStringsCareerRanked[NUMCAREERSTATS] = { "ERR" };
std::string renderStringsCareerAverage[NUMCAREERSTATS] = { "ERR" };

std::string renderStringsTeam[ENDNORMALSTATS] = { "ERR" };
std::string renderStringsOpponent[ENDNORMALSTATS] = { "ERR" };

// Label strings for ImGui
std::string labelStringsStandard[NUMSTATS] = { "ERR" };
std::string labelStringsGame[NUMSTATS] = { "ERR" };
std::string labelStringsAverage[NUMSTATS] = { "ERR" };

std::string labelStringsOther[NUMOTHERSTATS] = { "ERR" };

std::string labelStringsCareerTotal[NUMCAREERSTATS] = { "ERR" };
std::string labelStringsCareerPrivate[NUMCAREERSTATS] = { "ERR" };
std::string labelStringsCareerCasual[NUMCAREERSTATS] = { "ERR" };
std::string labelStringsCareerRanked[NUMCAREERSTATS] = { "ERR" };
std::string labelStringsCareerAverage[NUMCAREERSTATS] = { "ERR" };

std::string labelStringsTeam[ENDNORMALSTATS] = { "ERR" };
std::string labelStringsOpponent[ENDNORMALSTATS] = { "ERR" };

std::string statStringsCareerTotal[NUMCAREERSTATS] = { "ERR" };
std::string statStringsCareerPrivate[NUMCAREERSTATS] = { "ERR" };
std::string statStringsCareerCasual[NUMCAREERSTATS] = { "ERR" };
std::string statStringsCareerRanked[NUMCAREERSTATS] = { "ERR" };
std::string statStringsCareerAverage[NUMCAREERSTATS] = { "ERR" };
std::string statStringsTeam[ENDNORMALSTATS] = { "ERR" };
std::string statStringsOpponent[ENDNORMALSTATS] = { "ERR" };

std::vector<std::string*> labelStrings  = {
   labelStringsStandard,
   labelStringsAverage,
   labelStringsGame,
   labelStringsOther,
   labelStringsCareerTotal,
   labelStringsCareerPrivate,
   labelStringsCareerCasual,
   labelStringsCareerRanked,
   labelStringsCareerAverage,
   labelStringsTeam,
   labelStringsOpponent
};

std::vector<std::string*> renderStrings = {
   renderStringsStandard,
   renderStringsAverage,
   renderStringsGame,
   renderStringsOther,
   renderStringsCareerTotal,
   renderStringsCareerPrivate,
   renderStringsCareerCasual,
   renderStringsCareerRanked,
   renderStringsCareerAverage,
   renderStringsTeam,
   renderStringsOpponent
};

std::vector<std::string*> cvarBases = {
   statStringsStandard,
   statStringsAverage,
   statStringsGame,
   statStringsOther,
   statStringsCareerTotal,
   statStringsCareerPrivate,
   statStringsCareerCasual,
   statStringsCareerRanked,
   statStringsCareerAverage,
   statStringsTeam,
   statStringsOpponent
};

void Maps::generateStatStrings() {
    for (int i = 0; i < NUMSTATS; i++) {
        // camelCase internal, ex: "gameDemolitions"
        std::string capitalizedString = statStringsStandard[i];
        capitalizedString[0] = std::toupper(capitalizedString[0]);
        statStringsGame[i] = "game" + capitalizedString;
        statStringsAverage[i] = "average" + capitalizedString;

        // For use in ImGui labels
        std::string baseLabel = std::string(statCharStandard[i]);
        labelStrings[STAT_DEFAULT][i] = "Session " + baseLabel;

        // For rendering to overlay
        // ex: "Demolitions: "
        renderStrings[STAT_DEFAULT][i] = std::string(statCharStandard[i]) + ": ";
        // ex: "Game Demolitions: "
        renderStrings[STAT_GAME][i] = "Game " + renderStrings[STAT_DEFAULT][i];
        labelStrings[STAT_GAME][i] = "Game " + baseLabel;
        renderStrings[STAT_AVERAGE][i] = "Average " + renderStrings[STAT_DEFAULT][i];
        labelStrings[STAT_AVERAGE][i] = "Average " + baseLabel;
    }

    for (int i = 0; i < NUMOTHERSTATS; i++) {
        renderStrings[STAT_OTHER][i] = std::string(statCharOther[i]) + ": ";
        labelStrings[STAT_OTHER][i] = std::string(statCharOther[i]);
    }

    for (int i = 0; i < NUMCAREERSTATS; i++) {
        std::string renderBase = statStringsCareer[i] + ": ";
        renderStrings[STAT_CAREER_TOTAL][i] = "Career Total " + renderBase;
        renderStrings[STAT_CAREER_PRIVATE][i] = "Career Private " + renderBase;
        renderStrings[STAT_CAREER_CASUAL][i] = "Career Casual " + renderBase;
        renderStrings[STAT_CAREER_RANKED][i] = "Career Ranked " + renderBase;
        renderStrings[STAT_CAREER_AVERAGE][i] = "Career Average " + renderBase;

        std::string careerLabelBase = std::string(statCharsCareer[i]);
        labelStrings[STAT_CAREER_TOTAL][i] = "Career Total " + careerLabelBase;
        labelStrings[STAT_CAREER_PRIVATE][i] = "Career Private " + careerLabelBase;
        labelStrings[STAT_CAREER_CASUAL][i] = "Career Casual " + careerLabelBase;
        labelStrings[STAT_CAREER_RANKED][i] = "Career Ranked " + careerLabelBase;
        labelStrings[STAT_CAREER_AVERAGE][i] = "Career Average " + careerLabelBase;

        cvarBases[STAT_CAREER_TOTAL][i] = "total" + statStringsCareer[i];
        cvarBases[STAT_CAREER_PRIVATE][i] = "private" + statStringsCareer[i];
        cvarBases[STAT_CAREER_CASUAL][i] = "casual" + statStringsCareer[i];
        cvarBases[STAT_CAREER_RANKED][i] = "ranked" + statStringsCareer[i];
        cvarBases[STAT_CAREER_AVERAGE][i] = "average" + statStringsCareer[i];
    }

    for (int i = 0; i < ENDNORMALSTATS; i++) {
        renderStrings[STAT_TEAM][i] = "Team " + renderStrings[STAT_DEFAULT][i];
        renderStrings[STAT_TEAM_OPPONENT][i] = "Opponent " + renderStrings[STAT_DEFAULT][i];

        std::string baseLabel = std::string(statCharStandard[i]);
        labelStrings[STAT_TEAM][i] = "Team " + baseLabel;
        labelStrings[STAT_TEAM_OPPONENT][i] = "Opponent " + baseLabel;

        std::string cvarBase = statStringsStandard[i];
        cvarBase[0] = std::toupper(cvarBase[0]);

        cvarBases[STAT_TEAM][i] = "team" + cvarBase;
        cvarBases[STAT_TEAM_OPPONENT][i] = "opponent" + cvarBase;
    }
}