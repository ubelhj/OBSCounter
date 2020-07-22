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
	void writeAll();
	void write(int statIndex);
	void writeDemos();
	void writeDeaths();
	void writeExterms();
	void writeGames();
	void writeWins();
	void writeMvps();
	void writeGoals();
	void writeAerialGoals();
	void writeBackwardsGoals();
	void writeBicycleGoals();
	void writeLongGoals();
	void writeTurtleGoals();
	void writePoolShots();
	void writeOvertimeGoals();
	void writeHatTricks();
	void writeAssists();
	void writePlaymakers();
	void writeSaves();
	void writeEpicSaves();
	void writeSaviors();
	void writeShots();
	void writeCenters();
	void writeClears();
	void writeFirstTouchs();
	void writeDamages();
	void writeUltraDamages();
	void writeLowFives();
	void writeHighFives();
	void writeSwishs();
	void writeShootingPercentage();
	
	// Inherited via PluginWindow
	/*

	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "DemolitionCounter";

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	
	*/
};

