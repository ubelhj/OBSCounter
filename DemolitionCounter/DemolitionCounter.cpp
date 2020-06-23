#include "pch.h"
#include "DemolitionCounter.h"
#include <iostream>
#include <fstream>
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

BAKKESMOD_PLUGIN(DemolitionCounter, "Counts demolitions in online games", plugin_version, PLUGINTYPE_THREADED)

bool enabledCounter;
int demos = 0;
int exterms = 0;

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
		gameWrapper->HookEventWithCaller<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage", 
			std::bind(&DemolitionCounter::StatEvent, this, std::placeholders::_1, std::placeholders::_2));
	}
	else {
		gameWrapper->UnhookEvent("Function TAGame.GFxHUD_TA.HandleStatTickerMessage");
	}
}

struct TheArgStruct
{
	uintptr_t Receiver;
	uintptr_t Victim;
	uintptr_t StatEvent;
};

void DemolitionCounter::StatEvent(ServerWrapper caller, void* args) {
	auto tArgs = (TheArgStruct*)args;
	cvarManager->log("stat event!");

	auto victim = PriWrapper(tArgs->Victim);
	auto receiver = PriWrapper(tArgs->Receiver);
	auto statEvent = StatEventWrapper(tArgs->StatEvent);
	auto label = statEvent.GetLabel();
	cvarManager->log(label.ToString());

	// if the event is a demo
	if (label.ToString().compare("Demolition") == 0) {
		DemolitionCounter::demolition(receiver);
		return;
	}

	// if event is an exterm
	if (label.ToString().compare("Extermination") == 0) {
		DemolitionCounter::extermination(receiver);
		return;
	}
	
}

void DemolitionCounter::demolition(PriWrapper receiver) {
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

	auto primaryPri = primary.GetPRI();

	if (primaryPri.IsNull()) {
		cvarManager->log("null primary pri");
		return;
	}

	auto receiverID = receiver.GetUniqueId();
	auto primaryID = primaryPri.GetUniqueId();

	if (receiverID.ID == primaryID.ID) {
		cvarManager->log("main player demo");
		demos++;
		cvarManager->log(std::to_string(demos));

		std::ofstream demoFile;
		demoFile.open("./DemolitionCounter/demolitions.txt");
		demoFile << std::to_string(demos);
		demoFile.close();
	}
}

void DemolitionCounter::extermination(PriWrapper receiver) {
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

	auto primaryPri = primary.GetPRI();

	if (primaryPri.IsNull()) {
		cvarManager->log("null primary pri");
		return;
	}

	auto receiverID = receiver.GetUniqueId();
	auto primaryID = primaryPri.GetUniqueId();

	if (receiverID.ID == primaryID.ID) {
		cvarManager->log("main player exterm");
		exterms++;
		cvarManager->log(std::to_string(exterms));

		std::ofstream demoFile;
		demoFile.open("./DemolitionCounter/exterminations.txt");
		demoFile << std::to_string(exterms);
		demoFile.close();
	}
}

void DemolitionCounter::onUnload()
{
}