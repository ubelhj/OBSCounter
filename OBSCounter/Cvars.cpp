#include "pch.h"
#include "OBSCounter.h"

// creates cvars and sets global variable defaults to prevent any nulls
void OBSCounter::setCvars() {
    auto decimalsVar = cvarManager->registerCvar("counter_decimals", "1",
        "set decimal places in averages (1 - 10)", true, true, 1, true, 10);
    decimalPlaces = decimalsVar.getIntValue();
    decimalsVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        decimalPlaces = cvar.getIntValue();
        writeAll();
        });

    // in-game overlay cvars
    // enables or disables overlay
    auto overlayEnableVar = cvarManager->registerCvar("counter_enable_ingame",
        "0", "enables in game overlay");
    enabledOverlay = overlayEnableVar.getBoolValue();
    overlayEnableVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        enabledOverlay = cvar.getBoolValue();
        });

    // sets number of stats in in game overlay
    // 1-5 allowed
    auto overlayNumberVar = cvarManager->registerCvar(
        "counter_ingame_numStats", "5", "number of stats in in game overlay",
        true, true, 1, true, defaultMaxOverlayLines);
    overlayLines = overlayNumberVar.getIntValue();
    overlayNumberVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayLines = cvar.getIntValue();
        });

    for (int i = 0; i < defaultMaxOverlayLines; i++) {
        overlayStrings.push_back("");

        std::string str = std::to_string(i);
        // sets stat in overlay
        auto overlayVar = cvarManager->registerCvar("counter_ingame_stat_" + str,
            str, "stat " + str + " in overlay", true, true, 0);
        overlayStats.push_back(overlayVar.getIntValue());
        overlayVar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            overlayStats[i] = cvar.getIntValue();
            });

        // sets stat in overlay
        auto overlayVarCareer = cvarManager->registerCvar("counter_ingame_stat_career_" + str,
            str, "stat " + str + " in overlay", true, true, 0);
        overlayStatsCareer.push_back(overlayVarCareer.getIntValue());
        overlayVarCareer.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            overlayStatsCareer[i] = cvar.getIntValue();
            });

        // sets overlay stat state
        auto overlayStateVar = cvarManager->registerCvar(
            "counter_ingame_stat_render_state_" + str, "0",
            "Sets render state of stat " + str + " in overlay", true, true, 0, true, STAT_END - 1);
        overlayStates.push_back(STAT_DEFAULT);
        overlayStateVar.addOnValueChanged([this, i, str](std::string, CVarWrapper cvar) {
            overlayStates[i] = cvar.getIntValue();
            });
    }

    // sets cvar to move counter's X location
    auto xLocVar = cvarManager->registerCvar("counter_ingame_x_location",
        "0.86", "set location of ingame counter X in % of screen",
        true, true, 0.0, true, 1.0);
    xLocation = xLocVar.getFloatValue();
    xLocVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        xLocation = cvar.getFloatValue();
        });

    // sets cvar to move counter's Y location
    auto yLocVar = cvarManager->registerCvar("counter_ingame_y_location",
        "0", "set location of ingame counter Y in % of screen",
        true, true, 0.0, true, 1.0);
    yLocation = yLocVar.getFloatValue();
    yLocVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        yLocation = cvar.getFloatValue();
        });

    // sets cvar for counter's scale
    auto scaleVar = cvarManager->registerCvar("counter_ingame_scale",
        "2", "set counter scale",
        true, true, 0.0, true, 10.0);
    scale = scaleVar.getFloatValue();
    scaleVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        scale = cvar.getFloatValue();
        });

    auto colorVar = cvarManager->registerCvar("counter_color", "#FFFFFF", "color of overlay");
    overlayColor = colorVar.getColorValue();
    colorVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayColor = cvar.getColorValue();
        });

    auto overlayBackgroundEnableVar = cvarManager->registerCvar("counter_enable_background",
        "0", "enables in game overlay background");
    enabledOverlayBackground = overlayBackgroundEnableVar.getBoolValue();
    overlayBackgroundEnableVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        enabledOverlayBackground = cvar.getBoolValue();
        });

    auto backColorVar = cvarManager->registerCvar("counter_color_background", "#0000008C", "color of overlay background");
    overlayBackgroundColor = backColorVar.getColorValue();
    backColorVar.addOnValueChanged([this](std::string, CVarWrapper cvar) {
        overlayBackgroundColor = cvar.getColorValue();
        });

    // lists all the stats and their numbers to use in the in game counter
    cvarManager->registerNotifier("counter_list_stats",
        [this](std::vector<std::string> params) {
            writeCareerStats();
        }, "List all different stat types", PERMISSION_ALL);

    // adds 1 to games to fix any errors in game tracking
    cvarManager->registerNotifier("counter_add_game",
        [this](std::vector<std::string> params) {
            endedGame = true;
            startGame();
            endedGame = true;
        }, "Add a game to stats to fix any sync issues", PERMISSION_ALL);

    // resets all stats
    cvarManager->registerNotifier("counter_reset",
        [this](std::vector<std::string> params) {
            for (int i = 0; i < numStats; i++) {
                statArray[i] = 0;
                statArrayGame[i] = 0;
            }

            writeAll();
        }, "Resets all stats", PERMISSION_ALL);

    // creates setters for the default start value for each stat
    // also writes these new values to files
    for (int i = 0; i <= endNormalStats; i++) {
        std::string cvarName = "counter_set_" + indexStringMap[i];
        std::string cvarTip = "sets " + indexStringMap[i] + " value";
        cvarManager->registerCvar(cvarName, "0", cvarTip, true, false, 0, false, 0, false);
        auto cvar = cvarManager->getCvar(cvarName);
        statArray[i] = cvar.getIntValue();

        if (i != games) {
            cvar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
                statArray[i] = cvar.getIntValue(); write(i); });
        }
    }

    // creates setters for the default start value for time stats
    // also writes these new values to files
    for (int i = endNormalStats + 1; i < numStats; i++) {
        std::string cvarName = "counter_set_" + indexStringMap[i];
        std::string cvarTip = "sets " + indexStringMap[i] + " value";
        cvarManager->registerCvar(cvarName, "0", cvarTip, true, false, 0, false, 0, false);
        auto cvar = cvarManager->getCvar(cvarName);
        statArray[i] = cvar.getIntValue();

        cvar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            statArray[i] = cvar.getIntValue(); writeTimeStat(i); });
    }

    // setters for render strings for stats
    for (int i = 0; i < numStats; i++) {
        std::string cvarName = "counter_set_render_string_" + indexStringMap[i];
        std::string cvarTip = "sets " + indexStringMap[i] + " render string";
        cvarManager->registerCvar(cvarName, indexStringMapRender[i], cvarTip);
        auto cvar = cvarManager->getCvar(cvarName);
        cvar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            indexStringMapRender[i] = cvar.getStringValue(); });

        // setters for render strings for average stats
        cvarName = "counter_set_render_string_" + averageStrings[i];
        cvarTip = "sets " + averageStrings[i] + " render string";
        cvarManager->registerCvar(cvarName, averageStringsRender[i], cvarTip);
        cvar = cvarManager->getCvar(cvarName);
        statArray[i] = cvar.getIntValue();
        cvar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            averageStringsRender[i] = cvar.getStringValue(); });

        // setters for render strings for game stats
        cvarName = "counter_set_render_string_" + indexStringMapGame[i];
        cvarTip = "sets " + indexStringMapGame[i] + " render string";
        cvarManager->registerCvar(cvarName, indexStringMapRenderGame[i], cvarTip);
        cvar = cvarManager->getCvar(cvarName);
        cvar.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
            indexStringMapRenderGame[i] = cvar.getStringValue(); });
    }

    for (int i = 0; i < numOtherStats; i++) {
        // setters for render strings for other stats
        std::string cvarName = "counter_set_render_string_" + indexStringMapOther[i];
        std::string cvarTip = "sets " + indexStringMapOther[i] + " render string";
        cvarManager->registerCvar(cvarName, indexStringMapRenderOther[i], cvarTip)
            .addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
                indexStringMapRenderOther[i] = cvar.getStringValue();
            });
    }

    for (int i = 0; i < NUMCAREERSTATS; i++) {
        // setters for render strings for career stats
        std::string cvarNameTotal = "counter_set_render_string_total" + indexStringMapCareer[i];
        std::string cvarTipTotal = "sets Total" + indexStringMapCareer[i] + " render string";
        cvarManager->registerCvar(cvarNameTotal, indexStringMapRenderCareerTotal[i], cvarTipTotal)
            .addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
                indexStringMapRenderCareerTotal[i] = cvar.getStringValue();
            });

        std::string cvarNamePrivate = "counter_set_render_string_private" + indexStringMapCareer[i];
        std::string cvarTipPrivate = "sets Private" + indexStringMapCareer[i] + " render string";
        cvarManager->registerCvar(cvarNamePrivate, indexStringMapRenderCareerPrivate[i], cvarTipPrivate)
            .addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
                indexStringMapRenderCareerPrivate[i] = cvar.getStringValue();
            });

        std::string cvarNameRanked = "counter_set_render_string_ranked" + indexStringMapCareer[i];
        std::string cvarTipRanked = "sets Ranked" + indexStringMapCareer[i] + " render string";
        cvarManager->registerCvar(cvarNameRanked, indexStringMapRenderCareerRanked[i], cvarTipRanked)
            .addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
                indexStringMapRenderCareerRanked[i] = cvar.getStringValue();
            });

        std::string cvarNameCasual = "counter_set_render_string_casual" + indexStringMapCareer[i];
        std::string cvarTipCasual = "sets Casual" + indexStringMapCareer[i] + " render string";
        cvarManager->registerCvar(cvarNameCasual, indexStringMapRenderCareerCasual[i], cvarTipCasual)
            .addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
                indexStringMapRenderCareerCasual[i] = cvar.getStringValue();
            });
    }

    // special case to make sure games update properly
    // needs to call startGame() to ensure averages and 
    //  game stats are written
    cvarManager->getCvar("counter_set_games").addOnValueChanged(
        [this](std::string, CVarWrapper cvar) {
            statArray[games] = cvar.getIntValue() - 1; endedGame = true;
            startGame(); endedGame = true; });
}