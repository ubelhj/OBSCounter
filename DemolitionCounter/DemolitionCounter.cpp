#include "pch.h"
#include "DemolitionCounter.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

BAKKESMOD_PLUGIN(DemolitionCounter, "Counts demolitions in online games", plugin_version, PLUGINTYPE_THREADED)

bool endedGame = true;
int decimalPlaces = 2;
bool enabledOverlay = false;
int overlayNum = 5;
int overlayStats[5];
bool overlayAverages[5];
float xLocation;
float yLocation;
int overlayColors[3];

// constexpr for all stat indexes 
// easier to refer back to stat names
// total stats are from 0 to 28, game stats from 29 to end
// these stats have no game counterpart
constexpr int wins = 0;
constexpr int mvps = 1;
constexpr int games = 2;
// stat + 26 = gameStat
constexpr int goals = 3;
constexpr int demos = 4;
constexpr int deaths = 5;
constexpr int exterms = 6;
constexpr int aerialGoals = 7;
constexpr int backwardsGoals = 8;
constexpr int bicycleGoals = 9;
constexpr int longGoals = 10;
constexpr int turtleGoals = 11;
constexpr int poolShots = 12;
constexpr int overtimeGoals = 13;
constexpr int hatTricks = 14;
constexpr int assists = 15;
constexpr int playmakers = 16;
constexpr int saves = 17;
constexpr int epicSaves = 18;
constexpr int saviors = 19;
constexpr int shots = 20;
constexpr int centers = 21;
constexpr int clears = 22;
constexpr int firstTouchs = 23;
constexpr int damages = 24;
constexpr int ultraDamages = 25;
constexpr int lowFives = 26;
constexpr int highFives = 27;
constexpr int swishs = 28;
// game stats are from 29 -> end
constexpr int gameGoals = 29;
constexpr int gameDemos = 30;
constexpr int gameDeaths = 31;
constexpr int gameExterms = 32;
constexpr int gameAerialGoals = 33;
constexpr int gameBackwardsGoals = 34;
constexpr int gameBicycleGoals = 35;
constexpr int gameLongGoals = 36;
constexpr int gameTurtleGoals = 37;
constexpr int gamePoolShots = 38;
constexpr int gameOvertimeGoals = 39;
constexpr int gameHatTricks = 40;
constexpr int gameAssists = 41;
constexpr int gamePlaymakers = 42;
constexpr int gameSaves = 43;
constexpr int gameEpicSaves = 44;
constexpr int gameSaviors = 45;
constexpr int gameShots = 46;
constexpr int gameCenters = 47;
constexpr int gameClears = 48;
constexpr int gameFirstTouchs = 49;
constexpr int gameDamages = 50;
constexpr int gameUltraDamages = 51;
constexpr int gameLowFives = 52;
constexpr int gameHighFives = 53;
constexpr int gameSwishs = 54;
constexpr int numStats = 55;

// jump from a stat to its most recent game stat
constexpr int totalToGame = 26;
// number of stats without game counterpart
constexpr int statsWithoutGame = 3;
// start of game stats
constexpr int startGameStats = gameGoals;

// maps the indexes to their respective strings
// index to string
std::string indexStringMap[] = {
    "wins",
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
    "gameSwishs"
};

// holds all stats
int statArray[numStats];

// holds strings for averages (doesn't include game stats)
std::string averageStrings[startGameStats] = {
    "averageWins",
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
    "averageSwishs"
};

// holds all averages
float averages[startGameStats];

void DemolitionCounter::onLoad()
{
    cvarManager->log("Plugin loaded!");
    auto decimalsVar = cvarManager->registerCvar("counter_decimals", "2", "set decimal places in averages (1 - 10)");
    decimalPlaces = decimalsVar.getIntValue();
    decimalsVar.addOnValueChanged([this](std::string, CVarWrapper cvar) { 
            int newValue = cvar.getIntValue();
            if (newValue >= 1 && newValue <= 10) {
                decimalPlaces = newValue;
                writeAll();
            }
        });

    // in-game overlay cvars
    // enables or disables overlay
    auto overlayEnableVar = cvarManager->registerCvar("counter_enable_ingame", "0", "enables in game overlay");
    enabledOverlay = overlayEnableVar.getBoolValue();
    overlayEnableVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
            enabledOverlay = cvar.getBoolValue();
        });

    // sets number of stats in in game overlay
    // 1-5 allowed
    auto overlayNumberVar = cvarManager->registerCvar("counter_ingame_numStats", "5", "number of stats in in game overlay", true, true, 1, true, 5);
    overlayNum = overlayNumberVar.getIntValue();
    overlayNumberVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayNum = cvar.getIntValue();
        });

    // first stat in overlay (defaults to demos/4)
    auto overlayOneVar = cvarManager->registerCvar("counter_ingame_stat_one", "4", "First stat in overlay", true, true, 0, true, numStats - 1);
    overlayStats[0] = overlayOneVar.getIntValue();
    overlayOneVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayStats[0] = cvar.getIntValue();
        });

    // sets first stat in overlay to average or not
    auto overlayOneAvgVar = cvarManager->registerCvar("counter_ingame_stat_one_average", "0", "Toggles average of first stat in overlay", true, true, 0, true, 1);
    overlayAverages[0] = overlayOneAvgVar.getBoolValue();
    overlayOneAvgVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayAverages[0] = cvar.getBoolValue();
        });

    // second stat in overlay (defaults to exterms/6)
    auto overlayTwoVar = cvarManager->registerCvar("counter_ingame_stat_two", "6", "Second stat in overlay", true, true, 0, true, numStats - 1);
    overlayStats[1] = overlayTwoVar.getIntValue();
    overlayTwoVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayStats[1] = cvar.getIntValue();
        });

    // sets second stat in overlay to average or not
    auto overlayTwoAvgVar = cvarManager->registerCvar("counter_ingame_stat_two_average", "0", "Toggles average of second stat in overlay", true, true, 0, true, 1);
    overlayAverages[1] = overlayTwoAvgVar.getBoolValue();
    overlayTwoAvgVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayAverages[1] = cvar.getBoolValue();
        });

    // third stat in overlay (defaults to gameDemos/30)
    auto overlayThreeVar = cvarManager->registerCvar("counter_ingame_stat_three", "30", "Third stat in overlay", true, true, 0, true, numStats - 1);
    overlayStats[2] = overlayThreeVar.getIntValue();
    overlayThreeVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayStats[2] = cvar.getIntValue();
        });

    // sets third stat in overlay to average or not
    auto overlayThreeAvgVar = cvarManager->registerCvar("counter_ingame_stat_three_average", "0", "Toggles average of third stat in overlay", true, true, 0, true, 1);
    overlayAverages[2] = overlayThreeAvgVar.getBoolValue();
    overlayThreeAvgVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayAverages[2] = cvar.getBoolValue();
        });

    // fourth stat in overlay (defaults to games/2)
    auto overlayFourVar = cvarManager->registerCvar("counter_ingame_stat_four", "2", "Fourth stat in overlay", true, true, 0, true, numStats - 1);
    overlayStats[3] = overlayFourVar.getIntValue();
    overlayFourVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayStats[3] = cvar.getIntValue();
        });

    // sets fourth stat in overlay to average or not
    auto overlayFourAvgVar = cvarManager->registerCvar("counter_ingame_stat_four_average", "0", "Toggles average of fourth stat in overlay", true, true, 0, true, 1);
    overlayAverages[3] = overlayFourAvgVar.getBoolValue();
    overlayFourAvgVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayAverages[3] = cvar.getBoolValue();
        });

    // fifth stat in overlay (defaults to deaths/5)
    auto overlayFiveVar = cvarManager->registerCvar("counter_ingame_stat_five", "5", "Fifth stat in overlay", true, true, 0, true, numStats - 1);
    overlayStats[4] = overlayFiveVar.getIntValue();
    overlayFiveVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayStats[4] = cvar.getIntValue();
        });

    // sets fifth stat in overlay to average or not
    auto overlayFiveAvgVar = cvarManager->registerCvar("counter_ingame_stat_five_average", "0", "Toggles average of fifth stat in overlay", true, true, 0, true, 1);
    overlayAverages[4] = overlayFiveAvgVar.getBoolValue();
    overlayFiveAvgVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayAverages[4] = cvar.getBoolValue();
        });

    // sets cvar to move counter's X location
    auto xLocVar = cvarManager->registerCvar("counter_ingame_x_location", "0.86", "set location of ingame counter X in % of screen", true, true, 0.0, true, 1.0);
    xLocation = xLocVar.getFloatValue();
    xLocVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        xLocation = cvar.getFloatValue();
        });

    // sets cvar to move counter's Y location
    auto yLocVar = cvarManager->registerCvar("counter_ingame_y_location", "0.025", "set location of ingame counter Y in % of screen", true, true, 0.0, true, 1.0);
    yLocation = yLocVar.getFloatValue();
    yLocVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        yLocation = cvar.getFloatValue();
        });

    // overlay red value changer 
    auto overlayRedVar = cvarManager->registerCvar("counter_ingame_red", "255", "Red value in overlay", true, true, 0, true, 255);
    overlayColors[0] = overlayRedVar.getIntValue();
    overlayRedVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayColors[0] = cvar.getIntValue();
        });

    // overlay green value changer 
    auto overlayGreenVar = cvarManager->registerCvar("counter_ingame_green", "0", "green value in overlay", true, true, 0, true, 255);
    overlayColors[1] = overlayGreenVar.getIntValue();
    overlayGreenVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayColors[1] = cvar.getIntValue();
        });

    // overlay blue value changer 
    auto overlayBlueVar = cvarManager->registerCvar("counter_ingame_blue", "0", "blue value in overlay", true, true, 0, true, 255);
    overlayColors[2] = overlayBlueVar.getIntValue();
    overlayBlueVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayColors[2] = cvar.getIntValue();
        });

    cvarManager->registerNotifier("counter_list_stats", [this](std::vector<std::string> params) {
        listStats();
        }, "List all different stat types", PERMISSION_ALL);

    gameWrapper->RegisterDrawable(std::bind(&DemolitionCounter::render, this, std::placeholders::_1));

    // setters for totals
    setCvars();

    hookEvents();
    
    // no longer needed for anything
   // writeAll();
}

void DemolitionCounter::setCvars() {
    // special case to make sure games update properly
    auto startgames = cvarManager->registerCvar("counter_set_games", "0", "sets games value");
    startgames.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[games] = cvar.getIntValue() - 1; endedGame = true; startGame(); endedGame = true; });
    auto startDemos = cvarManager->registerCvar("counter_set_demos", "0", "sets demolition value");
    startDemos.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[demos] = cvar.getIntValue(); write(demos); });
    auto startDeaths = cvarManager->registerCvar("counter_set_deaths", "0", "sets deaths value");
    startDeaths.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[deaths] = cvar.getIntValue(); write(deaths); });
    auto startExterms = cvarManager->registerCvar("counter_set_exterms", "0", "sets extermination value");
    startExterms.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[exterms] = cvar.getIntValue(); write(exterms); });
    auto startwins = cvarManager->registerCvar("counter_set_wins", "0", "sets wins value");
    startwins.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[wins] = cvar.getIntValue(); write(wins); });
    auto startmvps = cvarManager->registerCvar("counter_set_mvps", "0", "sets mvps value");
    startmvps.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[mvps] = cvar.getIntValue(); write(mvps); });
    auto startgoals = cvarManager->registerCvar("counter_set_goals", "0", "sets goals value");
    startgoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[goals] = cvar.getIntValue(); write(goals); });
    auto startaerialGoals = cvarManager->registerCvar("counter_set_aerialGoals", "0", "sets aerialGoals value");
    startaerialGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[aerialGoals] = cvar.getIntValue(); write(aerialGoals); });
    auto startbackwardsGoals = cvarManager->registerCvar("counter_set_backwardsGoals", "0", "sets backwardsGoals value");
    startbackwardsGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[backwardsGoals] = cvar.getIntValue(); write(backwardsGoals); });
    auto startbicycleGoals = cvarManager->registerCvar("counter_set_bicycleGoals", "0", "sets bicycleGoals value");
    startbicycleGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[bicycleGoals] = cvar.getIntValue(); write(bicycleGoals); });
    auto startlongGoals = cvarManager->registerCvar("counter_set_longGoals", "0", "sets longGoals value");
    startlongGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[longGoals] = cvar.getIntValue(); write(longGoals); });
    auto startturtleGoals = cvarManager->registerCvar("counter_set_turtleGoals", "0", "sets turtleGoals value");
    startturtleGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[turtleGoals] = cvar.getIntValue(); write(turtleGoals); });
    auto startpoolShots = cvarManager->registerCvar("counter_set_poolShots", "0", "sets poolShots value");
    startpoolShots.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[poolShots] = cvar.getIntValue(); write(poolShots); });
    auto startovertimeGoals = cvarManager->registerCvar("counter_set_overtimeGoals", "0", "sets overtimeGoals value");
    startovertimeGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[overtimeGoals] = cvar.getIntValue(); write(overtimeGoals); });
    auto starthatTricks = cvarManager->registerCvar("counter_set_hatTricks", "0", "sets hatTricks value");
    starthatTricks.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[hatTricks] = cvar.getIntValue(); write(hatTricks); });
    auto startassists = cvarManager->registerCvar("counter_set_assists", "0", "sets assists value");
    startassists.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[assists] = cvar.getIntValue(); write(assists); });
    auto startplaymakers = cvarManager->registerCvar("counter_set_playmakers", "0", "sets playmakers value");
    startplaymakers.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[playmakers] = cvar.getIntValue(); write(playmakers); });
    auto startsaves = cvarManager->registerCvar("counter_set_saves", "0", "sets saves value");
    startsaves.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[saves] = cvar.getIntValue(); write(saves); });
    auto startepicSaves = cvarManager->registerCvar("counter_set_epicSaves", "0", "sets epicSaves value");
    startepicSaves.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[epicSaves] = cvar.getIntValue(); write(epicSaves); });
    auto startsaviors = cvarManager->registerCvar("counter_set_saviors", "0", "sets saviors value");
    startsaviors.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[saviors] = cvar.getIntValue(); write(saviors); });
    auto startshots = cvarManager->registerCvar("counter_set_shots", "0", "sets shots value");
    startshots.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[shots] = cvar.getIntValue(); write(shots); });
    auto startcenters = cvarManager->registerCvar("counter_set_centers", "0", "sets centers value");
    startcenters.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[centers] = cvar.getIntValue(); write(centers); });
    auto startclears = cvarManager->registerCvar("counter_set_clears", "0", "sets clears value");
    startclears.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[clears] = cvar.getIntValue(); write(clears); });
    auto startfirstTouchs = cvarManager->registerCvar("counter_set_firstTouchs", "0", "sets firstTouchs value");
    startfirstTouchs.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[firstTouchs] = cvar.getIntValue(); write(firstTouchs); });
    auto startdamages = cvarManager->registerCvar("counter_set_damages", "0", "sets damages value");
    startdamages.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[damages] = cvar.getIntValue(); write(damages); });
    auto startultraDamages = cvarManager->registerCvar("counter_set_ultraDamages", "0", "sets ultraDamages value");
    startultraDamages.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[ultraDamages] = cvar.getIntValue(); write(ultraDamages); });
    auto startlowFives = cvarManager->registerCvar("counter_set_lowFives", "0", "sets lowFives value");
    startlowFives.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[lowFives] = cvar.getIntValue(); write(lowFives); });
    auto starthighFives = cvarManager->registerCvar("counter_set_highFives", "0", "sets highFives value");
    starthighFives.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[highFives] = cvar.getIntValue(); write(highFives); });
    auto startswishs = cvarManager->registerCvar("counter_set_swishs", "0", "sets swishs value");
    startswishs.addOnValueChanged([this](std::string, CVarWrapper cvar) { statArray[swishs] = cvar.getIntValue(); write(swishs); });
}

void DemolitionCounter::hookEvents() {
    gameWrapper->HookEventWithCaller<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage", 
        std::bind(&DemolitionCounter::statEvent, this, std::placeholders::_1, std::placeholders::_2));

    // works on a starting game 
    gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.WaitingForPlayers.BeginState", std::bind(&DemolitionCounter::startGame, this));
    // works on a joined game in progress
    gameWrapper->HookEventPost("Function Engine.PlayerInput.InitInputSystem", std::bind(&DemolitionCounter::startGame, this));
    // works on ended game
    gameWrapper->HookEventPost("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&DemolitionCounter::endGame, this));
}

struct TheArgStruct
{
    uintptr_t Receiver;
    uintptr_t Victim;
    uintptr_t StatEvent;
};

// called whenever a stat appears in the top right and decides whether to update a file
void DemolitionCounter::statEvent(ServerWrapper caller, void* args) {
    auto tArgs = (TheArgStruct*)args;
    //cvarManager->log("stat event!");

    auto victim = PriWrapper(tArgs->Victim);
    auto receiver = PriWrapper(tArgs->Receiver);
    auto statEvent = StatEventWrapper(tArgs->StatEvent);
    auto label = statEvent.GetLabel();
    //cvarManager->log(label.ToString());

    // special case for demolitions to check for the player's death
    if (label.ToString().compare("Demolition") == 0) {
        if (DemolitionCounter::isPrimaryPlayer(receiver)) {
            statArray[demos]++;
            statArray[gameDemos]++;
            write(demos);
            return;
        }
        else if (DemolitionCounter::isPrimaryPlayer(victim)) {
            statArray[deaths]++;
            statArray[gameDeaths]++;
		    write(deaths);
            return;
        }
        return;
    }

    if (!DemolitionCounter::isPrimaryPlayer(receiver)) {
        return;
    }

    // Checks for each possible event name
    // no switches in c++ for strings
   /* if (label.ToString().compare("Demolition") == 0) {
        statArray[demolitions]++;
		write(demolitions);
        return; 
    }*/
    if (label.ToString().compare("Extermination") == 0) {
        statArray[exterms]++;
        statArray[gameExterms]++;
		write(exterms);
        return;
    }
    if (label.ToString().compare("Win") == 0) {
        statArray[wins]++;
		write(wins);
        return;
    }
    if (label.ToString().compare("Goal") == 0) {
        statArray[goals]++;
        statArray[gameGoals]++;
		write(goals);
        return;
    }
    if (label.ToString().compare("MVP") == 0) {
        statArray[mvps]++;
		write(mvps);
        return;
    }
    if (label.ToString().compare("Aerial Goal") == 0) {
        statArray[aerialGoals]++;
        statArray[gameAerialGoals]++;
		write(aerialGoals);
        return;
    }
    if (label.ToString().compare("Backwards Goal") == 0) {
        statArray[backwardsGoals]++;
        statArray[gameBackwardsGoals]++;
		write(backwardsGoals);
        return;
    }
    if (label.ToString().compare("Bicycle Goal") == 0) {
        statArray[bicycleGoals]++;
        statArray[gameBicycleGoals]++;
		write(bicycleGoals);
        return;
    }
    if (label.ToString().compare("Long Goal") == 0) {
        statArray[longGoals]++;
        statArray[gameLongGoals]++;
		write(longGoals);
        return;
    }
    if (label.ToString().compare("Turtle Goal") == 0) {
        statArray[turtleGoals]++;
        statArray[gameTurtleGoals]++;
		write(turtleGoals);
        return;
    }
    if (label.ToString().compare("Pool Shot") == 0) {
        statArray[poolShots]++;
        statArray[gamePoolShots]++;
		write(poolShots);
        return;
    }
    if (label.ToString().compare("Overtime Goal") == 0) {
        statArray[overtimeGoals]++;
        statArray[gameOvertimeGoals]++;
		write(overtimeGoals);
        return;
    }
    if (label.ToString().compare("Hat Trick") == 0) {
        statArray[hatTricks]++;
        statArray[gameHatTricks]++;
		write(hatTricks);
        return;
    }
    if (label.ToString().compare("Assist") == 0) {
        statArray[assists]++;
        statArray[gameAssists]++;
		write(assists);
        return;
    }
    if (label.ToString().compare("Playmaker") == 0) {
        statArray[playmakers]++;
        statArray[gamePlaymakers]++;
		write(playmakers);
        return;
    }
    if (label.ToString().compare("Save") == 0) {
        statArray[saves]++;
        statArray[gameSaves]++;
		write(saves);
        return;
    }
    if (label.ToString().compare("Epic Save") == 0) {
        statArray[epicSaves]++;
        statArray[gameEpicSaves]++;
		write(epicSaves);
        return;
    }
    if (label.ToString().compare("Savior") == 0) {
        statArray[saviors]++;
        statArray[gameSaviors]++;
		write(saviors);
        return;
    }
    if (label.ToString().compare("Shot on Goal") == 0) {
        statArray[shots]++;
        statArray[gameShots]++;
		write(shots);
        return;
    }
    if (label.ToString().compare("Center Ball") == 0) {
        statArray[centers]++;
        statArray[gameCenters]++;
		write(centers);
        return;
    }
    if (label.ToString().compare("Clear Ball") == 0) {
        statArray[clears]++;
        statArray[gameClears]++;
		write(clears);
        return;
    }
    if (label.ToString().compare("First Touch") == 0) {
        statArray[firstTouchs]++;
        statArray[gameFirstTouchs]++;
		write(firstTouchs);
        return;
    }
    if (label.ToString().compare("Damage") == 0) {
        statArray[damages]++;
        statArray[gameDamages]++;
		write(damages);
        return;
    }
    if (label.ToString().compare("Ultra Damage") == 0) {
        statArray[ultraDamages]++;
        statArray[gameUltraDamages]++;
		write(ultraDamages);
        return;
    }
    if (label.ToString().compare("Low Five") == 0) {
        statArray[lowFives]++;
        statArray[gameLowFives]++;
		write(lowFives);
        return;
    }
    if (label.ToString().compare("High Five") == 0) {
        statArray[highFives]++;
        statArray[gameHighFives]++;
		write(highFives);
        return;
    }
    if (label.ToString().compare("Swish Goal") == 0) {
        statArray[swishs]++;
        statArray[gameSwishs]++;
		write(swishs);
        return;
    }
}

// checks if the player who received a stat is the user's player
bool DemolitionCounter::isPrimaryPlayer(PriWrapper receiver) {
    if (!gameWrapper->IsInOnlineGame()) {
        cvarManager->log("not in online game");
        return false;
    }

    ServerWrapper sw = gameWrapper->GetOnlineGame();

    if (sw.IsNull()) {
        cvarManager->log("null server");
        return false;
    }

    auto primary = sw.GetLocalPrimaryPlayer();

    if (primary.IsNull()) {
        cvarManager->log("null primary player");
        return false;
    }

    auto primaryPri = primary.GetPRI();

    if (primaryPri.IsNull()) {
        cvarManager->log("null primary pri");
        return false;
    }

    if (receiver.IsNull()) {
        cvarManager->log("null receiver");
        return false;
    }

    auto receiverID = receiver.GetUniqueId();
    auto primaryID = primaryPri.GetUniqueId();

    return receiverID.ID == primaryID.ID;
}

// called whenever a new game begins, and resets all game stats to 0
void DemolitionCounter::startGame() {
    cvarManager->log("started game");
    // if the last game didnt end, doesn't start game
    if (!endedGame) {
        cvarManager->log("last game didn't end");
        return;
    }
    endedGame = false;

    statArray[games]++;
    statArray[gameExterms] = 0;
    statArray[gameDemos] = 0;
    statArray[gameDeaths] = 0;
    statArray[gameGoals] = 0;
    statArray[gameAerialGoals] = 0;
    statArray[gameBackwardsGoals] = 0;
    statArray[gameBicycleGoals] = 0;
    statArray[gameLongGoals] = 0;
    statArray[gameTurtleGoals] = 0;
    statArray[gamePoolShots] = 0;
    statArray[gameOvertimeGoals] = 0;
    statArray[gameHatTricks] = 0;
    statArray[gameAssists] = 0;
    statArray[gamePlaymakers] = 0;
    statArray[gameSaves] = 0;
    statArray[gameEpicSaves] = 0;
    statArray[gameSaviors] = 0;
    statArray[gameShots] = 0;
    statArray[gameCenters] = 0;
    statArray[gameClears] = 0;
    statArray[gameFirstTouchs] = 0;
    statArray[gameDamages] = 0;
    statArray[gameUltraDamages] = 0;
    statArray[gameLowFives] = 0;
    statArray[gameHighFives] = 0;
    statArray[gameSwishs] = 0;

    writeAllGameStats();
    write(games);
}

void DemolitionCounter::endGame() {
    endedGame = true;
}

// writing stats to files
void DemolitionCounter::writeShootingPercentage() {
    std::ofstream gameFile;
    float gameShooting;
    if (statArray[gameShots] == 0) {
        gameShooting = 0.0;
    }
    else {
        gameShooting = (float)statArray[gameGoals] / (float)statArray[gameShots];
    }
    gameFile.open("./OBSCounter/gameShootingPercentage.txt");
    gameFile << std::fixed << std::setprecision(decimalPlaces);
    gameFile << gameShooting;
    gameFile.close();

    float totalShooting;
    if (statArray[shots] == 0) {
        totalShooting = 0.0;
    }
    else {
        totalShooting = (float)statArray[goals] / (float)statArray[shots];
    }
    std::ofstream file;
    file.open("./OBSCounter/shootingPercentage.txt");
    file << std::fixed << std::setprecision(decimalPlaces);
    file << totalShooting;
    file.close();
}

// writes K/D ratio
void DemolitionCounter::writeKillPercentage() {
    std::ofstream gameFile;
    float gameKD;
    if (statArray[gameDeaths] == 0) {
        gameKD = 0.0;
    }
    else {
        gameKD = (float)statArray[gameDemos] / (float)statArray[gameDeaths];
    }
    gameFile.open("./OBSCounter/gameKDRatio.txt");
    gameFile << std::fixed << std::setprecision(decimalPlaces);
    gameFile << gameKD;
    gameFile.close();

    float totalKDRatio;
    if (statArray[deaths] == 0) {
        totalKDRatio = 0.0;
    }
    else {
        totalKDRatio = (float)statArray[demos] / (float)statArray[deaths];
    }
    std::ofstream file;
    file.open("./OBSCounter/KDRatio.txt");
    file << std::fixed << std::setprecision(decimalPlaces);
    file << totalKDRatio;
    file.close();
}

// writes missed exterm % for the session
void DemolitionCounter::writeMissedExterms() {
    // calculates possible exterms (demos / 7)
    std::ofstream totalFile;
    int possibleExterms = statArray[demos] / 7;
    totalFile.open("./OBSCounter/possibleExterminations.txt");
    totalFile << std::fixed << std::setprecision(decimalPlaces);
    totalFile << possibleExterms;
    totalFile.close();

    int missedExtermPercent;
    if (possibleExterms == 0) {
        missedExtermPercent = 0;
    }
    else {
        missedExtermPercent = ((float)statArray[exterms] / (float)possibleExterms) * 100;
    }
    std::ofstream file;
    file.open("./OBSCounter/missedExterminationPercent.txt");
    file << missedExtermPercent << "%";
    file.close();
}

// writes a stat to its files
void DemolitionCounter::write(int statIndex) {
    // writes the total stat
    std::ofstream totalFile;
    totalFile.open("./OBSCounter/" + indexStringMap[statIndex] + ".txt");
    totalFile << statArray[statIndex];
    totalFile.close();

    if (statIndex != games) {
        // writes average of stat per game
        std::ofstream averageFile;
        // sets up average file location
        std::string averageLocation = "./OBSCounter/";
        // makes the first letter uppercase for nice looking files
        averageLocation += averageStrings[statIndex] + ".txt";
        averageFile.open(averageLocation);

        float averageStat = average(statIndex);
        
        averageFile << std::fixed << std::setprecision(decimalPlaces);
        averageFile << averageStat;
        averageFile.close();
    }

    // writes the game stat
    // only writes if stat has a game version
    if (statIndex >= statsWithoutGame) {
        writeGameStat(statIndex + totalToGame);
    }

    // any extra stats needed with more computing
    // shooting % (shot or goal)
    // k/d (demo or death)
    // missed exterm % (demo or exterm)
    switch (statIndex) {
    case shots:
    case goals:
        writeShootingPercentage();
        break;
    case demos:
        writeKillPercentage();
    case exterms:
        writeMissedExterms();
        break;
    case deaths:
        writeKillPercentage();
    }
}

// writes a game stat, must be a game stat index
void DemolitionCounter::writeGameStat(int statIndex) {
    std::ofstream gameStatFile;
    gameStatFile.open("./OBSCounter/" + indexStringMap[statIndex] + ".txt");
    // jumps from stat to its game counterpart
    gameStatFile << statArray[statIndex];
    gameStatFile.close();
}

// writes all game stats at once
void DemolitionCounter::writeAllGameStats() {
    for (int i = startGameStats; i < numStats; i++) {
        writeGameStat(i);
    }
}

// calls all write functions at once
void DemolitionCounter::writeAll() {
    for (int i = 0; i < startGameStats; i++) {
        write(i);
    }
}

// Renders in game overlay
void DemolitionCounter::render(CanvasWrapper canvas) {
    if (!enabledOverlay) {
        return;
    }

    Vector2 screen = canvas.GetSize();

    // in 1080p font size is 2
    // y value of 2 size text is approx 20
    float fontSize = ((float)screen.X / (float)1920) * 2;

    // sets to red
    canvas.SetColor(overlayColors[0], overlayColors[1], overlayColors[2], 255);

    for (int i = 0; i < overlayNum; i++) {
        // locates based on screen and font size
        canvas.SetPosition(Vector2({ int((float)screen.X * xLocation), int((fontSize * (11 * i)) + ((float) screen.Y * yLocation)) }));
        // does averages if the user wants them
        if (overlayAverages[i] && overlayStats[i] < startGameStats) {
            // makes sure string has right number of decimal places
            std::ostringstream averageStream;
            averageStream << std::fixed << std::setprecision(decimalPlaces);
            averageStream << averages[overlayStats[i]];

            canvas.DrawString(averageStrings[overlayStats[i]] + ": " + averageStream.str(), fontSize, fontSize);
        }
        else {
            canvas.DrawString(indexStringMap[overlayStats[i]] + ": " + std::to_string(statArray[overlayStats[i]]), fontSize, fontSize);
        }
        
    }

    //cvarManager->log(std::to_string(fontSize));
}

// Lists all stats to console
void DemolitionCounter::listStats() {
    for (int i = 0; i < numStats; i++) {
        cvarManager->log(std::to_string(i) + ": " + indexStringMap[i]);
    }
}

// calculates average of stat and avoids NaN
float DemolitionCounter::average(int statIndex) {
    if (statArray[games] == 0) {
        averages[statIndex] = 0.0;
    }
    else {
        averages[statIndex] = (float)statArray[statIndex] / (float)statArray[games];
    }
    return averages[statIndex];
}

void DemolitionCounter::onUnload()
{
}