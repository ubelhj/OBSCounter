#include "pch.h"
#include "DemolitionCounter.h"

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
}

void DemolitionCounter::onUnload()
{
}