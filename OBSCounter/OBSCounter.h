#pragma once


#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
#include <sstream>
#include <fstream>
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class OBSCounter: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow
{

	//std::shared_ptr<bool> enabled;

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
	// writes a game stat only
	void writeGameStat(int statIndex);
	// writes time stats
	void writeTimeStat(int statIndex);
	// writes a game stat only but formats for time
	void writeGameTimeStat(int statIndex);
	// calculates an average of a stat
	float average(int statValue);
	// divides two stats and prevents NaN
	float divide(int firstStatIndex, int secondStatIndex);


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
	void render(CanvasWrapper canvas);

	void renderString(int statIndex);
	void renderAllStrings();
	std::string statToRenderString(int statIndex, bool isAverage);

	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;

	// prints all stat types
	void listStats();
};

