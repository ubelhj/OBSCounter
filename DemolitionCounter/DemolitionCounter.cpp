#include "pch.h"
#include "DemolitionCounter.h"
#include <iostream>
#include <fstream>
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

BAKKESMOD_PLUGIN(DemolitionCounter, "Counts demolitions in online games", plugin_version, PLUGINTYPE_THREADED)

bool enabledCounter;
bool endedGame = true;

// stores all stats
int wins = 0;
int mvps = 0;
int games = 0;
int demos = 0;
int exterms = 0;
int goals = 0;
int aerialGoals = 0;
int backwardsGoals = 0;
int bicycleGoals = 0;
int longGoals = 0;
int turtleGoals = 0;
int poolShots = 0;
int overtimeGoals = 0;
int hatTricks = 0;
int assists = 0;
int playmakers = 0;
int saves = 0;
int epicSaves = 0;
int saviors = 0;
int shots = 0;
int centers = 0;
int clears = 0;
int firstTouchs = 0;
int damages = 0;
int ultraDamages = 0;
int lowFives = 0;
int highFives = 0;
int swishs = 0;
int gameGoals = 0;
int gameDemos = 0;
int gameExterms = 0;
int gameAerialGoals = 0;
int gameBackwardsGoals = 0;
int gameBicycleGoals = 0;
int gameLongGoals = 0;
int gameTurtleGoals = 0;
int gamePoolShots = 0;
int gameOvertimeGoals = 0;
int gameHatTricks = 0;
int gameAssists = 0;
int gamePlaymakers = 0;
int gameSaves = 0;
int gameEpicSaves = 0;
int gameSaviors = 0;
int gameShots = 0;
int gameCenters = 0;
int gameClears = 0;
int gameFirstTouchs = 0;
int gameDamages = 0;
int gameUltraDamages = 0;
int gameLowFives = 0;
int gameHighFives = 0;
int gameSwishs = 0;

void DemolitionCounter::onLoad()
{
    cvarManager->log("Plugin loaded!");
    auto enabledVar = cvarManager->registerCvar("counter_enabled", "0", "activate/deactivate demolition counter");

    updateEnabled(enabledVar.getBoolValue());
    enabledVar.addOnValueChanged([this](std::string, CVarWrapper cvar) { updateEnabled(cvar.getBoolValue()); });

    // setters for totals
    setCvars();
    
    writeAll();
}

void DemolitionCounter::setCvars() {
    // special case to make sure games update properly
    auto startgames = cvarManager->registerCvar("counter_set_games", "0", "sets games value");
    games = startgames.getIntValue();
    endedGame = true;
    startgames.addOnValueChanged([this](std::string, CVarWrapper cvar) { games = cvar.getIntValue() - 1; startGame(); endedGame = true; });
    auto startDemos = cvarManager->registerCvar("counter_set_demos", "0", "sets demolition value");
    demos = startDemos.getIntValue();
    startDemos.addOnValueChanged([this](std::string, CVarWrapper cvar) { demos = cvar.getIntValue(); writeDemos(); });
    auto startExterms = cvarManager->registerCvar("counter_set_exterms", "0", "sets extermination value");
    exterms = startExterms.getIntValue();
    startExterms.addOnValueChanged([this](std::string, CVarWrapper cvar) { exterms = cvar.getIntValue(); writeExterms(); });
    auto startwins = cvarManager->registerCvar("counter_set_wins", "0", "sets wins value");
	wins = startwins.getIntValue();
	startwins.addOnValueChanged([this](std::string, CVarWrapper cvar) { wins = cvar.getIntValue(); writeWins(); });
    auto startmvps = cvarManager->registerCvar("counter_set_mvps", "0", "sets mvps value");
	mvps = startmvps.getIntValue();
	startmvps.addOnValueChanged([this](std::string, CVarWrapper cvar) { mvps = cvar.getIntValue(); writeMvps(); });
    auto startgoals = cvarManager->registerCvar("counter_set_goals", "0", "sets goals value");
	goals = startgoals.getIntValue();
	startgoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { goals = cvar.getIntValue(); writeGoals(); });
    auto startaerialGoals = cvarManager->registerCvar("counter_set_aerialGoals", "0", "sets aerialGoals value");
	aerialGoals = startaerialGoals.getIntValue();
	startaerialGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { aerialGoals = cvar.getIntValue(); writeAerialGoals(); });
    auto startbackwardsGoals = cvarManager->registerCvar("counter_set_backwardsGoals", "0", "sets backwardsGoals value");
	backwardsGoals = startbackwardsGoals.getIntValue();
	startbackwardsGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { backwardsGoals = cvar.getIntValue(); writeBackwardsGoals(); });
    auto startbicycleGoals = cvarManager->registerCvar("counter_set_bicycleGoals", "0", "sets bicycleGoals value");
	bicycleGoals = startbicycleGoals.getIntValue();
	startbicycleGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { bicycleGoals = cvar.getIntValue(); writeBicycleGoals(); });
    auto startlongGoals = cvarManager->registerCvar("counter_set_longGoals", "0", "sets longGoals value");
	longGoals = startlongGoals.getIntValue();
	startlongGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { longGoals = cvar.getIntValue(); writeLongGoals(); });
    auto startturtleGoals = cvarManager->registerCvar("counter_set_turtleGoals", "0", "sets turtleGoals value");
	turtleGoals = startturtleGoals.getIntValue();
	startturtleGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { turtleGoals = cvar.getIntValue(); writeTurtleGoals(); });
    auto startpoolShots = cvarManager->registerCvar("counter_set_poolShots", "0", "sets poolShots value");
	poolShots = startpoolShots.getIntValue();
	startpoolShots.addOnValueChanged([this](std::string, CVarWrapper cvar) { poolShots = cvar.getIntValue(); writePoolShots(); });
    auto startovertimeGoals = cvarManager->registerCvar("counter_set_overtimeGoals", "0", "sets overtimeGoals value");
	overtimeGoals = startovertimeGoals.getIntValue();
	startovertimeGoals.addOnValueChanged([this](std::string, CVarWrapper cvar) { overtimeGoals = cvar.getIntValue(); writeOvertimeGoals(); });
    auto starthatTricks = cvarManager->registerCvar("counter_set_hatTricks", "0", "sets hatTricks value");
	hatTricks = starthatTricks.getIntValue();
	starthatTricks.addOnValueChanged([this](std::string, CVarWrapper cvar) { hatTricks = cvar.getIntValue(); writeHatTricks(); });
    auto startassists = cvarManager->registerCvar("counter_set_assists", "0", "sets assists value");
	assists = startassists.getIntValue();
	startassists.addOnValueChanged([this](std::string, CVarWrapper cvar) { assists = cvar.getIntValue(); writeAssists(); });
    auto startplaymakers = cvarManager->registerCvar("counter_set_playmakers", "0", "sets playmakers value");
	playmakers = startplaymakers.getIntValue();
	startplaymakers.addOnValueChanged([this](std::string, CVarWrapper cvar) { playmakers = cvar.getIntValue(); writePlaymakers(); });
    auto startsaves = cvarManager->registerCvar("counter_set_saves", "0", "sets saves value");
	saves = startsaves.getIntValue();
	startsaves.addOnValueChanged([this](std::string, CVarWrapper cvar) { saves = cvar.getIntValue(); writeSaves(); });
    auto startepicSaves = cvarManager->registerCvar("counter_set_epicSaves", "0", "sets epicSaves value");
	epicSaves = startepicSaves.getIntValue();
	startepicSaves.addOnValueChanged([this](std::string, CVarWrapper cvar) { epicSaves = cvar.getIntValue(); writeEpicSaves(); });
    auto startsaviors = cvarManager->registerCvar("counter_set_saviors", "0", "sets saviors value");
	saviors = startsaviors.getIntValue();
	startsaviors.addOnValueChanged([this](std::string, CVarWrapper cvar) { saviors = cvar.getIntValue(); writeSaviors(); });
    auto startshots = cvarManager->registerCvar("counter_set_shots", "0", "sets shots value");
	shots = startshots.getIntValue();
	startshots.addOnValueChanged([this](std::string, CVarWrapper cvar) { shots = cvar.getIntValue(); writeShots(); });
    auto startcenters = cvarManager->registerCvar("counter_set_centers", "0", "sets centers value");
	centers = startcenters.getIntValue();
	startcenters.addOnValueChanged([this](std::string, CVarWrapper cvar) { centers = cvar.getIntValue(); writeCenters(); });
    auto startclears = cvarManager->registerCvar("counter_set_clears", "0", "sets clears value");
	clears = startclears.getIntValue();
	startclears.addOnValueChanged([this](std::string, CVarWrapper cvar) { clears = cvar.getIntValue(); writeClears(); });
    auto startfirstTouchs = cvarManager->registerCvar("counter_set_firstTouchs", "0", "sets firstTouchs value");
	firstTouchs = startfirstTouchs.getIntValue();
	startfirstTouchs.addOnValueChanged([this](std::string, CVarWrapper cvar) { firstTouchs = cvar.getIntValue(); writeFirstTouchs(); });
    auto startdamages = cvarManager->registerCvar("counter_set_damages", "0", "sets damages value");
	damages = startdamages.getIntValue();
	startdamages.addOnValueChanged([this](std::string, CVarWrapper cvar) { damages = cvar.getIntValue(); writeDamages(); });
    auto startultraDamages = cvarManager->registerCvar("counter_set_ultraDamages", "0", "sets ultraDamages value");
	ultraDamages = startultraDamages.getIntValue();
	startultraDamages.addOnValueChanged([this](std::string, CVarWrapper cvar) { ultraDamages = cvar.getIntValue(); writeUltraDamages(); });
    auto startlowFives = cvarManager->registerCvar("counter_set_lowFives", "0", "sets lowFives value");
	lowFives = startlowFives.getIntValue();
	startlowFives.addOnValueChanged([this](std::string, CVarWrapper cvar) { lowFives = cvar.getIntValue(); writeLowFives(); });
    auto starthighFives = cvarManager->registerCvar("counter_set_highFives", "0", "sets highFives value");
	highFives = starthighFives.getIntValue();
	starthighFives.addOnValueChanged([this](std::string, CVarWrapper cvar) { highFives = cvar.getIntValue(); writeHighFives(); });
    auto startswishs = cvarManager->registerCvar("counter_set_swishs", "0", "sets swishs value");
	swishs = startswishs.getIntValue();
	startswishs.addOnValueChanged([this](std::string, CVarWrapper cvar) { swishs = cvar.getIntValue(); writeSwishs(); });
}

void DemolitionCounter::updateEnabled(bool enabled) {
    enabledCounter = enabled;

    if (enabled) {
        gameWrapper->HookEventWithCaller<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage", 
            std::bind(&DemolitionCounter::statEvent, this, std::placeholders::_1, std::placeholders::_2));

        // works on a starting game 
        gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.WaitingForPlayers.BeginState", std::bind(&DemolitionCounter::startGame, this));
        // works on a joined game in progress
        gameWrapper->HookEventPost("Function Engine.PlayerInput.InitInputSystem", std::bind(&DemolitionCounter::startGame, this));
        // works on ended game
        gameWrapper->HookEventPost("Function TAGame.GFxHUD_Soccar_TA.HandleMatchWinnerSet", std::bind(&DemolitionCounter::endGame, this));
    }
    else {
        gameWrapper->UnhookEvent("Function TAGame.GFxHUD_TA.HandleStatTickerMessage");
        gameWrapper->UnhookEvent("Function GameEvent_Soccar_TA.WaitingForPlayers.BeginState");
        gameWrapper->UnhookEvent("Function Engine.PlayerInput.InitInputSystem");
    }
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

    if (!DemolitionCounter::isPrimaryPlayer(receiver)) {
        return;
    }

    // Checks for each possible event name
    // no switches in c++ for strings
    if (label.ToString().compare("Demolition") == 0) {
        DemolitionCounter::demolition();
        return;
    }
    if (label.ToString().compare("Extermination") == 0) {
        DemolitionCounter::extermination();
        return;
    }
    if (label.ToString().compare("Win") == 0) {
        DemolitionCounter::win();
        return;
    }
    if (label.ToString().compare("Goal") == 0) {
        DemolitionCounter::goal();
        return;
    }
    if (label.ToString().compare("MVP") == 0) {
        DemolitionCounter::mvp();
        return;
    }
    if (label.ToString().compare("Aerial Goal") == 0) {
        DemolitionCounter::aerialGoal();
        return;
    }
    if (label.ToString().compare("Backwards Goal") == 0) {
        DemolitionCounter::backwardsGoal();
        return;
    }
    if (label.ToString().compare("Bicycle Goal") == 0) {
    DemolitionCounter::bicycleGoal();
    return;
    }
    if (label.ToString().compare("Long Goal") == 0) {
        DemolitionCounter::longGoal();
        return;
    }
    if (label.ToString().compare("Turtle Goal") == 0) {
        DemolitionCounter::turtleGoal();
        return;
    }
    if (label.ToString().compare("Pool Shot") == 0) {
        DemolitionCounter::poolShot();
        return;
    }
    if (label.ToString().compare("Overtime Goal") == 0) {
        DemolitionCounter::overtimeGoal();
        return;
    }
    if (label.ToString().compare("Hat Trick") == 0) {
        DemolitionCounter::hatTrick();
        return;
    }
    if (label.ToString().compare("Assist") == 0) {
        DemolitionCounter::assist();
        return;
    }
    if (label.ToString().compare("Playmaker") == 0) {
        DemolitionCounter::playmaker();
        return;
    }
    if (label.ToString().compare("Save") == 0) {
        DemolitionCounter::save();
        return;
    }
    if (label.ToString().compare("Epic Save") == 0) {
        DemolitionCounter::epicSave();
        return;
    }
    if (label.ToString().compare("Savior") == 0) {
        DemolitionCounter::savior();
        return;
    }
    if (label.ToString().compare("Shot") == 0) {
        DemolitionCounter::shot();
        return;
    }
    if (label.ToString().compare("Center") == 0) {
        DemolitionCounter::center();
        return;
    }
    if (label.ToString().compare("Clear") == 0) {
        DemolitionCounter::clear();
        return;
    }
    if (label.ToString().compare("First Touch") == 0) {
        DemolitionCounter::firstTouch();
        return;
    }
    if (label.ToString().compare("Damage") == 0) {
        DemolitionCounter::damage();
        return;
    }
    if (label.ToString().compare("Ultra Damage") == 0) {
        DemolitionCounter::ultraDamage();
        return;
    }
    if (label.ToString().compare("Low Five") == 0) {
        DemolitionCounter::lowFive();
        return;
    }
    if (label.ToString().compare("High Five") == 0) {
        DemolitionCounter::highFive();
        return;
    }
    if (label.ToString().compare("Swish") == 0) {
        DemolitionCounter::swish();
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

    games++;
    gameExterms = 0;
    gameDemos = 0;
    gameGoals = 0;
    gameAerialGoals = 0;
    gameBackwardsGoals = 0;
    gameBicycleGoals = 0;
    gameLongGoals = 0;
    gameTurtleGoals = 0;
    gamePoolShots = 0;
    gameOvertimeGoals = 0;
    gameHatTricks = 0;
    gameAssists = 0;
    gamePlaymakers = 0;
    gameSaves = 0;
    gameEpicSaves = 0;
    gameSaviors = 0;
    gameShots = 0;
    gameCenters = 0;
    gameClears = 0;
    gameFirstTouchs = 0;
    gameDamages = 0;
    gameUltraDamages = 0;
    gameLowFives = 0;
    gameHighFives = 0;
    gameSwishs = 0;

    writeAll();
}

void DemolitionCounter::endGame() {
    endedGame = true;
}

// saving to variables as a stat happens and then call write functions
void DemolitionCounter::demolition() {
    //cvarManager->log("main player demo");
    demos++;
    gameDemos++;
    //cvarManager->log(std::to_string(demos));

    DemolitionCounter::writeDemos();
}
void DemolitionCounter::extermination() {
    //cvarManager->log("main player exterm");
    exterms++;
    gameExterms++;
    //cvarManager->log(std::to_string(exterms));

    DemolitionCounter::writeExterms();
}
void DemolitionCounter::win() {
    wins++;
    DemolitionCounter::writeWins();
}
void DemolitionCounter::mvp() {
    mvps++;
    DemolitionCounter::writeMvps();
}
void DemolitionCounter::goal() {
    goals++;
    gameGoals++;
    DemolitionCounter::writeGoals();
}
void DemolitionCounter::aerialGoal() {
    aerialGoals++;
    gameAerialGoals++;
    DemolitionCounter::writeAerialGoals();
}
void DemolitionCounter::backwardsGoal() {
    backwardsGoals++;
    gameBackwardsGoals++;
    DemolitionCounter::writeBackwardsGoals();
}
void DemolitionCounter::bicycleGoal() {
    bicycleGoals++;
    gameBicycleGoals++;
    DemolitionCounter::writeBicycleGoals();
}
void DemolitionCounter::longGoal() {
    longGoals++;
    gameLongGoals++;
    DemolitionCounter::writeLongGoals();
}
void DemolitionCounter::turtleGoal() {
    turtleGoals++;
    gameTurtleGoals++;
    DemolitionCounter::writeTurtleGoals();
}
void DemolitionCounter::poolShot() {
    poolShots++;
    gamePoolShots++;
    DemolitionCounter::writePoolShots();
}
void DemolitionCounter::overtimeGoal() {
    overtimeGoals++;
    gameOvertimeGoals++;
    DemolitionCounter::writeOvertimeGoals();
}
void DemolitionCounter::hatTrick() {
    hatTricks++;
    gameHatTricks++;
    DemolitionCounter::writeHatTricks();
}
void DemolitionCounter::assist() {
    assists++;
    gameAssists++;
    DemolitionCounter::writeAssists();
}
void DemolitionCounter::playmaker() {
    playmakers++;
    gamePlaymakers++;
    DemolitionCounter::writePlaymakers();
}
void DemolitionCounter::save() {
    saves++;
    gameSaves++;
    DemolitionCounter::writeSaves();
}
void DemolitionCounter::epicSave() {
    epicSaves++;
    gameEpicSaves++;
    DemolitionCounter::writeEpicSaves();
}
void DemolitionCounter::savior() {
    saviors++;
    gameSaviors++;
    DemolitionCounter::writeSaviors();
}
void DemolitionCounter::shot() {
    shots++;
    gameShots++;
    DemolitionCounter::writeShots();
}
void DemolitionCounter::center() {
    centers++;
    gameCenters++;
    DemolitionCounter::writeCenters();
}
void DemolitionCounter::clear() {
    clears++;
    gameClears++;
    DemolitionCounter::writeClears();
}
void DemolitionCounter::firstTouch() {
    firstTouchs++;
    gameFirstTouchs++;
    DemolitionCounter::writeFirstTouchs();
}
void DemolitionCounter::damage() {
    damages++;
    gameDamages++;
    DemolitionCounter::writeDamages();
}
void DemolitionCounter::ultraDamage() {
    ultraDamages++;
    gameUltraDamages++;
    DemolitionCounter::writeUltraDamages();
}
void DemolitionCounter::lowFive() {
    lowFives++;
    gameLowFives++;
    DemolitionCounter::writeLowFives();
}
void DemolitionCounter::highFive() {
    highFives++;
    gameHighFives++;
    DemolitionCounter::writeHighFives();
}
void DemolitionCounter::swish() {
    swishs++;
    gameSwishs++;
    DemolitionCounter::writeSwishs();
}

// writing stats to to files
void DemolitionCounter::writeDemos() {
    std::ofstream file;
    file.open("./DemolitionCounter/demolitions.txt");
    file << std::to_string(demos);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameDemolitions.txt");
    gameFile << std::to_string(gameDemos);
    gameFile.close();
}
void DemolitionCounter::writeGoals() {
    std::ofstream file;
    file.open("./DemolitionCounter/goals.txt");
    file << std::to_string(goals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameGoals.txt");
    gameFile << std::to_string(gameGoals);
    gameFile.close();
}
void DemolitionCounter::writeAerialGoals() {
    std::ofstream file;
    file.open("./DemolitionCounter/aerialGoals.txt");
    file << std::to_string(aerialGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameAerialGoals.txt");
    gameFile << std::to_string(gameAerialGoals);
    gameFile.close();
}
void DemolitionCounter::writeBackwardsGoals() {
    std::ofstream file;
    file.open("./DemolitionCounter/backwardsGoals.txt");
    file << std::to_string(backwardsGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameBackwardsGoals.txt");
    gameFile << std::to_string(gameBackwardsGoals);
    gameFile.close();
}
void DemolitionCounter::writeBicycleGoals() {
    std::ofstream file;
    file.open("./DemolitionCounter/bicycleGoals.txt");
    file << std::to_string(bicycleGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameBicycleGoals.txt");
    gameFile << std::to_string(gameBicycleGoals);
    gameFile.close();
}
void DemolitionCounter::writeLongGoals() {
    std::ofstream file;
    file.open("./DemolitionCounter/longGoals.txt");
    file << std::to_string(longGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameLongGoals.txt");
    gameFile << std::to_string(gameLongGoals);
    gameFile.close();
}
void DemolitionCounter::writeTurtleGoals() {
    std::ofstream file;
    file.open("./DemolitionCounter/turtleGoals.txt");
    file << std::to_string(turtleGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameTurtleGoals.txt");
    gameFile << std::to_string(gameTurtleGoals);
    gameFile.close();
}
void DemolitionCounter::writePoolShots() {
    std::ofstream file;
    file.open("./DemolitionCounter/poolShots.txt");
    file << std::to_string(poolShots);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gamePoolShots.txt");
    gameFile << std::to_string(gamePoolShots);
    gameFile.close();
}
void DemolitionCounter::writeOvertimeGoals() {
    std::ofstream file;
    file.open("./DemolitionCounter/overtimeGoals.txt");
    file << std::to_string(overtimeGoals);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameOvertimeGoals.txt");
    gameFile << std::to_string(gameOvertimeGoals);
    gameFile.close();
}
void DemolitionCounter::writeHatTricks() {
    std::ofstream file;
    file.open("./DemolitionCounter/hatTricks.txt");
    file << std::to_string(hatTricks);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameHatTricks.txt");
    gameFile << std::to_string(gameHatTricks);
    gameFile.close();
}
void DemolitionCounter::writeAssists() {
    std::ofstream file;
    file.open("./DemolitionCounter/assists.txt");
    file << std::to_string(assists);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameAssists.txt");
    gameFile << std::to_string(gameAssists);
    gameFile.close();
}
void DemolitionCounter::writePlaymakers() {
    std::ofstream file;
    file.open("./DemolitionCounter/playmakers.txt");
    file << std::to_string(playmakers);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gamePlaymakers.txt");
    gameFile << std::to_string(gamePlaymakers);
    gameFile.close();
}
void DemolitionCounter::writeSaves() {
    std::ofstream file;
    file.open("./DemolitionCounter/saves.txt");
    file << std::to_string(saves);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameSaves.txt");
    gameFile << std::to_string(gameSaves);
    gameFile.close();
}
void DemolitionCounter::writeEpicSaves() {
    std::ofstream file;
    file.open("./DemolitionCounter/epicSaves.txt");
    file << std::to_string(epicSaves);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameEpicSaves.txt");
    gameFile << std::to_string(gameEpicSaves);
    gameFile.close();
}
void DemolitionCounter::writeSaviors() {
    std::ofstream file;
    file.open("./DemolitionCounter/saviors.txt");
    file << std::to_string(saviors);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameSaviors.txt");
    gameFile << std::to_string(gameSaviors);
    gameFile.close();
}
void DemolitionCounter::writeShots() {
    std::ofstream file;
    file.open("./DemolitionCounter/shots.txt");
    file << std::to_string(shots);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameShots.txt");
    gameFile << std::to_string(gameShots);
    gameFile.close();
}
void DemolitionCounter::writeCenters() {
    std::ofstream file;
    file.open("./DemolitionCounter/centers.txt");
    file << std::to_string(centers);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameCenters.txt");
    gameFile << std::to_string(gameCenters);
    gameFile.close();
}
void DemolitionCounter::writeClears() {
    std::ofstream file;
    file.open("./DemolitionCounter/clears.txt");
    file << std::to_string(clears);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameClears.txt");
    gameFile << std::to_string(gameClears);
    gameFile.close();
}
void DemolitionCounter::writeFirstTouchs() {
    std::ofstream file;
    file.open("./DemolitionCounter/firstTouchs.txt");
    file << std::to_string(firstTouchs);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameFirstTouchs.txt");
    gameFile << std::to_string(gameFirstTouchs);
    gameFile.close();
}
void DemolitionCounter::writeDamages() {
    std::ofstream file;
    file.open("./DemolitionCounter/damages.txt");
    file << std::to_string(damages);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameDamages.txt");
    gameFile << std::to_string(gameDamages);
    gameFile.close();
}
void DemolitionCounter::writeUltraDamages() {
    std::ofstream file;
    file.open("./DemolitionCounter/ultraDamages.txt");
    file << std::to_string(ultraDamages);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameUltraDamages.txt");
    gameFile << std::to_string(gameUltraDamages);
    gameFile.close();
}
void DemolitionCounter::writeLowFives() {
    std::ofstream file;
    file.open("./DemolitionCounter/lowFives.txt");
    file << std::to_string(lowFives);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameLowFives.txt");
    gameFile << std::to_string(gameLowFives);
    gameFile.close();
}
void DemolitionCounter::writeHighFives() {
    std::ofstream file;
    file.open("./DemolitionCounter/highFives.txt");
    file << std::to_string(highFives);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameHighFives.txt");
    gameFile << std::to_string(gameHighFives);
    gameFile.close();
}
void DemolitionCounter::writeSwishs() {
    std::ofstream file;
    file.open("./DemolitionCounter/swishs.txt");
    file << std::to_string(swishs);
    file.close();

    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/gameSwishs.txt");
    gameFile << std::to_string(gameSwishs);
    gameFile.close();
}
void DemolitionCounter::writeExterms() {
    std::ofstream extermFile;
    extermFile.open("./DemolitionCounter/exterminations.txt");
    extermFile << std::to_string(exterms);
    extermFile.close();

    std::ofstream gameExtermFile;
    gameExtermFile.open("./DemolitionCounter/gameExterminations.txt");
    gameExtermFile << std::to_string(gameExterms);
    gameExtermFile.close();
}
void DemolitionCounter::writeWins() {
    std::ofstream file;
    file.open("./DemolitionCounter/wins.txt");
    file << std::to_string(wins);
    file.close();
}
void DemolitionCounter::writeMvps() {
    std::ofstream file;
    file.open("./DemolitionCounter/mvps.txt");
    file << std::to_string(mvps);
    file.close();
}
void DemolitionCounter::writeGames() {
    std::ofstream gameFile;
    gameFile.open("./DemolitionCounter/games.txt");
    gameFile << std::to_string(games);
    gameFile.close();
}

// calls all write functions at once
void DemolitionCounter::writeAll() {
    writeDemos();
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
}

void DemolitionCounter::onUnload()
{
}