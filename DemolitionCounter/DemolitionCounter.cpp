#include "pch.h"
#include "DemolitionCounter.h"
#include <iostream>
#include <fstream>
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

BAKKESMOD_PLUGIN(DemolitionCounter, "write a plugin description here", plugin_version, PLUGINTYPE_THREADED)

bool enabledCounter;
int demos = 0;

void DemolitionCounter::onLoad()
{
	cvarManager->log("Plugin loaded!");
	auto enabledVar = cvarManager->registerCvar("counter_enabled", "0", "activate/deactivate demolition counter");

	updateEnabled(enabledVar.getBoolValue());
	enabledVar.addOnValueChanged([this](std::string, CVarWrapper cvar) { updateEnabled(cvar.getBoolValue()); });


}

void DemolitionCounter::updateEnabled(bool enabled) {
	enabledCounter = enabled;

	if (enabled) {
		gameWrapper->HookEvent("Function TAGame.Car_TA.OnDemolished", std::bind(&DemolitionCounter::demolishEvent, this));
	}
}

void DemolitionCounter::demolishEvent() {
	cvarManager->log("demolition happened");
	if (!gameWrapper->IsInOnlineGame()) {
		cvarManager->log("not in game");
		return;
	}

	ServerWrapper sw = gameWrapper->GetOnlineGame();

	if (sw.IsNull()) {
		cvarManager->log("game not server");
		return;
	}

	PlayerControllerWrapper player = sw.GetLocalPrimaryPlayer();

	if (player.IsNull()) {
		cvarManager->log("no primary player");
		return;
	}

	PriWrapper playerPri = player.GetPRI();

	if (playerPri.IsNull()) {
		cvarManager->log("null player pri");
		return;
	}

	int demos = playerPri.GetMatchDemolishes();

	std::string targetStr = std::to_string(demos);

	cvarManager->log(targetStr);
	cvarManager->log("end demoed");
}

void DemolitionCounter::onUnload()
{
}