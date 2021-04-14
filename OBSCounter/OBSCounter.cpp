#include "pch.h"
#include "OBSCounter.h"
#include "Maps.h"


BAKKESMOD_PLUGIN(OBSCounter, "OBSCounter", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

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
bool enabledOverlayBackground;
const int maxOverlayLines = 5;
int overlayLines;
int overlayStats[maxOverlayLines];
bool overlayAverages[maxOverlayLines];
std::string overlayStrings[maxOverlayLines];
float xLocation;
float yLocation;
float scale;
LinearColor overlayColor;
LinearColor overlayBackgroundColor;

std::filesystem::path fileLocation;

// holds all stats
int statArray[numStats];

// holds all averages
// caching improves performance significantly
float averages[startGameStats];

// called when the plugin loads and prepares all cvars and default values
void OBSCounter::onLoad()
{
    fileLocation = gameWrapper->GetDataFolder() / "OBSCounter";
    // tells the plugin to render the in game overlay
    gameWrapper->RegisterDrawable(
        std::bind(&OBSCounter::render, this, std::placeholders::_1));

    // creates cvars and sets global variable defaults to prevent any nulls
    setCvars();

    // hooks the events for the plugin to work
    hookEvents();

    namespace fs = std::filesystem;
    fs::create_directories(fileLocation);
    //cvarManager->log(gameWrapper->GetDataFolder().generic_string() + fileLocation);

    writeAll();
    renderAllStrings();
}

// creates cvars and sets global variable defaults to prevent any nulls
void OBSCounter::setCvars() {
    auto decimalsVar = cvarManager->registerCvar("counter_decimals", "1",
        "set decimal places in averages (1 - 10)", true, true, 1, true, 10);
    decimalPlaces = decimalsVar.getIntValue();
    decimalsVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        decimalPlaces = cvar.getIntValue();
        writeAll();
        renderAllStrings();
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
        true, true, 1, true, maxOverlayLines);
    overlayLines = overlayNumberVar.getIntValue();
    overlayNumberVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayLines = cvar.getIntValue();
        renderAllStrings();
        });

    std::string numberStrings[] = {
        "one",
        "two",
        "three",
        "four",
        "five"
    };

    for (int i = 0; i < maxOverlayLines; i++) {
        std::string str = numberStrings[i];
        // sets stat in overlay
        auto overlayVar = cvarManager->registerCvar("counter_ingame_stat_" + str,
            std::to_string(i), "stat " + str + " in overlay", true, true, 0, true, numStats - 1);
        overlayStats[i] = overlayVar.getIntValue();
        overlayVar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            overlayStats[i] = cvar.getIntValue();
            renderAllStrings();
            });

        // sets stat in overlay to average or not
        auto overlayAvgVar = cvarManager->registerCvar(
            "counter_ingame_stat_" + str + "_average", "0",
            "Toggles average of stat " + str + " in overlay", true, true, 0, true, 1);
        overlayAverages[i] = overlayAvgVar.getBoolValue();
        overlayAvgVar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            overlayAverages[i] = cvar.getBoolValue();
            renderAllStrings();
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

    // sets cvar for counter's scale
    auto scaleVar = cvarManager->registerCvar("counter_ingame_scale",
        "2", "set counter scale",
        true, true, 0.0, true, 10.0);
    scale = scaleVar.getFloatValue();
    scaleVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        scale = cvar.getFloatValue();
        });

    auto colorVar = cvarManager->registerCvar("counter_color", "#FFFFFF", "color of overlay");
    overlayColor = colorVar.getColorValue();
    colorVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayColor = cvar.getColorValue();
        });

    auto overlayBackgroundEnableVar = cvarManager->registerCvar("counter_enable_background",
        "0", "enables in game overlay background");
    enabledOverlayBackground = overlayBackgroundEnableVar.getBoolValue();
    overlayBackgroundEnableVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        enabledOverlayBackground = cvar.getBoolValue();
        });

    auto backColorVar = cvarManager->registerCvar("counter_color_background", "#0000008C", "color of overlay background");
    overlayBackgroundColor = backColorVar.getColorValue();
    backColorVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayBackgroundColor = cvar.getColorValue();
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
    for (int i = 0; i <= endNormalStats; i++) {
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

    // creates setters for the default start value for each stat
    // also writes these new values to files
    for (int i = endNormalStats + 1; i < startGameStats; i++) {
        std::string cvarName = "counter_set_" + indexStringMap[i];
        std::string cvarTip = "sets " + indexStringMap[i] + " value";
        cvarManager->registerCvar(cvarName, "0", cvarTip, true, false, 0, false, 0, false);
        auto cvar = cvarManager->getCvar(cvarName);
        statArray[i] = cvar.getIntValue();

        cvar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            statArray[i] = cvar.getIntValue(); writeTimeStat(i); });
    }

    // setters for render strings for stats
    for (int i = 0; i < numStats; i++) {
        std::string cvarName = "counter_set_render_string_" + indexStringMap[i];
        std::string cvarTip = "sets " + indexStringMap[i] + " render string";
        cvarManager->registerCvar(cvarName, indexStringMapRender[i], cvarTip);
        auto cvar = cvarManager->getCvar(cvarName);

        cvar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            indexStringMapRender[i] = cvar.getStringValue(); renderAllStrings(); });
    }

    // setters for render strings for average stats
    for (int i = 0; i < startGameStats; i++) {
        std::string cvarName = "counter_set_render_string_" + averageStrings[i];
        std::string cvarTip = "sets " + averageStrings[i] + " render string";
        cvarManager->registerCvar(cvarName, averageStringsRender[i], cvarTip);
        auto cvar = cvarManager->getCvar(cvarName);
        statArray[i] = cvar.getIntValue();

        cvar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            averageStringsRender[i] = cvar.getStringValue(); renderAllStrings(); });
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
void OBSCounter::hookEvents() {
    // hooked whenever a stat appears in the top right corner of rocket league
    //  (even if stat display is turned off)
    gameWrapper->HookEventWithCallerPost<ServerWrapper>(
        "Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
        std::bind(&OBSCounter::statTickerEvent, this,
            std::placeholders::_1, std::placeholders::_2));

    // hooked whenever the primary player earns a stat
    gameWrapper->HookEventWithCallerPost<ServerWrapper>(
        "Function TAGame.GFxHUD_TA.HandleStatEvent",
        std::bind(&OBSCounter::statEvent, this,
            std::placeholders::_1, std::placeholders::_2));

    // hooks on a starting game 
    gameWrapper->HookEventPost(
        "Function GameEvent_Soccar_TA.WaitingForPlayers.BeginState",
        std::bind(&OBSCounter::startGame, this));
    // hooks on a joined game in progress
    gameWrapper->HookEventPost("Function Engine.PlayerInput.InitInputSystem",
        std::bind(&OBSCounter::startGame, this));
    // works on ended game
    gameWrapper->HookEventPost(
        "Function TAGame.GameEvent_Soccar_TA.EventMatchEnded",
        std::bind(&OBSCounter::endGame, this));
    // works on time tick
    gameWrapper->HookEventPost(
        "Function TAGame.GameEvent_Soccar_TA.OnGameTimeUpdated",
        [this](auto) { checkCarLocation(); });
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

void OBSCounter::statEvent(ServerWrapper caller, void* args) {
    cvarManager->log("stat event!");
    if (!gameWrapper->IsInOnlineGame()) {
        cvarManager->log("not in online game");
        return;
    }
    auto tArgs = (StatEventStruct*)args;

    auto statEvent = StatEventWrapper(tArgs->StatEvent);
    std::string eventString = statEvent.GetEventName();

    // PRI is always main player for this event
    /*auto pri = PriWrapper(tArgs->PRI);
    cvarManager->log("is primary: " + std::to_string(pri.IsLocalPlayerPRI()));*/

    // Count always is int_max. No idea why
    /*auto count = tArgs->Count;
    cvarManager->log("count: " + std::to_string(count));*/

    auto eventTypePtr = eventDictionary.find(eventString);

    int eventType;

    if (eventTypePtr != eventDictionary.end()) {
        eventType = eventTypePtr->second;
        cvarManager->log("event type: " + eventString);
        cvarManager->log("event num: " + std::to_string(eventType));
    }
    else {
        cvarManager->log("missing stat: " + eventString);
        cvarManager->log("missing stat points: " + statEvent.GetPoints());
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

void OBSCounter::statTickerEvent(ServerWrapper caller, void* args) {
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
    auto eventString = statEvent.GetEventName();

    auto eventTypePtr = eventDictionary.find(eventString);

    int eventType;

    if (eventTypePtr != eventDictionary.end()) {
        eventType = eventTypePtr->second;
        cvarManager->log("event type: " + eventString);
        cvarManager->log("event num: " + std::to_string(eventType));
    }
    else {
        cvarManager->log("missing stat: " + eventString);
        cvarManager->log("missing stat points: " + statEvent.GetPoints());
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
bool OBSCounter::isPrimaryPlayer(PriWrapper receiver) {
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

    auto receiverID = receiver.GetUniqueIdWrapper();
    auto primaryID = primaryPri.GetUniqueIdWrapper();

    return receiverID.GetUID() == primaryID.GetUID();
}

// called whenever a new game begins, and resets all game stats to 0
void OBSCounter::startGame() {
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

void OBSCounter::endGame() {
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

// called every second to log car location on offense or defense
void OBSCounter::checkCarLocation() {
    cvarManager->log("updated time");
    if (!gameWrapper->IsInOnlineGame()) {
        cvarManager->log("not in online game");
        return;
    }

    ServerWrapper sw = gameWrapper->GetOnlineGame();

    if (sw.IsNull()) {
        cvarManager->log("null server");
        return;
    }

    auto car = gameWrapper->GetLocalCar();

    if (car.IsNull()) {
        cvarManager->log("null car");
        return;
    }

    auto loc = car.GetLocation();

    auto team = car.GetTeamNum2();
    // offense for team 0 is y > 0
    // offense for team 1 is y < 0

    // inverts team 1 so both have the same offense / defense
    if (team) {
        loc = loc * -1;
    }

    if (loc.Y > 0) {
        cvarManager->log("offense time");
        statArray[offenseTime]++;
        statArray[gameOffenseTime]++;
        writeTimeStat(offenseTime);
    }
    else {
        cvarManager->log("defense time");
        statArray[defenseTime]++;
        statArray[gameDefenseTime]++;
        writeTimeStat(defenseTime);
    }
    statArray[timePlayed]++;
    statArray[gameTime]++;
    writeTimeStat(timePlayed);
}

// writes a stat to its files
// can only be called with a stat index and not a game stat index
void OBSCounter::write(int statIndex) {
    // writes the total stat
    std::ofstream totalFile(fileLocation / (indexStringMap[statIndex] + ".txt"));
    //cvarManager->log("wrote");
    totalFile << statArray[statIndex];
    totalFile.close();

    // doesn't write averageGames, 
    //  as that would just be games/games and always 1
    if (statIndex != games) {
        // writes average of stat per game
        // sets up average file location
        std::ofstream averageFile(fileLocation / (averageStrings[statIndex] + ".txt"));

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

    renderString(statIndex);

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
void OBSCounter::writeGameStat(int statIndex) {
    std::ofstream gameStatFile(fileLocation / (indexStringMap[statIndex] + ".txt"));
    gameStatFile << statArray[statIndex];
    gameStatFile.close();

    renderString(statIndex);
}

// writes a time stat with special formatting minutes:seconds
void OBSCounter::writeTimeStat(int statIndex) {
    int totalSeconds = statArray[statIndex];
    // writes the total stat
    std::ofstream totalFile(fileLocation / (indexStringMap[statIndex] + ".txt"));
    totalFile << totalSeconds / 60;
    totalFile << ":";
    int remSeconds = totalSeconds % 60;
    if (remSeconds < 10) {
        totalFile << "0";
    }
    totalFile << remSeconds;
    totalFile.close();

    // writes average of stat per game
    std::ofstream averageFile(fileLocation / (averageStrings[statIndex] + ".txt"));

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
    if (statIndex > statsWithoutGame) {
        writeGameTimeStat(statIndex + totalToGame);
    }

    renderString(statIndex);
}

// writes only the game time stat
void OBSCounter::writeGameTimeStat(int statIndex) {
    int totalSeconds = statArray[statIndex];
    // writes the stat
    std::ofstream file(fileLocation / (indexStringMap[statIndex] + ".txt"));
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
    for (int i = 0; i <= endNormalStats; i++) {
        write(i);
    }

    for (int i = endNormalStats + 1; i < startGameStats; i++) {
        writeTimeStat(i);
    }
}

// special cases for extra complicated stats
// calculates shooting percentage (shots/goals)
void OBSCounter::writeShootingPercentage() {
    // calculates current game shooting %
    // divides and checks for NaN
    int gameShooting = getPercentage(statArray[gameGoals], statArray[gameShots]);
    std::ofstream gameFile(fileLocation / "gameShootingPercentage.txt");
    gameFile << gameShooting << "%";
    gameFile.close();

    // writes total session shooting
    int totalShooting = getPercentage(statArray[goals], statArray[shots]);
    std::ofstream file(fileLocation / "shootingPercentage.txt");
    file << totalShooting << "%";
    file.close();
}

// writes K/D ratio
void OBSCounter::writeKillPercentage() {
    float gameKD = divide(gameDemos, gameDeaths);
    std::ofstream gameFile(fileLocation / "gameKDRatio.txt");
    gameFile << std::fixed << std::setprecision(decimalPlaces);
    gameFile << gameKD;
    gameFile.close();

    float totalKDRatio = divide(demos, deaths);
    std::ofstream file(fileLocation / "KDRatio.txt");
    file << std::fixed << std::setprecision(decimalPlaces);
    file << totalKDRatio;
    file.close();
}

// writes missed exterm % for the session
// total exterms / possible exterms (demos / 7) 
void OBSCounter::writeMissedExterms() {
    // calculates possible exterms (demos / 7) 
    int possibleExterms = statArray[demos] / 7;
    std::ofstream totalFile(fileLocation / "possibleExterminations.txt");
    totalFile << possibleExterms;
    totalFile.close();

    int missedExtermPercent = getPercentage(statArray[exterms], possibleExterms);
    std::ofstream file(fileLocation / "missedExterminationPercent.txt");
    file << missedExtermPercent << "%";
    file.close();
}

void OBSCounter::writeWinPercentage() {
    int winPercent = getPercentage(statArray[wins], statArray[wins] + statArray[losses]);
    std::ofstream file(fileLocation / "winPercent.txt");
    file << winPercent << "%";
    file.close();
}

int OBSCounter::getPercentage(int numerator, int denominator) {
    if (denominator == 0) {
        return 0;
    }
    else {
        return ((float)numerator / (float)denominator) * 100;
    }
}

// Renders in game overlay
void OBSCounter::render(CanvasWrapper canvas) {
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
    float fontSize = //((float)screen.X / (float)1920) * 2;
        scale;
    int xValue = int((float)screen.X * xLocation);
    int yValue = int((float)screen.Y * yLocation);

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
        canvas.SetPosition(Vector2({ xValue, yValue }));
        canvas.FillBox(Vector2({ int(maxStringSize.X), int(maxStringSize.Y * overlayLines) }));
    }

    // sets to user-chosen color
    //canvas.SetColor(overlayColors[0], overlayColors[1], overlayColors[2], 255);
    canvas.SetColor(overlayColor);

    for (int i = 0; i < overlayLines; i++) {
        // locates based on screen and font size
        canvas.SetPosition(Vector2({ xValue, int((maxStringSize.Y * i) + yValue) }));
        // does averages if the user wants them and if a stat has an average
        //std::string renderString = statToRenderString(overlayStats[i], overlayAverages[i]);
        //int width = renderString.length() * fontSize * 10;
        canvas.DrawString(overlayStrings[i], fontSize, fontSize, false);
    }

    //cvarManager->log(std::to_string(fontSize));
}

std::string OBSCounter::statToRenderString(int index, bool isAverage) {
    // writes averages of stats
    if (isAverage && index < startGameStats) {
        std::ostringstream averageStream;

        if (index > endNormalStats) {
            int totalSeconds = averages[index];
            // writes the stat
            averageStream << totalSeconds / 60;
            averageStream << ":";
            int remSeconds = totalSeconds % 60;
            if (remSeconds < 10) {
                averageStream << "0";
            }
            averageStream << remSeconds;
        }
        else {
            averageStream << std::fixed << std::setprecision(decimalPlaces);
            averageStream << averages[index];
        }
        return averageStringsRender[index] + averageStream.str();
        // writes non-averages
    }
    else {
        std::ostringstream strStream;

        // writes time stats
        if ((index > endNormalStats && index < startGameStats) || index > endNormalGameStats) {
            int totalSeconds = statArray[index];
            // writes the stat
            strStream << totalSeconds / 60;
            strStream << ":";
            int remSeconds = totalSeconds % 60;
            if (remSeconds < 10) {
                strStream << "0";
            }
            strStream << remSeconds;
        }
        else {
            strStream << statArray[index];
        }
        return indexStringMapRender[index] + strStream.str();
    }
}

void OBSCounter::renderAllStrings() {
    for (int i = 0; i < overlayLines; i++) {
        overlayStrings[i] = statToRenderString(overlayStats[i], overlayAverages[i]);
    }
}

void OBSCounter::renderString(int statIndex) {
    for (int i = 0; i < overlayLines; i++) {
        if (overlayStats[i] == statIndex) {
            overlayStrings[i] = statToRenderString(statIndex, overlayAverages[i]);
        }
    }
}

// Lists all stats to console
void OBSCounter::listStats() {
    for (int i = 0; i < numStats; i++) {
        cvarManager->log(std::to_string(i) + ": " + indexStringMap[i]);
    }
}

// calculates average of stat and avoids NaN
float OBSCounter::average(int statIndex) {
    averages[statIndex] = divide(statIndex, games);
    return averages[statIndex];
}

// calculates division of two stats and avoids NaN
// prevents a bunch of duplicated code
float OBSCounter::divide(int firstStatIndex, int secondStatIndex) {
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
void OBSCounter::onUnload()
{
}