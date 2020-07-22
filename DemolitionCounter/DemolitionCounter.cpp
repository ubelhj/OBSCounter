#include "pch.h"
#include "DemolitionCounter.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

BAKKESMOD_PLUGIN(DemolitionCounter, "Counts demolitions in online games", plugin_version, PLUGINTYPE_THREADED)

bool endedGame = true;
int decimalPlaces = 2;

// Macros for all stat indexes 
// easier to refer back with macro
constexpr int wins = 0;
constexpr int mvps = 1;
constexpr int games = 2;
constexpr int demos = 3;
constexpr int deaths = 4;
constexpr int exterms = 5;
constexpr int goals = 6;
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

// holds all stats
int statArray[numStats];

void DemolitionCounter::onLoad()
{
    cvarManager->log("Plugin loaded!");
    auto decimalsVar = cvarManager->registerCvar("counter_decimals", "2", "set decimal places in averages (1 - 10)");
    decimalsVar.addOnValueChanged([this](std::string, CVarWrapper cvar) { 
            int newValue = cvar.getIntValue();
            if (newValue >= 1 && newValue <= 10) {
                decimalPlaces = newValue;
                writeAll();
            }
        });

    // setters for totals
    setCvars();

    hookEvents();
    
    writeAll();
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
        statArray[swishs]++;
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

    writeAll();
}

void DemolitionCounter::endGame() {
    endedGame = true;
}

// writing stats to to files
void DemolitionCounter::writeDemos() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageDemos.txt");
    float averageDemos;
    if (games == 0) {
        averageDemos = 0.00;
    } else {
        averageDemos = (float) demos / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageDemos;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/demolitions.txt");
    file << std::to_string(demos);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameDemolitions.txt");
    gameFile << std::to_string(gameDemos);
    gameFile.close();
}
void DemolitionCounter::writeDeaths() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageDeaths.txt");
    float averageDeaths;
    if (games == 0) {
        averageDeaths = 0.00;
    }
    else {
        averageDeaths = (float)deaths / (float)games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageDeaths;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/deaths.txt");
    file << std::to_string(deaths);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameDeaths.txt");
    gameFile << std::to_string(gameDeaths);
    gameFile.close();
}
void DemolitionCounter::writeGoals() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageGoals.txt");
    float averageGoals;
    if (games == 0) {
        averageGoals = 0;
    } else {
        averageGoals = (float) goals / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageGoals;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/goals.txt");
    file << std::to_string(goals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameGoals.txt");
    gameFile << std::to_string(gameGoals);
    gameFile.close();
}
void DemolitionCounter::writeAerialGoals() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageAerialGoals.txt");
    float averageAerialGoals;
    if (games == 0) {
        averageAerialGoals = 0;
    } else {
        averageAerialGoals = (float) aerialGoals / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageAerialGoals;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/aerialGoals.txt");
    file << std::to_string(aerialGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameAerialGoals.txt");
    gameFile << std::to_string(gameAerialGoals);
    gameFile.close();
}
void DemolitionCounter::writeBackwardsGoals() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageBackwardsGoals.txt");
    float averageBackwardsGoals;
    if (games == 0) {
        averageBackwardsGoals = 0;
    } else {
        averageBackwardsGoals = (float) backwardsGoals / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageBackwardsGoals;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/backwardsGoals.txt");
    file << std::to_string(backwardsGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameBackwardsGoals.txt");
    gameFile << std::to_string(gameBackwardsGoals);
    gameFile.close();
}
void DemolitionCounter::writeBicycleGoals() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageBicycleGoals.txt");
    float averageBicycleGoals;
    if (games == 0) {
        averageBicycleGoals = 0;
    } else {
        averageBicycleGoals = (float) bicycleGoals / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageBicycleGoals;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/bicycleGoals.txt");
    file << std::to_string(bicycleGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameBicycleGoals.txt");
    gameFile << std::to_string(gameBicycleGoals);
    gameFile.close();
}
void DemolitionCounter::writeLongGoals() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageLongGoals.txt");
    float averageLongGoals;
    if (games == 0) {
        averageLongGoals = 0;
    } else {
        averageLongGoals = (float) longGoals / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageLongGoals;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/longGoals.txt");
    file << std::to_string(longGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameLongGoals.txt");
    gameFile << std::to_string(gameLongGoals);
    gameFile.close();
}
void DemolitionCounter::writeTurtleGoals() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageTurtleGoals.txt");
    float averageTurtleGoals;
    if (games == 0) {
        averageTurtleGoals = 0;
    } else {
        averageTurtleGoals = (float) turtleGoals / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageTurtleGoals;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/turtleGoals.txt");
    file << std::to_string(turtleGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameTurtleGoals.txt");
    gameFile << std::to_string(gameTurtleGoals);
    gameFile.close();
}
void DemolitionCounter::writePoolShots() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averagePoolShots.txt");
    float averagePoolShots;
    if (games == 0) {
        averagePoolShots = 0;
    } else {
        averagePoolShots = (float) poolShots / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averagePoolShots;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/poolShots.txt");
    file << std::to_string(poolShots);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gamePoolShots.txt");
    gameFile << std::to_string(gamePoolShots);
    gameFile.close();
}
void DemolitionCounter::writeOvertimeGoals() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageOvertimeGoals.txt");
    float averageOvertimeGoals;
    if (games == 0) {
        averageOvertimeGoals = 0;
    } else {
        averageOvertimeGoals = (float) overtimeGoals / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageOvertimeGoals;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/overtimeGoals.txt");
    file << std::to_string(overtimeGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameOvertimeGoals.txt");
    gameFile << std::to_string(gameOvertimeGoals);
    gameFile.close();
}
void DemolitionCounter::writeHatTricks() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageHatTricks.txt");
    float averageHatTricks;
    if (games == 0) {
        averageHatTricks = 0;
    } else {
        averageHatTricks = (float) hatTricks / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageHatTricks;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/hatTricks.txt");
    file << std::to_string(hatTricks);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameHatTricks.txt");
    gameFile << std::to_string(gameHatTricks);
    gameFile.close();
}
void DemolitionCounter::writeAssists() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageAssists.txt");
    float averageAssists;
    if (games == 0) {
        averageAssists = 0;
    } else {
        averageAssists = (float) assists / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageAssists;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/assists.txt");
    file << std::to_string(assists);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameAssists.txt");
    gameFile << std::to_string(gameAssists);
    gameFile.close();
}
void DemolitionCounter::writePlaymakers() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averagePlaymakers.txt");
    float averagePlaymakers;
    if (games == 0) {
        averagePlaymakers = 0;
    } else {
        averagePlaymakers = (float) playmakers / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averagePlaymakers;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/playmakers.txt");
    file << std::to_string(playmakers);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gamePlaymakers.txt");
    gameFile << std::to_string(gamePlaymakers);
    gameFile.close();
}
void DemolitionCounter::writeSaves() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageSaves.txt");
    float averageSaves;
    if (games == 0) {
        averageSaves = 0;
    } else {
        averageSaves = (float) saves / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageSaves;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/saves.txt");
    file << std::to_string(saves);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameSaves.txt");
    gameFile << std::to_string(gameSaves);
    gameFile.close();
}
void DemolitionCounter::writeEpicSaves() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageEpicSaves.txt");
    float averageEpicSaves;
    if (games == 0) {
        averageEpicSaves = 0;
    } else {
        averageEpicSaves = (float) epicSaves / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageEpicSaves;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/epicSaves.txt");
    file << std::to_string(epicSaves);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameEpicSaves.txt");
    gameFile << std::to_string(gameEpicSaves);
    gameFile.close();
}
void DemolitionCounter::writeSaviors() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageSaviors.txt");
    float averageSaviors;
    if (games == 0) {
        averageSaviors = 0;
    } else {
        averageSaviors = (float) saviors / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageSaviors;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/saviors.txt");
    file << std::to_string(saviors);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameSaviors.txt");
    gameFile << std::to_string(gameSaviors);
    gameFile.close();
}
void DemolitionCounter::writeShots() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageShots.txt");
    float averageShots;
    if (games == 0) {
        averageShots = 0;
    } else {
        averageShots = (float) shots / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageShots;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/shots.txt");
    file << std::to_string(shots);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameShots.txt");
    gameFile << std::to_string(gameShots);
    gameFile.close();
}
void DemolitionCounter::writeCenters() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageCenters.txt");
    float averageCenters;
    if (games == 0) {
        averageCenters = 0;
    } else {
        averageCenters = (float) centers / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageCenters;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/centers.txt");
    file << std::to_string(centers);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameCenters.txt");
    gameFile << std::to_string(gameCenters);
    gameFile.close();
}
void DemolitionCounter::writeClears() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageClears.txt");
    float averageClears;
    if (games == 0) {
        averageClears = 0;
    } else {
        averageClears = (float) clears / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageClears;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/clears.txt");
    file << std::to_string(clears);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameClears.txt");
    gameFile << std::to_string(gameClears);
    gameFile.close();
}
void DemolitionCounter::writeFirstTouchs() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageFirstTouchs.txt");
    float averageFirstTouchs;
    if (games == 0) {
        averageFirstTouchs = 0;
    } else {
        averageFirstTouchs = (float) firstTouchs / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageFirstTouchs;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/firstTouchs.txt");
    file << std::to_string(firstTouchs);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameFirstTouchs.txt");
    gameFile << std::to_string(gameFirstTouchs);
    gameFile.close();
}
void DemolitionCounter::writeDamages() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageDamages.txt");
    float averageDamages;
    if (games == 0) {
        averageDamages = 0;
    } else {
        averageDamages = (float) damages / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageDamages;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/damages.txt");
    file << std::to_string(damages);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameDamages.txt");
    gameFile << std::to_string(gameDamages);
    gameFile.close();
}
void DemolitionCounter::writeUltraDamages() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageUltraDamages.txt");
    float averageUltraDamages;
    if (games == 0) {
        averageUltraDamages = 0;
    } else {
        averageUltraDamages = (float) ultraDamages / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageUltraDamages;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/ultraDamages.txt");
    file << std::to_string(ultraDamages);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameUltraDamages.txt");
    gameFile << std::to_string(gameUltraDamages);
    gameFile.close();
}
void DemolitionCounter::writeLowFives() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageLowFives.txt");
    float averageLowFives;
    if (games == 0) {
        averageLowFives = 0;
    } else {
        averageLowFives = (float) lowFives / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageLowFives;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/lowFives.txt");
    file << std::to_string(lowFives);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameLowFives.txt");
    gameFile << std::to_string(gameLowFives);
    gameFile.close();
}
void DemolitionCounter::writeHighFives() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageHighFives.txt");
    float averageHighFives;
    if (games == 0) {
        averageHighFives = 0;
    } else {
        averageHighFives = (float) highFives / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageHighFives;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/highFives.txt");
    file << std::to_string(highFives);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameHighFives.txt");
    gameFile << std::to_string(gameHighFives);
    gameFile.close();
}
void DemolitionCounter::writeSwishs() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageSwishs.txt");
    float averageSwishs;
    if (games == 0) {
        averageSwishs = 0;
    } else {
        averageSwishs = (float) swishs / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageSwishs;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/swishs.txt");
    file << std::to_string(swishs);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./OBSCounter/gameSwishs.txt");
    gameFile << std::to_string(gameSwishs);
    gameFile.close();
}
void DemolitionCounter::writeExterms() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageExterms.txt");
    float averageExterms;
    if (games == 0) {
        averageExterms = 0;
    } else {
        averageExterms = (float) exterms / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageExterms;
    averageFile.close();

    std::ofstream extermFile;
    extermFile.open("./OBSCounter/exterminations.txt");
    extermFile << std::to_string(exterms);
    extermFile.close();

    std::ofstream gameExtermFile;
    gameExtermFile.open("./OBSCounter/gameExterminations.txt");
    gameExtermFile << std::to_string(gameExterms);
    gameExtermFile.close();
}
void DemolitionCounter::writeWins() {
    std::ofstream file;
    file.open("./OBSCounter/wins.txt");
    file << std::to_string(wins);
    file.close();
}
void DemolitionCounter::writeMvps() {
    std::ofstream averageFile;
    averageFile.open("./OBSCounter/averageMvps.txt");
    float averageMvps;
    if (games == 0) {
        averageMvps = 0;
    } else {
        averageMvps = (float) mvps / (float) games;
    }
    averageFile << std::fixed << std::setprecision(decimalPlaces);
    averageFile << averageMvps;
    averageFile.close();

    std::ofstream file;
    file.open("./OBSCounter/mvps.txt");
    file << std::to_string(mvps);
    file.close();
}
void DemolitionCounter::writeGames() {
    std::ofstream gameFile;
    gameFile.open("./OBSCounter/games.txt");
    gameFile << std::to_string(games);
    gameFile.close();
}
void DemolitionCounter::writeShootingPercentage() {
    std::ofstream gameFile;
    float gameShooting;
    if (gameShots == 0) {
        gameShooting = 0;
    }
    else {
        gameShooting = (float)gameGoals / (float)gameShots;
    }
    gameFile.open("./OBSCounter/gameShootingPercentage.txt");
    gameFile << std::fixed << std::setprecision(decimalPlaces);
    gameFile << gameShooting;
    gameFile.close();

    float totalShooting;
    if (shots == 0) {
        totalShooting = 0;
    }
    else {
        totalShooting = (float)goals / (float)shots;
    }
    std::ofstream file;
    file.open("./OBSCounter/shootingPercentage.txt");
    file << std::fixed << std::setprecision(decimalPlaces);
    file << totalShooting;
    file.close();
}

// calls all write functions at once
void DemolitionCounter::writeAll() {
    writeDemos();
    writeDeaths();
    writeExterms();
    writeGames();
    writeWins();
    writeMvps();
    writeGoals();
    writeAerialGoals();
    writeBackwardsGoals();
    writeBicycleGoals();
    writeLongGoals();
    writeTurtleGoals();
    writePoolShots();
    writeOvertimeGoals();
    writeHatTricks();
    writeAssists();
    writePlaymakers();
    writeSaves();
    writeEpicSaves();
    writeSaviors();
    writeShots();
    writeCenters();
    writeClears();
    writeFirstTouchs();
    writeDamages();
    writeUltraDamages();
    writeLowFives();
    writeHighFives();
    writeSwishs();
    writeShootingPercentage();
}

void DemolitionCounter::onUnload()
{
}