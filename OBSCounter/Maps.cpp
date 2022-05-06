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

void Maps::generateStatStrings() {
    for (int i = 0; i < NUMSTATS; i++) {
        // camelCase internal, ex: "gameDemolitions"
        std::string capitalizedString = statStringsStandard[i];
        capitalizedString[0] = std::toupper(capitalizedString[0]);
        statStringsGame[i] = "game" + capitalizedString;
        statStringsAverage[i] = "average" + capitalizedString;

        // For rendering to overlay
        // ex: "Demolitions: "
        renderStringsStandard[i] = std::string(statCharStandard[i]) + ": ";
        // ex: "Game Demolitions: "
        renderStringsGame[i] = "Game " + renderStringsStandard[i];
        renderStringsAverage[i] = "Average " + renderStringsStandard[i];
    }

    for (int i = 0; i < NUMOTHERSTATS; i++) {
        renderStringsOther[i] = std::string(statCharOther[i]) + ": ";
    }

    for (int i = 0; i < NUMCAREERSTATS; i++) {
        std::string renderBase = std::string(statCharsCareer[i]) + ": ";
        renderStringsCareerTotal[i] = "Career Total " + renderBase;
        renderStringsCareerPrivate[i] = "Career Private " + renderBase;
        renderStringsCareerCasual[i] = "Career Casual " + renderBase;
        renderStringsCareerRanked[i] = "Career Ranked " + renderBase;
        renderStringsCareerAverage[i] = "Career Average " + renderBase;
    }

    for (int i = 0; i < ENDNORMALSTATS; i++) {
        renderStringsTeam[i] = "Team " + renderStringsStandard[i];
        renderStringsOpponent[i] = "Opponent " + renderStringsStandard[i];
    }
}