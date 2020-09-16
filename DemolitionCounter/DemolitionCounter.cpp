#include "pch.h"
#include "DemolitionCounter.h"
#include <sstream>
#include <fstream>
#include <filesystem>
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
LinearColor overlayColor;
bool nextNotify = true;
std::string fileLocation = "./OBSCounter/";

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
    // total number of stats in the stat array
    numStats
};

// jump from a stat to its most recent game stat by adding this number (27)
// goals + 27 = gameGoals
constexpr int totalToGame = startGameStats - 1 - statsWithoutGame;

// maps the stat indexes to their respective strings to write files
// index to string
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
};

// holds all stats
int statArray[numStats];

// holds strings for averages (doesn't include game stats)
std::string averageStrings[] = {
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
    { "Swish Goal", swishs},
    { "Bicycle Hit", bicycleHits}
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

    namespace fs = std::filesystem;
    fs::create_directory(fileLocation);
}

// creates cvars and sets global variable defaults to prevent any nulls
void DemolitionCounter::setCvars() {
    auto decimalsVar = cvarManager->registerCvar("counter_decimals", "1",
        "set decimal places in averages (1 - 10)", true, true, 1, true, 10);
    decimalPlaces = decimalsVar.getIntValue();
    decimalsVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        decimalPlaces = cvar.getIntValue();
        writeAll();
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

    std::string numberStrings[] = {
        "one",
        "two",
        "three",
        "four",
        "five"
    };

    for (int i = 0; i < 5; i++) {
        std::string str = numberStrings[i];
        // sets stat in overlay
        auto overlayVar = cvarManager->registerCvar("counter_ingame_stat_" + str,
            std::to_string(i), "stat " + str + " in overlay", true, true, 0, true, numStats - 1);
        overlayStats[i] = overlayVar.getIntValue();
        overlayVar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            overlayStats[i] = cvar.getIntValue();
            });

        // sets stat in overlay to average or not
        auto overlayAvgVar = cvarManager->registerCvar(
            "counter_ingame_stat_" + str + "_average", "0",
            "Toggles average of stat " + str + " in overlay", true, true, 0, true, 1);
        overlayAverages[i] = overlayAvgVar.getBoolValue();
        overlayAvgVar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            overlayAverages[i] = cvar.getBoolValue();
            });
    }

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
        "0", "set location of ingame counter Y in % of screen",
        true, true, 0.0, true, 1.0);
    yLocation = yLocVar.getFloatValue();
    yLocVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        yLocation = cvar.getFloatValue();
        });

    auto colorVar = cvarManager->registerCvar("counter_color", "#FFFFFF", "color of overlay");
    overlayColor = colorVar.getColorValue();
    colorVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayColor = cvar.getColorValue();
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
    gameWrapper->HookEventWithCallerPost<ServerWrapper>(
        "Function TAGame.GFxHUD_TA.HandleStatTickerMessage", 
        std::bind(&DemolitionCounter::statTickerEvent, this, 
            std::placeholders::_1, std::placeholders::_2));

    // hooked whenever the primary player earns a stat
    gameWrapper->HookEventWithCallerPost<ServerWrapper>(
        "Function TAGame.GFxHUD_TA.HandleStatEvent",
        std::bind(&DemolitionCounter::statEvent, this,
            std::placeholders::_1, std::placeholders::_2));

    // hooks on a starting game 
    gameWrapper->HookEventPost(
        "Function GameEvent_Soccar_TA.WaitingForPlayers.BeginState", 
        std::bind(&DemolitionCounter::startGame, this));
    // hooks on a joined game in progress
    gameWrapper->HookEventPost("Function Engine.PlayerInput.InitInputSystem",
        std::bind(&DemolitionCounter::startGame, this));
    // works on ended game
    gameWrapper->HookEventPost(
        "Function TAGame.GameEvent_Soccar_TA.EventMatchEnded",
        std::bind(&DemolitionCounter::endGame, this));
}

// The structure of a ticker stat event
struct TickerStruct {
    // person who got a stat
    uintptr_t Receiver;
    // person who is victim of a stat (only exists for demos afaik)
    uintptr_t Victim;
    // wrapper for the stat event
    uintptr_t StatEvent;
};

// structure of a stat event
struct StatEventStruct {
    uintptr_t PRI; 
    uintptr_t StatEvent; 
    // Count always is int_max. No idea why
    uintptr_t Count;
};

void DemolitionCounter::statEvent(ServerWrapper caller, void* args) {
    cvarManager->log("stat event!");
    if (!gameWrapper->IsInOnlineGame()) {
        cvarManager->log("not in online game");
        return;
    }
    auto tArgs = (StatEventStruct*)args;

    auto statEvent = StatEventWrapper(tArgs->StatEvent);
    auto label = statEvent.GetLabel();
    auto eventStr = label.ToString();

    // PRI is always main player for this event
    /*auto pri = PriWrapper(tArgs->PRI);
    cvarManager->log("is primary: " + std::to_string(pri.IsLocalPlayerPRI()));*/

    // Count always is int_max. No idea why
    /*auto count = tArgs->Count;
    cvarManager->log("count: " + std::to_string(count));*/

    auto eventTypePtr = eventDictionary.find(eventStr);

    int eventType;

    if (eventTypePtr != eventDictionary.end()) {
        eventType = eventTypePtr->second;
        cvarManager->log("event type: " + eventStr);
        cvarManager->log("event num: " + std::to_string(eventType));
    }
    else {
        cvarManager->log("missing stat: " + eventStr);
        return;
    }
    
    int statPoints = statEvent.GetPoints();
    cvarManager->log("points: " + std::to_string(statPoints));
    if (statPoints > 0) {
        statArray[points] += statPoints;
        statArray[gamePoints] += statPoints;
        write(points);
    }
    statArray[eventType]++;
    // adds 1 to the game stat equivalent if applicable
    if (eventType > statsWithoutGame) {
        statArray[eventType + totalToGame]++;
    }
    write(eventType);
}

void DemolitionCounter::statTickerEvent(ServerWrapper caller, void* args) {
    auto tArgs = (TickerStruct*)args;
    cvarManager->log("stat ticker event!");
    if (!gameWrapper->IsInOnlineGame()) {
        cvarManager->log("not in online game");
        return;
    }

    // separates the parts of the stat event args
    auto receiver = PriWrapper(tArgs->Receiver);
    auto victim = PriWrapper(tArgs->Victim);
    auto statEvent = StatEventWrapper(tArgs->StatEvent);
    // name of the stat as shown in rocket league 
    //  (Demolition, Extermination, etc.)
    auto label = statEvent.GetLabel();

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
    if (eventType == demos && isPrimaryPlayer(victim)) {
        cvarManager->log("player died");
        statArray[deaths]++;
        statArray[gameDeaths]++;
        write(deaths);
        return;
    }
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

void DemolitionCounter::endGame() {
    if (!gameWrapper->IsInOnlineGame()) {
        cvarManager->log("not in online game");
        return;
    }

    ServerWrapper sw = gameWrapper->GetOnlineGame();

    if (sw.IsNull()) {
        cvarManager->log("null server");
        return;
    }

    auto primary = sw.GetLocalPrimaryPlayer();

    if (primary.IsNull()) {
        cvarManager->log("null primary player");
        return;
    }

    auto primaryNum = primary.GetTeamNum2();

    auto winner = sw.GetWinningTeam();

    if (winner.IsNull()) {
        cvarManager->log("null winner");
        return;
    }

    auto winningNum = winner.GetTeamNum2();

    if (winningNum != primaryNum) {
        statArray[losses]++;
        write(losses);
    }

    endedGame = true;
}

// writes a stat to its files
// can only be called with a stat index and not a game stat index
void DemolitionCounter::write(int statIndex) {
    // writes the total stat
    std::ofstream totalFile;
    totalFile.open(fileLocation + indexStringMap[statIndex] + ".txt");
    totalFile << statArray[statIndex];
    totalFile.close();

    // doesn't write averageGames, 
    //  as that would just be games/games and always 1
    if (statIndex != games) {
        // writes average of stat per game
        std::ofstream averageFile;
        // sets up average file location
        std::string averageLocation = fileLocation;
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
    // wins / losses (win or loss)
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
        break;
    case wins:
    case losses:
        writeWinPercentage();
        break;
    }
}

// writes a game stat, only supposed to be used with a game stat index 
void DemolitionCounter::writeGameStat(int statIndex) {
    std::ofstream gameStatFile;
    gameStatFile.open(fileLocation + indexStringMap[statIndex] + ".txt");
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
    int gameShooting = getPercentage(statArray[gameGoals], statArray[gameShots]);
    gameFile.open(fileLocation + "gameShootingPercentage.txt");
    gameFile << gameShooting << "%";
    gameFile.close();

    // writes total session shooting
    int totalShooting = getPercentage(statArray[goals], statArray[shots]);
    std::ofstream file;
    file.open(fileLocation + "shootingPercentage.txt");
    file << totalShooting << "%";
    file.close();
}

// writes K/D ratio
void DemolitionCounter::writeKillPercentage() {
    std::ofstream gameFile;
    float gameKD = divide(gameDemos, gameDeaths);
    gameFile.open(fileLocation + "gameKDRatio.txt");
    gameFile << std::fixed << std::setprecision(decimalPlaces);
    gameFile << gameKD;
    gameFile.close();

    float totalKDRatio = divide(demos, deaths);
    std::ofstream file;
    file.open(fileLocation + "KDRatio.txt");
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
    totalFile.open(fileLocation + "possibleExterminations.txt");
    totalFile << possibleExterms;
    totalFile.close();

    int missedExtermPercent = getPercentage(statArray[exterms], possibleExterms);
    
    std::ofstream file;
    file.open(fileLocation + "missedExterminationPercent.txt");
    file << missedExtermPercent << "%";
    file.close();
}

void DemolitionCounter::writeWinPercentage() {
    int winPercent = getPercentage(statArray[wins], statArray[wins] + statArray[losses]);

    std::ofstream file;
    file.open(fileLocation + "winPercent.txt");
    file << winPercent << "%";
    file.close();
}

int DemolitionCounter::getPercentage(int numerator, int denominator) {
    if (denominator == 0) {
        return 0;
    }
    else {
       return ((float)numerator / (float) denominator) * 100;
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
    // scaling has no documentation so I think this works 
    //  but am unable to test on high fidelity monitors
    // if you notice an issue please let me 
    //  know by @ing me in the bakkesmod discord
    float fontSize = ((float)screen.X / (float)1920) * 2;

    // sets to user-chosen color
    //canvas.SetColor(overlayColors[0], overlayColors[1], overlayColors[2], 255);
    canvas.SetColor(overlayColor);

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