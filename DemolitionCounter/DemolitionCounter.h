#pragma once
#pragma comment(lib, "pluginsdk.lib")

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"

constexpr auto plugin_version = "3.0";

class DemolitionCounter: public BakkesMod::Plugin::BakkesModPlugin
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
	void statEvent2(ServerWrapper caller, void* args);

	// fires when the stat ticker is updated
	void statTickerEvent(ServerWrapper caller, void* args);

	// checks if the plyer that received a stat is the main player
	bool isPrimaryPlayer(PriWrapper receiver);

	// called when a new game starts, resets game stats
	void startGame();

	void endGame();


	// writes the .txt files

	// writes all at once
	void writeAll();
	// writes a specific stat
	void write(int statIndex);
	// writes a game stat only
	void writeGameStat(int statIndex);
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

	// prints all stat types
	void listStats();
};

