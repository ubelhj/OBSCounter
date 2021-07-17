#include "pch.h"
#include "OBSCounter.h"

std::string OBSCounter::GetPluginName() {
	return "OBSCounter";
}

void OBSCounter::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void OBSCounter::RenderSettings() {
	ImGui::TextUnformatted("OBSCounter plugin settings");
}