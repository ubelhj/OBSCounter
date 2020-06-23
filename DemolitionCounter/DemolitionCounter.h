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
	
	// enables or disables plugin
	void updateEnabled(bool enabled);

	// fires when a stat happens and this is enabled
	void statEvent(ServerWrapper caller, void* args);

	void demolition(PriWrapper receiver);

	void extermination(PriWrapper receiver);

	void startGame();

	void writeDemos();

	void writeExterms();

	void writeGames();
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

