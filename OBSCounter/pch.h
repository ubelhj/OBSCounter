#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include "bakkesmod/plugin/bakkesmodplugin.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <array> 

#include "imgui/imgui.h"

#include "fmt/core.h"
#include "fmt/ranges.h"

extern std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

template<typename S, typename... Args>
void LOG(const S& format_str, Args&&... args)
{
    _globalCvarManager->log(fmt::format(format_str, args...));
}

const bool debugLog = false;
template<typename S, typename... Args>
void DEBUGLOG(const S& format_str, Args&&... args)
{
    if (debugLog) {
        _globalCvarManager->log(fmt::format(format_str, args...));
    }
}