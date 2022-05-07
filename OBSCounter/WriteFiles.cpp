#include "pch.h"
#include "OBSCounter.h"

// writes a stat to its files
// also writes its game and average stats
void OBSCounter::write(int statIndex) {
    if (!writeFiles) { return; }
    // writes the total stat
    writeFile(fileLocation / (statStringsStandard[statIndex] + ".txt"), statArray[statIndex]);

    // doesn't write averageGames, 
    //  as that would just be games/games and always 1
    if (statIndex != games) {
        // writes average of stat per game
        writeFileAverage(fileLocation / (statStringsAverage[statIndex] + ".txt"), average(statIndex));
    }

    // writes the game version of stat
    // only writes if stat has a game version
    if (statIndex > STATSWITHOUTGAME) {
        writeSpecific(statIndex, STAT_GAME);
    }
}

// writes a single stat
void OBSCounter::writeSpecific(int statIndex, int statType) {
    if (!writeFiles) { return; }
    std::string statLocation = "ERR";
    int statValue = 0;

    switch (statType) {
    case STAT_DEFAULT:
        statLocation = statStringsStandard[statIndex];
        statValue = statArray[statIndex];
        break;
    case STAT_GAME:
        statLocation = statStringsGame[statIndex];
        statValue = statArrayGame[statIndex];
        break;
    case STAT_TEAM:
        statLocation = "Team/" + cvarBases[STAT_TEAM][statIndex];
        statValue = statArrayTeam[statIndex];
        break;
    case STAT_TEAM_OPPONENT:
        statLocation = "Team/" + cvarBases[STAT_TEAM_OPPONENT][statIndex];
        statValue = statArrayOpponent[statIndex];
        break;
    default:
        break;
    }

    writeFile(fileLocation / (statLocation + ".txt"), statValue);
}

void OBSCounter::writeTeams() {
    if (!writeFiles) { return; }
    for (int i = 0; i < ENDNORMALSTATS; i++) {
        std::string statLocationTeam = "Team/" + cvarBases[STAT_TEAM][i];
        int statValueTeam = statArrayTeam[i];

        writeFile(fileLocation / (statLocationTeam + ".txt"), statValueTeam);

        std::string statLocationOpponent = "Team/" + cvarBases[STAT_TEAM_OPPONENT][i];
        int statValueOpponent = statArrayOpponent[i];

        writeFile(fileLocation / (statLocationOpponent + ".txt"), statValueOpponent);
    }
}

// writes a time stat with special formatting minutes:seconds
void OBSCounter::writeTimeStat(int statIndex) {
    if (!writeFiles) { return; }
    int totalSeconds = statArray[statIndex];
    // writes the total stat
    std::ofstream totalFile(fileLocation / (statStringsStandard[statIndex] + ".txt"));
    totalFile << totalSeconds / 60;
    totalFile << ":";
    int remSeconds = totalSeconds % 60;
    if (remSeconds < 10) {
        totalFile << "0";
    }
    totalFile << remSeconds;
    totalFile.close();

    // writes average of stat per game
    std::ofstream averageFile(fileLocation / (statStringsAverage[statIndex] + ".txt"));

    // average time is total / games
    int avgSeconds;
    if (statArray[games] == 0) {
        avgSeconds = 0;
    }
    else {
        avgSeconds = totalSeconds / statArray[games];
    }
    averages[statIndex] = avgSeconds;
    averageFile << avgSeconds / 60;
    averageFile << ":";
    remSeconds = avgSeconds % 60;
    if (remSeconds < 10) {
        averageFile << "0";
    }
    averageFile << remSeconds;
    averageFile.close();

    // writes the game version of stat
    // only writes if stat has a game version
    if (statIndex > STATSWITHOUTGAME) {
        writeGameTimeStat(statIndex);
    }
}

// writes only the game time stat
void OBSCounter::writeGameTimeStat(int statIndex) {
    if (!writeFiles) { return; }
    int totalSeconds = statArrayGame[statIndex];
    // writes the stat
    std::ofstream file(fileLocation / (statStringsGame[statIndex] + ".txt"));
    file << totalSeconds / 60;
    file << ":";
    int remSeconds = totalSeconds % 60;
    if (remSeconds < 10) {
        file << "0";
    }
    file << remSeconds;
    file.close();
}

// calls all write functions at once
void OBSCounter::writeAll() {
    if (!writeFiles) { return; }
    for (int i = 0; i <= ENDNORMALSTATS; i++) {
        write(i);
    }

    for (int i = ENDNORMALSTATS + 1; i < NUMSTATS; i++) {
        writeTimeStat(i);
    }

    writeShootingPercentage();
    writeKillPercentage();
    writeMissedExterms();
    writeWinPercentage();
    writeCareerStats();
    writeTeams();
}

// special cases for extra complicated stats
// calculates shooting percentage (shots/goals)
void OBSCounter::writeShootingPercentage() {
    if (!writeFiles) { return; }
    // calculates current game shooting %
    // divides and checks for NaN
    int gameShooting = getPercentage(statArrayGame[goals], statArrayGame[shots]);
    std::stringstream gameShootingStream;
    gameShootingStream << gameShooting << "%";
    std::string gameShootingStr = gameShootingStream.str();
    std::ofstream gameFile(fileLocation / "gameShootingPercentage.txt");
    gameFile << gameShootingStr;
    gameFile.close();
    statArrayOther[gameShootingPercentage] = gameShootingStr;

    // writes total session shooting
    int totalShooting = getPercentage(statArray[goals], statArray[shots]);
    std::stringstream totalShootingStream;
    totalShootingStream << totalShooting << "%";
    std::string totalShootingStr = totalShootingStream.str();
    std::ofstream file(fileLocation / "shootingPercentage.txt");
    file << totalShootingStr;
    file.close();
    statArrayOther[shootingPercentage] = totalShootingStr;
}

// writes K/D ratio
void OBSCounter::writeKillPercentage() {
    if (!writeFiles) { return; }
    float gameKD = divide(statArrayGame[demos], statArrayGame[deaths]);
    std::stringstream gameStream;
    gameStream << std::fixed << std::setprecision(decimalPlaces) << gameKD;
    std::string gameStr = gameStream.str();
    std::ofstream gameFile(fileLocation / "gameKDRatio.txt");
    gameFile << gameStr;
    gameFile.close();
    statArrayOther[gameKDRatio] = gameStr;

    float totalKDRatio = divide(statArray[demos], statArray[deaths]);
    std::stringstream totalStream;
    totalStream << std::fixed << std::setprecision(decimalPlaces) << totalKDRatio;
    std::string totalStr = totalStream.str();
    std::ofstream file(fileLocation / "KDRatio.txt");
    file << totalStr;
    file.close();
    statArrayOther[kDRatio] = totalStr;
}

// writes missed exterm % for the session
// total exterms / possible exterms (demos / 7) 
void OBSCounter::writeMissedExterms() {
    if (!writeFiles) { return; }
    // calculates possible exterms (demos / 7) 
    int possibleExterms = statArray[demos] / 7;
    std::stringstream totalStream;
    totalStream << possibleExterms;
    std::string totalStr = totalStream.str();
    std::ofstream totalFile(fileLocation / "possibleExterminations.txt");
    totalFile << totalStr;
    totalFile.close();
    statArrayOther[possibleExterminations] = totalStr;

    int missedExtermPercent = getPercentage(statArray[exterms], possibleExterms);
    std::stringstream percentStream;
    percentStream << missedExtermPercent << "%";
    std::string percentStr = percentStream.str();
    std::ofstream file(fileLocation / "missedExterminationPercent.txt");
    file << percentStr;
    file.close();
    statArrayOther[missedExterminationPercent] = percentStr;
}

void OBSCounter::writeWinPercentage() {
    if (!writeFiles) { return; }
    int winPercent = getPercentage(statArray[wins], statArray[wins] + statArray[losses]);
    std::stringstream percentStream;
    percentStream << winPercent << "%";
    std::string percentStr = percentStream.str();
    std::ofstream file(fileLocation / "winPercent.txt");
    file << percentStr;
    file.close();
    statArrayOther[winPercentage] = percentStr;
}

void OBSCounter::writeFile(std::filesystem::path path, int value) {
    if (!writeFiles) { return; }
    std::ofstream file(path);
    file << value;
    file.close();
}

void OBSCounter::writeFileAverage(std::filesystem::path path, float value) {
    if (!writeFiles) { return; }
    std::ofstream file(path);
    file << std::fixed << std::setprecision(decimalPlaces);
    file << value;
    file.close();
}