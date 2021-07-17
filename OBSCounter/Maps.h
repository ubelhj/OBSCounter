#include "pch.h"

// enum for all stat indexes 
// easier to refer back to stat names
// total stats are from 0 to 30, game stats from 31 to end
enum stats {
    wins,
    losses,
    mvps,
    games,
    // end of stats without game counterpart
    statsWithoutGame = games,
    // stat + 27 = gameStat
    goals,
    demos,
    deaths,
    exterms,
    aerialGoals,
    backwardsGoals,
    bicycleGoals,
    longGoals,
    turtleGoals,
    poolShots,
    overtimeGoals,
    hatTricks,
    assists,
    playmakers,
    saves,
    epicSaves,
    saviors,
    shots,
    centers,
    clears,
    firstTouchs,
    damages,
    ultraDamages,
    lowFives,
    highFives,
    swishs,
    bicycleHits,
    points,
    // last stat with normal output type
    endNormalStats = points,
    timePlayed,
    offenseTime,
    defenseTime,
    // game stats are from 31 -> end
    // goals + 27 = gameGoals
    gameGoals,
    // start of game stats and/or 1 after the end of non-game stats
    startGameStats = gameGoals,
    gameDemos,
    gameDeaths,
    gameExterms,
    gameAerialGoals,
    gameBackwardsGoals,
    gameBicycleGoals,
    gameLongGoals,
    gameTurtleGoals,
    gamePoolShots,
    gameOvertimeGoals,
    gameHatTricks,
    gameAssists,
    gamePlaymakers,
    gameSaves,
    gameEpicSaves,
    gameSaviors,
    gameShots,
    gameCenters,
    gameClears,
    gameFirstTouchs,
    gameDamages,
    gameUltraDamages,
    gameLowFives,
    gameHighFives,
    gameSwishs,
    gameBicycleHits,
    gamePoints,
    endNormalGameStats = gamePoints,
    gameTime,
    gameOffenseTime,
    gameDefenseTime,
    // total number of stats in the stat array
    numStats
};

// jump from a stat to its most recent game stat by adding this number (27)
// goals + 27 = gameGoals
constexpr int totalToGame = startGameStats - 1 - statsWithoutGame;

std::string indexStringMap[] = {
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

std::string indexStringMapRender[numStats] = {
    "Wins: ",
    "Losses: ",
    "Mvps: ",
    "Games: ",
    "Goals: ",
    "Demolitions: ",
    "Deaths: ",
    "Exterminations: ",
    "AerialGoals: ",
    "BackwardsGoals: ",
    "BicycleGoals: ",
    "LongGoals: ",
    "TurtleGoals: ",
    "PoolShots: ",
    "OvertimeGoals: ",
    "HatTricks: ",
    "Assists: ",
    "Playmakers: ",
    "Saves: ",
    "EpicSaves: ",
    "Saviors: ",
    "Shots: ",
    "Centers: ",
    "Clears: ",
    "FirstTouchs: ",
    "Damages: ",
    "UltraDamages: ",
    "LowFives: ",
    "HighFives: ",
    "Swishs: ",
    "BicycleHits: ",
    "Points: ",
    "TimePlayed: ",
    "OffenseTime: ",
    "DefenseTime: ",
    "GameGoals: ",
    "GameDemolitions: ",
    "GameDeaths: ",
    "GameExterminations: ",
    "GameAerialGoals: ",
    "GameBackwardsGoals: ",
    "GameBicycleGoals: ",
    "GameLongGoals: ",
    "GameTurtleGoals: ",
    "GamePoolShots: ",
    "GameOvertimeGoals: ",
    "GameHatTricks: ",
    "GameAssists: ",
    "GamePlaymakers: ",
    "GameSaves: ",
    "GameEpicSaves: ",
    "GameSaviors: ",
    "GameShots: ",
    "GameCenters: ",
    "GameClears: ",
    "GameFirstTouchs: ",
    "GameDamages: ",
    "GameUltraDamages: ",
    "GameLowFives: ",
    "GameHighFives: ",
    "GameSwishs: ",
    "GameBicycleHits: ",
    "GamePoints: ",
    "GameTimePlayed: ",
    "GameOffenseTime: ",
    "GameDefenseTime: "
};

std::string averageStrings[startGameStats] = {
    "averageWins",
    "averageLosses",
    "averageMvps",
    "averageGames",
    "averageGoals",
    "averageDemolitions",
    "averageDeaths",
    "averageExterminations",
    "averageAerialGoals",
    "averageBackwardsGoals",
    "averageBicycleGoals",
    "averageLongGoals",
    "averageTurtleGoals",
    "averagePoolShots",
    "averageOvertimeGoals",
    "averageHatTricks",
    "averageAssists",
    "averagePlaymakers",
    "averageSaves",
    "averageEpicSaves",
    "averageSaviors",
    "averageShots",
    "averageCenters",
    "averageClears",
    "averageFirstTouchs",
    "averageDamages",
    "averageUltraDamages",
    "averageLowFives",
    "averageHighFives",
    "averageSwishs",
    "averageBicycleHits",
    "averagePoints",
    "averageTimePlayed",
    "averageOffenseTime",
    "averageDefenseTime"
};

std::string averageStringsRender[] = {
    "AverageWins: ",
    "AverageLosses: ",
    "AverageMvps: ",
    "AverageGames: ",
    "AverageGoals: ",
    "AverageDemolitions: ",
    "AverageDeaths: ",
    "AverageExterminations: ",
    "AverageAerialGoals: ",
    "AverageBackwardsGoals: ",
    "AverageBicycleGoals: ",
    "AverageLongGoals: ",
    "AverageTurtleGoals: ",
    "AveragePoolShots: ",
    "AverageOvertimeGoals: ",
    "AverageHatTricks: ",
    "AverageAssists: ",
    "AveragePlaymakers: ",
    "AverageSaves: ",
    "AverageEpicSaves: ",
    "AverageSaviors: ",
    "AverageShots: ",
    "AverageCenters: ",
    "AverageClears: ",
    "AverageFirstTouchs: ",
    "AverageDamages: ",
    "AverageUltraDamages: ",
    "AverageLowFives: ",
    "AverageHighFives: ",
    "AverageSwishs: ",
    "AverageBicycleHits: ",
    "AveragePoints: ",
    "AverageTimePlayed: ",
    "AverageOffenseTime: ",
    "AverageDefenseTime: "
};

const std::map<std::string, int> eventDictionary = {
    { "Demolish", demos},
    { "Demolition", exterms},
    { "Goal", goals},
    { "Win", wins},
    { "MVP", mvps},
    { "AerialGoal", aerialGoals},
    { "BackwardsGoal", backwardsGoals},
    { "BicycleGoal", bicycleGoals},
    { "LongGoal", longGoals},
    { "TurtleGoal", turtleGoals},
    { "PoolShot", poolShots},
    { "OvertimeGoal", overtimeGoals},
    { "HatTrick", hatTricks},
    { "Assist", assists},
    { "Playmaker", playmakers},
    { "Save", saves},
    { "EpicSave", epicSaves},
    { "Savior", saviors},
    { "Shot", shots},
    { "Center", centers},
    { "Clear", clears},
    { "FirstTouch", firstTouchs},
    { "BreakoutDamage", damages},
    { "BreakoutDamageLarge", ultraDamages},
    { "LowFive", lowFives},
    { "HighFive", highFives},
    { "HoopsSwishGoal", swishs},
    { "BicycleHit", bicycleHits}
};