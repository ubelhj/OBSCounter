#pragma once


#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"
#include "Maps.h"

#include "version.h"
#include <sstream>
#include <fstream>
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class OBSCounter: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow, public BakkesMod::Plugin::PluginWindow
{
    //Boilerplate
    virtual void onLoad();
    virtual void onUnload();

    // sets cvars to modify counters 
    void setCvars();

    // hooks events for stats
    void hookEvents();

    // fires when a stat happens
    void statEvent(ServerWrapper caller, void* args);

    // fires when the stat ticker is updated
    void statTickerEvent(ServerWrapper caller, void* args);

    // checks if the plyer that received a stat is the main player
    bool isPrimaryPlayer(PriWrapper receiver);

    // called when a new game starts, resets game stats
    void startGame();

    // called at the end of a game
    void endGame();

    // called each second to write to player location stats
    void checkCarLocation();

    // writes the .txt files

    // writes all at once
    void writeAll();
    // writes a specific stat
    void write(int statIndex);
    // writes a specific type of stat
    void writeSpecific(int statIndex, int statType);
    // writes time stats
    void writeTimeStat(int statIndex);
    // writes a game stat only but formats for time
    void writeGameTimeStat(int statIndex);
    // writes career stats
    void writeCareerStats();
    // helper to above 
    void writeCareerStatsWrapped();
    // writes to a file
    void writeFile(std::filesystem::path path, int value);
    // writes an average to a file
    void writeFileAverage(std::filesystem::path path, float value);
    // calculates an average of a stat
    float average(int statIndex);
    // divides two stats and prevents NaN
    float divide(int firstStat, int secondStat);


    // extra stats beyond basic ones
    // writes shooting percentage on a shot or a goal
    void writeShootingPercentage();
    // writes K/D ratio
    void writeKillPercentage();
    // writes missed exterms
    void writeMissedExterms();
    // writes win percentage
    void writeWinPercentage();

    int getPercentage(int numerator, int denominator);

    // renders overlay
    //void render(CanvasWrapper canvas);

    void renderAllStrings();
    std::string statToRenderString(int statIndex, int state);

    // Plugin settings interface
    void RenderSettings() override;
    std::string GetPluginName() override;
    void SetImGuiContext(uintptr_t ctx) override;

    // Plugin window
    virtual void Render() override;
    virtual std::string GetMenuName() override;
    virtual std::string GetMenuTitle() override;
    virtual bool ShouldBlockInput() override;
    virtual bool IsActiveOverlay() override;
    virtual void OnOpen() override;
    virtual void OnClose() override;

    void colorSettings();
    void enableSettings();
    void locationAndScaleSettings();
    void statSettings(int renderIndex);
    void addRemoveStatSettings();

private:
    bool isWindowOpen_ = false;
    std::string menuTitle_ = "OBSCounter";
    bool inDragMode = false;
    ImFont* font;

    enum statStates {
        STAT_DEFAULT,
        STAT_AVERAGE,
        STAT_GAME,
        STAT_OTHER,
        STAT_CAREER_START = STAT_OTHER,
        STAT_CAREER_TOTAL,
        STAT_CAREER_PRIVATE,
        STAT_CAREER_RANKED,
        STAT_CAREER_CASUAL,
        STAT_CAREER_AVERAGE,
        STAT_TEAM_START = STAT_CAREER_AVERAGE,
        STAT_TEAM,
        STAT_TEAM_OPPONENT,
        STAT_END
    };

    int careerStatTotal[NUMCAREERSTATS];
    int careerStatTotalOffset[NUMCAREERSTATS];
    int careerStatPrivate[NUMCAREERSTATS];
    int careerStatRanked[NUMCAREERSTATS];
    int careerStatCasual[NUMCAREERSTATS];
    float careerStatAverage[NUMCAREERSTATS];

    std::filesystem::path fileLocation;

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
    const int defaultMaxOverlayLines = 10;
    int overlayLines;
    std::vector<int> overlayStats;
    std::vector<int> overlayStatsCareer;
    std::vector<int> overlayStates;
    std::vector<std::string> overlayStrings;
    float xLocation;
    float yLocation;
    float scale;
    Vector2 screenSize;
    LinearColor overlayColor;
    LinearColor overlayBackgroundColor;
    LinearColor overlayOutlineColor;
    float outlineRounding;
    float outlineSize;

    // holds all stats
    int statArray[NUMSTATS];
    int statArrayTeam[ENDNORMALSTATS - 1];
    int statArrayOpponent[ENDNORMALSTATS - 1];
    int statArrayGame[NUMSTATS];
    std::string statArrayOther[NUMOTHERSTATS];

    // holds all averages
    // caching improves performance significantly
    float averages[NUMSTATS];
};

