#include "pch.h"
#include "OBSCounter.h"

BAKKESMOD_PLUGIN(OBSCounter, "OBSCounter", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

// called when the plugin loads and prepares all cvars and default values
void OBSCounter::onLoad()
{
    _globalCvarManager = cvarManager;

    fileLocation = gameWrapper->GetDataFolder() / "OBSCounter";

    Maps::generateStatStrings();

    // creates cvars and sets global variable defaults to prevent any nulls
    setCvars();

    // hooks the events for the plugin to work
    hookEvents();

    std::filesystem::create_directories(fileLocation);
    std::filesystem::create_directories(fileLocation / "Career");
    std::filesystem::create_directories(fileLocation / "Team");

    writeAll();
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
    // hooks opening main menu
    gameWrapper->HookEventPost(
        "Function TAGame.GFxData_MainMenu_TA.MainMenuAdded",
        [this](...) { writeCareerStats(); });
}

int OBSCounter::getPercentage(int numerator, int denominator) {
    if (denominator == 0) {
        return 0;
    } else {
        return ((float)numerator / (float)denominator) * 100;
    }
}

// calculates average of stat and avoids NaN
float OBSCounter::average(int statIndex) {
    averages[statIndex] = divide(statArray[statIndex], statArray[games]);
    return averages[statIndex];
}

// calculates division of two stats and avoids NaN
// prevents a bunch of duplicated code
float OBSCounter::divide(int firstStat, int secondStat) {
    // if second number is 0 returns 0
    if (secondStat == 0) {
        return 0.0;
    }
    // otherwise divides ints as a float
    else {
        return (float)firstStat /
            (float)secondStat;
    }
}

// plugin is unloaded by the plugin manager, and as no state needs to be saved,
// allows it to be removed
void OBSCounter::onUnload()
{
}