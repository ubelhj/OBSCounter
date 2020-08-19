#include "pch.h"
#include "DemolitionCounter.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

BAKKESMOD_PLUGIN(DemolitionCounter, "Counts demolitions in online games", 
    plugin_version, PLUGINTYPE_THREADED)

// whether the last game ended
// I know it may seem redundant, 
//  but it allows game tracking to work with casual games
// in casual you can join games in progress which makes the programming 
//  to deal with it a bit complicated. 
// leaving a game before it ends can mess with this boolean and 
//  game tracking but I haven't found a good workaround yet.
bool endedGame = true;
int decimalPlaces;

// global vars to control overlay
// are initialized in onLoad()
bool enabledOverlay;
int overlayNum;
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
// total number of stats in the stat array
constexpr int numStats = 55;

// jump from a stat to its most recent game stat by adding this number
constexpr int totalToGame = 26;
// end of stats without game counterpart
constexpr int statsWithoutGame = games;
// start of game stats and/or 1 after the end of non-game stats
constexpr int startGameStats = gameGoals;

// maps the stat indexes to their respective strings to write files
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

const std::map<std::string, int> eventDictionary = {
    { "Demolition", demos},
    { "Extermination", exterms},
    { "Goal", goals},
    { "Win", wins},
    { "MVP", mvps},
    { "Aerial Goal", aerialGoals},
    { "Backwards Goal", backwardsGoals},
    { "Bicycle Goal", bicycleGoals},
    { "Long Goal", longGoals},
    { "Turtle Goal", turtleGoals},
    { "Pool Shot", poolShots},
    { "Overtime Goal", overtimeGoals},
    { "Hat Trick", hatTricks},
    { "Assist", assists},
    { "Playmaker", playmakers},
    { "Save", saves},
    { "Epic Save", epicSaves},
    { "Savior", saviors},
    { "Shot on Goal", shots},
    { "Center Ball", centers},
    { "Clear Ball", clears},
    { "First Touch", firstTouchs},
    { "Damage", damages},
    { "Ultra Damage", ultraDamages},
    { "Low Five", lowFives},
    { "High Five", highFives},
    { "Swish Goal", swishs}
};

// holds all averages
// caching improves performance significantly
float averages[startGameStats];

// called when the plugin loads and prepares all cvars and default values
void DemolitionCounter::onLoad()
{
    // tells the plugin to render the in game overlay
    gameWrapper->RegisterDrawable(
        std::bind(&DemolitionCounter::render, this, std::placeholders::_1));

    // creates cvars and sets global variable defaults to prevent any nulls
    setCvars();

    // hooks the events for the plugin to work
    hookEvents();
}

// creates cvars and sets global variable defaults to prevent any nulls
void DemolitionCounter::setCvars() {
    auto decimalsVar = cvarManager->registerCvar("counter_decimals", "2",
        "set decimal places in averages (1 - 10)");
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
    auto overlayEnableVar = cvarManager->registerCvar("counter_enable_ingame",
        "0", "enables in game overlay");
    enabledOverlay = overlayEnableVar.getBoolValue();
    overlayEnableVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        enabledOverlay = cvar.getBoolValue();
        });

    // sets number of stats in in game overlay
    // 1-5 allowed
    auto overlayNumberVar = cvarManager->registerCvar(
        "counter_ingame_numStats", "5", "number of stats in in game overlay",
        true, true, 1, true, 5);
    overlayNum = overlayNumberVar.getIntValue();
    overlayNumberVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayNum = cvar.getIntValue();
        });

    // first stat in overlay (defaults to demos/4)
    auto overlayOneVar = cvarManager->registerCvar("counter_ingame_stat_one",
        "4", "First stat in overlay", true, true, 0, true, numStats - 1);
    overlayStats[0] = overlayOneVar.getIntValue();
    overlayOneVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayStats[0] = cvar.getIntValue();
        });

    // sets first stat in overlay to average or not
    auto overlayOneAvgVar = cvarManager->registerCvar(
        "counter_ingame_stat_one_average", "0",
        "Toggles average of first stat in overlay", true, true, 0, true, 1);
    overlayAverages[0] = overlayOneAvgVar.getBoolValue();
    overlayOneAvgVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayAverages[0] = cvar.getBoolValue();
        });

    // second stat in overlay (defaults to exterms/6)
    auto overlayTwoVar = cvarManager->registerCvar("counter_ingame_stat_two",
        "6", "Second stat in overlay", true, true, 0, true, numStats - 1);
    overlayStats[1] = overlayTwoVar.getIntValue();
    overlayTwoVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayStats[1] = cvar.getIntValue();
        });

    // sets second stat in overlay to average or not
    auto overlayTwoAvgVar = cvarManager->registerCvar(
        "counter_ingame_stat_two_average", "0",
        "Toggles average of second stat in overlay", true, true, 0, true, 1);
    overlayAverages[1] = overlayTwoAvgVar.getBoolValue();
    overlayTwoAvgVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayAverages[1] = cvar.getBoolValue();
        });

    // third stat in overlay (defaults to gameDemos/30)
    auto overlayThreeVar = cvarManager->registerCvar(
        "counter_ingame_stat_three", "30", "Third stat in overlay",
        true, true, 0, true, numStats - 1);
    overlayStats[2] = overlayThreeVar.getIntValue();
    overlayThreeVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayStats[2] = cvar.getIntValue();
        });

    // sets third stat in overlay to average or not
    auto overlayThreeAvgVar = cvarManager->registerCvar(
        "counter_ingame_stat_three_average", "0",
        "Toggles average of third stat in overlay", true, true, 0, true, 1);
    overlayAverages[2] = overlayThreeAvgVar.getBoolValue();
    overlayThreeAvgVar.addOnValueChanged(
        [this](std::string, CVarWrapper cvar) {
            overlayAverages[2] = cvar.getBoolValue();
        });

    // fourth stat in overlay (defaults to games/2)
    auto overlayFourVar = cvarManager->registerCvar("counter_ingame_stat_four",
        "2", "Fourth stat in overlay", true, true, 0, true, numStats - 1);
    overlayStats[3] = overlayFourVar.getIntValue();
    overlayFourVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayStats[3] = cvar.getIntValue();
        });

    // sets fourth stat in overlay to average or not
    auto overlayFourAvgVar = cvarManager->registerCvar(
        "counter_ingame_stat_four_average", "0",
        "Toggles average of fourth stat in overlay", true, true, 0, true, 1);
    overlayAverages[3] = overlayFourAvgVar.getBoolValue();
    overlayFourAvgVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayAverages[3] = cvar.getBoolValue();
        });

    // fifth stat in overlay (defaults to deaths/5)
    auto overlayFiveVar = cvarManager->registerCvar(
        "counter_ingame_stat_five", "5", "Fifth stat in overlay",
        true, true, 0, true, numStats - 1);
    overlayStats[4] = overlayFiveVar.getIntValue();
    overlayFiveVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayStats[4] = cvar.getIntValue();
        });

    // sets fifth stat in overlay to average or not
    auto overlayFiveAvgVar = cvarManager->registerCvar(
        "counter_ingame_stat_five_average", "0",
        "Toggles average of fifth stat in overlay", true, true, 0, true, 1);
    overlayAverages[4] = overlayFiveAvgVar.getBoolValue();
    overlayFiveAvgVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayAverages[4] = cvar.getBoolValue();
        });

    // sets cvar to move counter's X location
    auto xLocVar = cvarManager->registerCvar("counter_ingame_x_location",
        "0.86", "set location of ingame counter X in % of screen",
        true, true, 0.0, true, 1.0);
    xLocation = xLocVar.getFloatValue();
    xLocVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        xLocation = cvar.getFloatValue();
        });

    // sets cvar to move counter's Y location
    auto yLocVar = cvarManager->registerCvar("counter_ingame_y_location",
        "0.025", "set location of ingame counter Y in % of screen",
        true, true, 0.0, true, 1.0);
    yLocation = yLocVar.getFloatValue();
    yLocVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        yLocation = cvar.getFloatValue();
        });

    // overlay red value changer 
    auto overlayRedVar = cvarManager->registerCvar("counter_ingame_red",
        "255", "Red value in overlay", true, true, 0, true, 255);
    overlayColors[0] = overlayRedVar.getIntValue();
    overlayRedVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayColors[0] = cvar.getIntValue();
        });

    // overlay green value changer 
    auto overlayGreenVar = cvarManager->registerCvar("counter_ingame_green",
        "0", "green value in overlay", true, true, 0, true, 255);
    overlayColors[1] = overlayGreenVar.getIntValue();
    overlayGreenVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayColors[1] = cvar.getIntValue();
        });

    // overlay blue value changer 
    auto overlayBlueVar = cvarManager->registerCvar("counter_ingame_blue",
        "0", "blue value in overlay", true, true, 0, true, 255);
    overlayColors[2] = overlayBlueVar.getIntValue();
    overlayBlueVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayColors[2] = cvar.getIntValue();
        });

    // lists all the stats and their numbers to use in the in game counter
    cvarManager->registerNotifier("counter_list_stats",
        [this](std::vector<std::string> params) {
            listStats();
        }, "List all different stat types", PERMISSION_ALL);

    // adds 1 to games to fix any errors in game tracking
    cvarManager->registerNotifier("counter_add_game",
        [this](std::vector<std::string> params) {
            endedGame = true;
            startGame();
            endedGame = true;
        }, "Add a game to stats to fix any sync issues", PERMISSION_ALL);

    // creates setters for the default start value for each stat
    // also writes these new values to files
    for (int i = 0; i < startGameStats; i++) {
        std::string cvarName = "counter_set_" + indexStringMap[i];
        std::string cvarTip = "sets " + indexStringMap[i] + " value";
        cvarManager->registerCvar(cvarName, "0", cvarTip, true, false, 0, false, 0, false);
        auto cvar = cvarManager->getCvar(cvarName);
        statArray[i] = cvar.getIntValue();
        
        if (i != games) {
            cvar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
                statArray[i] = cvar.getIntValue(); write(i); });
        }
    }

    // special case to make sure games update properly
    // needs to call startGame() to ensure averages and 
    //  game stats are written
    cvarManager->getCvar("counter_set_games").addOnValueChanged(
        [this](std::string, CVarWrapper cvar) {
            statArray[games] = cvar.getIntValue() - 1; endedGame = true;
            startGame(); endedGame = true; });;
}

// hooks events to allow the plugin to work
void DemolitionCounter::hookEvents() {
    // hooked whenever a stat appears in the top right corner of rocket league
    //  (even if stat display is turned off)
    gameWrapper->HookEventWithCaller<ServerWrapper>(
        "Function TAGame.GFxHUD_TA.HandleStatTickerMessage", 
        std::bind(&DemolitionCounter::statEvent, this, 
            std::placeholders::_1, std::placeholders::_2));

    // hookss on a starting game 
    gameWrapper->HookEventPost(
        "Function GameEvent_Soccar_TA.WaitingForPlayers.BeginState", 
        std::bind(&DemolitionCounter::startGame, this));
    // hooks on a joined game in progress
    gameWrapper->HookEventPost("Function Engine.PlayerInput.InitInputSystem",
        std::bind(&DemolitionCounter::startGame, this));
    // works on ended game
    gameWrapper->HookEventPost(
        "Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", 
        [this](std::string eventName) { endedGame = true; });
}

// The structure of a stat event
struct TheArgStruct
{
    // person who got a stat
    uintptr_t Receiver;
    // person who is victim of a stat (only exists for demos afaik)
    uintptr_t Victim;
    // wrapper for the stat event
    uintptr_t StatEvent;
};

// called whenever a stat appears in the top right and 
//  decides whether to update a file
void DemolitionCounter::statEvent(ServerWrapper caller, void* args) {
    auto tArgs = (TheArgStruct*)args;
    //cvarManager->log("stat event!");

    // separates the parts of the stat event args
    auto victim = PriWrapper(tArgs->Victim);
    auto receiver = PriWrapper(tArgs->Receiver);
    auto statEvent = StatEventWrapper(tArgs->StatEvent);
    // name of the stat as shown in rocket league 
    //  (Demolition, Extermination, etc.)
    auto label = statEvent.GetLabel();
    //cvarManager->log(label.ToString());

    auto eventTypePtr = eventDictionary.find(label.ToString());

    int eventType;

    if (eventTypePtr != eventDictionary.end()) {
        eventType = eventTypePtr->second;
        cvarManager->log("event type: " + label.ToString());
        cvarManager->log("event num: " + std::to_string(eventType));
    }
    else {
        cvarManager->log("missing stat: " + label.ToString());
        return;
    }

    // special case for demolitions to check for the player's death
    if (eventType == demos) {
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

    // if the stat reciever isn't the person with the plugin, 
    //  doesn't count the stat
    if (!DemolitionCounter::isPrimaryPlayer(receiver)) {
        return;
    }

    // Checks for each possible event name
    // no switches in c++ for strings so this looks kind of ugly :(
   /* if (label.ToString().compare("Demolition") == 0) {
        statArray[demolitions]++;
        write(demolitions);
        return; 
    }*/
    // if the stat event matches a label, 
    // then writes that stat to files
    statArray[eventType]++;
    // adds 1 to the game stat equivalent if applicable
    if (eventType > statsWithoutGame) {
        statArray[eventType + totalToGame]++;
    }
    write(eventType);
}

// checks if the player who received a stat is the user's player
bool DemolitionCounter::isPrimaryPlayer(PriWrapper receiver) {
    // anything down this conversion line can be null, so always checks
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
    // can lead to issues if the user ragequits. No good workaround yet
    if (!endedGame) {
        cvarManager->log("last game didn't end");
        return;
    }
    endedGame = false;

    statArray[games]++;

    // resets all game stats to 0 and writes each one
    for (int i = startGameStats; i < numStats; i++) {
        statArray[i] = 0;
        writeGameStat(i);
    }

    // writes all the averages
    for (int i = 0; i < startGameStats; i++) {
        average(i);
    }

    write(games);
}

// writes a stat to its files
// can only be called with a stat index and not a game stat index
void DemolitionCounter::write(int statIndex) {
    // writes the total stat
    std::ofstream totalFile;
    totalFile.open("./OBSCounter/" + indexStringMap[statIndex] + ".txt");
    totalFile << statArray[statIndex];
    totalFile.close();

    // doesn't write averageGames, 
    //  as that would just be games/games and always 1
    if (statIndex != games) {
        // writes average of stat per game
        std::ofstream averageFile;
        // sets up average file location
        std::string averageLocation = "./OBSCounter/";
        // makes the first letter uppercase for nice looking files
        averageLocation += averageStrings[statIndex] + ".txt";
        averageFile.open(averageLocation);

        // calls function to get average of a stat
        float averageStat = average(statIndex);

        // sets number of decimal places based on user input
        averageFile << std::fixed << std::setprecision(decimalPlaces);
        averageFile << averageStat;
        averageFile.close();
    }

    // writes the game version of stat
    // only writes if stat has a game version
    if (statIndex > statsWithoutGame) {
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

// writes a game stat, only supposed to be used with a game stat index 
void DemolitionCounter::writeGameStat(int statIndex) {
    std::ofstream gameStatFile;
    gameStatFile.open("./OBSCounter/" + indexStringMap[statIndex] + ".txt");
    gameStatFile << statArray[statIndex];
    gameStatFile.close();
}

// calls all write functions at once
void DemolitionCounter::writeAll() {
    for (int i = 0; i < startGameStats; i++) {
        write(i);
    }
}

// special cases for extra complicated stats
// calculates shooting percentage (shots/goals)
void DemolitionCounter::writeShootingPercentage() {
    // calculates current game shooting %
    std::ofstream gameFile;
    // divides and checks for NaN
    float gameShooting = divide(gameGoals, gameShots);
    gameFile.open("./OBSCounter/gameShootingPercentage.txt");
    gameFile << std::fixed << std::setprecision(decimalPlaces);
    gameFile << gameShooting;
    gameFile.close();

    // writes total session shooting
    float totalShooting = divide(goals, shots);
    std::ofstream file;
    file.open("./OBSCounter/shootingPercentage.txt");
    file << std::fixed << std::setprecision(decimalPlaces);
    file << totalShooting;
    file.close();
}

// writes K/D ratio
void DemolitionCounter::writeKillPercentage() {
    std::ofstream gameFile;
    float gameKD = divide(gameDemos, gameDeaths);
    gameFile.open("./OBSCounter/gameKDRatio.txt");
    gameFile << std::fixed << std::setprecision(decimalPlaces);
    gameFile << gameKD;
    gameFile.close();

    float totalKDRatio = divide(demos, deaths);
    std::ofstream file;
    file.open("./OBSCounter/KDRatio.txt");
    file << std::fixed << std::setprecision(decimalPlaces);
    file << totalKDRatio;
    file.close();
}

// writes missed exterm % for the session
// total exterms / possible exterms (demos / 7) 
void DemolitionCounter::writeMissedExterms() {
    // calculates possible exterms (demos / 7)
    std::ofstream totalFile;
    int possibleExterms = statArray[demos] / 7;
    totalFile.open("./OBSCounter/possibleExterminations.txt");
    totalFile << possibleExterms;
    totalFile.close();

    int missedExtermPercent;
    if (possibleExterms == 0) {
        missedExtermPercent = 0;
    }
    else {
        missedExtermPercent = 
            ((float)statArray[exterms] / (float)possibleExterms) * 100;
    }
    std::ofstream file;
    file.open("./OBSCounter/missedExterminationPercent.txt");
    file << missedExtermPercent << "%";
    file.close();
}

// Renders in game overlay
void DemolitionCounter::render(CanvasWrapper canvas) {
    if (!enabledOverlay) {
        return;
    }

    Vector2 screen = canvas.GetSize();

    // in 1080p font size is 2
    // y value of 2 size text is approx 20
    // scaling has no documentation so I think this works 
    //  but am unable to test on high fidelity monitors
    // if you notice an issue please let me 
    //  know by @ing me in the bakkesmod discord
    float fontSize = ((float)screen.X / (float)1920) * 2;

    // sets to user-chosen color
    canvas.SetColor(overlayColors[0], overlayColors[1], overlayColors[2], 255);

    for (int i = 0; i < overlayNum; i++) {
        // locates based on screen and font size
        canvas.SetPosition(Vector2({ int((float)screen.X * xLocation), 
            int((fontSize * (11 * i)) + ((float) screen.Y * yLocation)) }));
        // does averages if the user wants them and if a stat has an average
        if (overlayAverages[i] && overlayStats[i] < startGameStats) {
            // makes sure string has right number of decimal places
            std::ostringstream averageStream;
            averageStream << std::fixed << std::setprecision(decimalPlaces);
            averageStream << averages[overlayStats[i]];

            canvas.DrawString(averageStrings[overlayStats[i]] + ": " + 
                averageStream.str(), fontSize, fontSize);
        }
        else {
            canvas.DrawString(indexStringMap[overlayStats[i]] + ": " + 
                std::to_string(statArray[overlayStats[i]]), 
                fontSize, fontSize);
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
    averages[statIndex] = divide(statIndex, games);
    return averages[statIndex];
}

// calculates division of two stats and avoids NaN
// prevents a bunch of duplicated code
float DemolitionCounter::divide(int firstStatIndex, int secondStatIndex) {
    // if second number is 0 returns 0
    if (statArray[secondStatIndex] == 0) {
        return 0.0;
    }
    // otherwise divides ints as a float
    else {
        return (float)statArray[firstStatIndex] / 
            (float)statArray[secondStatIndex];
    }
}

// plugin is unloaded by the plugin manager, and as no state needs to be saved,
// allows it to be removed
void DemolitionCounter::onUnload()
{
}