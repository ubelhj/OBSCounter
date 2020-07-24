#pragma once
#pragma comment(lib, "bakkesmod.lib")

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"

constexpr auto plugin_version = "1.0";

class DemolitionCounter: public BakkesMod::Plugin::BakkesModPlugin/*, public BakkesMod::Plugin::PluginWindow*/
{

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	// sets cvars to modify counters 
	void setCvars();

	// hooks events for stats
	void hookEvents();

	// fires when a stat happens and this is enabled
	void statEvent(ServerWrapper caller, void* args);

	// checks if the plyer that received a stat is the main player
	bool isPrimaryPlayer(PriWrapper receiver);

	// called when a new game starts, resets game stats
	void startGame();

	// called when a game ends, allows next game to be started
	void endGame();


	// writes the .txt files
	// writes all at once
	void writeAll();
	// writes all game stats
	void writeAllGameStats();

	// writes a specific stat
	void write(int statIndex);
	// writes a game stat only
	void writeGameStat(int statIndex);
	// calculates an average of a stat
	float average(int statValue);


	// extra stats beyond basic ones
	// writes shooting percentage on a shot or a goal
	void writeShootingPercentage();

	// writes K/D ratio
	void writeKillPercentage();

	// writes missed exterms
	void writeMissedExterms();

	// renders overlay
	void render(CanvasWrapper canvas);

	// prints all stat types
	void listStats();
};

