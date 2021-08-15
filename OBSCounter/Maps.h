// enum for all stat indexes 
// easier to refer back to stat names
// total stats are from 0 to 30, game stats from 31 to end
static enum stats {
    wins,
    losses,
    mvps,
    games,
    // end of stats without game counterpart
    statsWithoutGame = games,
    // stat + 27 = gameStat
    goals,
    demos,
    deaths,
    exterms,
    aerialGoals,
    backwardsGoals,
    bicycleGoals,
    longGoals,
    turtleGoals,
    poolShots,
    overtimeGoals,
    hatTricks,
    assists,
    playmakers,
    saves,
    epicSaves,
    saviors,
    shots,
    centers,
    clears,
    firstTouchs,
    damages,
    ultraDamages,
    lowFives,
    highFives,
    swishs,
    bicycleHits,
    points,
    totalSaves,
    ownGoals,
    // last stat with normal output type
    endNormalStats = ownGoals,
    timePlayed,
    offenseTime,
    defenseTime,
    // total number of stats in the stat array
    numStats
};

static std::string indexStringMap[numStats] = {
    "wins",
    "losses",
    "mvps",
    "games",
    "goals",
    "demolitions",
    "deaths",
    "exterminations",
    "aerialGoals",
    "backwardsGoals",
    "bicycleGoals",
    "longGoals",
    "turtleGoals",
    "poolShots",
    "overtimeGoals",
    "hatTricks",
    "assists",
    "playmakers",
    "saves",
    "epicSaves",
    "saviors",
    "shots",
    "centers",
    "clears",
    "firstTouchs",
    "damages",
    "ultraDamages",
    "lowFives",
    "highFives",
    "swishs",
    "bicycleHits",
    "points",
    "totalSaves",
    "ownGoals",
    "timePlayed",
    "offenseTime",
    "defenseTime"
};

static const char* indexStringMapChar[numStats] = {
    "wins",
    "losses",
    "mvps",
    "games",
    "goals",
    "demolitions",
    "deaths",
    "exterminations",
    "aerialGoals",
    "backwardsGoals",
    "bicycleGoals",
    "longGoals",
    "turtleGoals",
    "poolShots",
    "overtimeGoals",
    "hatTricks",
    "assists",
    "playmakers",
    "saves",
    "epicSaves",
    "saviors",
    "shots",
    "centers",
    "clears",
    "firstTouchs",
    "damages",
    "ultraDamages",
    "lowFives",
    "highFives",
    "swishs",
    "bicycleHits",
    "points",
    "totalSaves",
    "ownGoals",
    "timePlayed",
    "offenseTime",
    "defenseTime"
};

static std::string indexStringMapGame[numStats] = {
    "gameWins",
    "gameLosses",
    "gameMvps",
    "gameGames",
    "gameGoals",
    "gameDemolitions",
    "gameDeaths",
    "gameExterminations",
    "gameAerialGoals",
    "gameBackwardsGoals",
    "gameBicycleGoals",
    "gameLongGoals",
    "gameTurtleGoals",
    "gamePoolShots",
    "gameOvertimeGoals",
    "gameHatTricks",
    "gameAssists",
    "gamePlaymakers",
    "gameSaves",
    "gameEpicSaves",
    "gameSaviors",
    "gameShots",
    "gameCenters",
    "gameClears",
    "gameFirstTouchs",
    "gameDamages",
    "gameUltraDamages",
    "gameLowFives",
    "gameHighFives",
    "gameSwishs",
    "gameBicycleHits",
    "gamePoints",
    "gameTotalSaves",
    "gameOwnGoals",
    "gameTimePlayed",
    "gameOffenseTime",
    "gameDefenseTime"
};

static std::string indexStringMapRender[numStats] = {
    "Wins: ",
    "Losses: ",
    "MVPs: ",
    "Games: ",
    "Goals: ",
    "Demolitions: ",
    "Deaths: ",
    "Exterminations: ",
    "Aerial Goals: ",
    "Backwards Goals: ",
    "Bicycle Goals: ",
    "Long Goals: ",
    "Turtle Goals: ",
    "Pool Shots: ",
    "Overtime Goals: ",
    "Hat Tricks: ",
    "Assists: ",
    "Playmakers: ",
    "Saves: ",
    "Epic Saves: ",
    "Saviors: ",
    "Shots: ",
    "Centers: ",
    "Clears: ",
    "First Touches: ",
    "Damages: ",
    "Ultra Damages: ",
    "LowFives: ",
    "HighFives: ",
    "Swishs: ",
    "Bicycle Hits: ",
    "Points: ",
    "Total Saves: ",
    "Own Goals: ",
    "Time Played: ",
    "Offense Time: ",
    "Defense Time: "
};

static std::string indexStringMapRenderGame[numStats] = {
    "Game Wins: ",
    "Game Losses: ",
    "Game MVPs: ",
    "Game Games: ",
    "Game Goals: ",
    "Game Demolitions: ",
    "Game Deaths: ",
    "Game Exterminations: ",
    "Game Aerial Goals: ",
    "Game Backwards Goals: ",
    "Game Bicycle Goals: ",
    "Game Long Goals: ",
    "Game Turtle Goals: ",
    "Game Pool Shots: ",
    "Game Overtime Goals: ",
    "Game Hat Tricks: ",
    "Game Assists: ",
    "Game Playmakers: ",
    "Game Saves: ",
    "Game Epic Saves: ",
    "Game Saviors: ",
    "Game Shots: ",
    "Game Centers: ",
    "Game Clears: ",
    "Game First Touches: ",
    "Game Damages: ",
    "Game Ultra Damages: ",
    "Game Low Fives: ",
    "Game High Fives: ",
    "Game Swishs: ",
    "Game Bicycle Hits: ",
    "Game Points: ",
    "Game Total Saves: ",
    "Game Own Goals: ",
    "Game Time Played: ",
    "Game Offense Time: ",
    "Game Defense Time: "
};

static std::string averageStrings[numStats] = {
    "averageWins",
    "averageLosses",
    "averageMvps",
    "averageGames",
    "averageGoals",
    "averageDemolitions",
    "averageDeaths",
    "averageExterminations",
    "averageAerialGoals",
    "averageBackwardsGoals",
    "averageBicycleGoals",
    "averageLongGoals",
    "averageTurtleGoals",
    "averagePoolShots",
    "averageOvertimeGoals",
    "averageHatTricks",
    "averageAssists",
    "averagePlaymakers",
    "averageSaves",
    "averageEpicSaves",
    "averageSaviors",
    "averageShots",
    "averageCenters",
    "averageClears",
    "averageFirstTouchs",
    "averageDamages",
    "averageUltraDamages",
    "averageLowFives",
    "averageHighFives",
    "averageSwishs",
    "averageBicycleHits",
    "averagePoints",
    "averageTotalSaves",
    "averageOwnGoals",
    "averageTimePlayed",
    "averageOffenseTime",
    "averageDefenseTime"
};

static std::string averageStringsRender[numStats] = {
    "Average Wins: ",
    "Average Losses: ",
    "Average MVPs: ",
    "Average Games: ",
    "Average Goals: ",
    "Average Demolitions: ",
    "Average Deaths: ",
    "Average Exterminations: ",
    "Average Aerial Goals: ",
    "Average Backwards Goals: ",
    "Average Bicycle Goals: ",
    "Average Long Goals: ",
    "Average Turtle Goals: ",
    "Average PoolShots: ",
    "Average Overtime Goals: ",
    "Average Hat Tricks: ",
    "Average Assists: ",
    "Average Playmakers: ",
    "Average Saves: ",
    "Average Epic Saves: ",
    "Average Saviors: ",
    "Average Shots: ",
    "Average Centers: ",
    "Average Clears: ",
    "Average First Touchs: ",
    "Average Damages: ",
    "Average Ultra Damages: ",
    "Average Low Fives: ",
    "Average High Fives: ",
    "Average Swishs: ",
    "Average Bicycle Hits: ",
    "Average Points: ",
    "Average Total Saves: ",
    "Average Own Goals: ",
    "Average Time Played: ",
    "Average Offense Time: ",
    "Average Defense Time: "
};

static const std::map<std::string, int> eventDictionary = {
    { "Demolish", demos},
    { "Demolition", exterms},
    { "Goal", goals},
    { "Win", wins},
    { "MVP", mvps},
    { "AerialGoal", aerialGoals},
    { "BackwardsGoal", backwardsGoals},
    { "BicycleGoal", bicycleGoals},
    { "LongGoal", longGoals},
    { "TurtleGoal", turtleGoals},
    { "PoolShot", poolShots},
    { "OvertimeGoal", overtimeGoals},
    { "HatTrick", hatTricks},
    { "Assist", assists},
    { "Playmaker", playmakers},
    { "Save", saves},
    { "EpicSave", epicSaves},
    { "Savior", saviors},
    { "Shot", shots},
    { "Center", centers},
    { "Clear", clears},
    { "FirstTouch", firstTouchs},
    { "BreakoutDamage", damages},
    { "BreakoutDamageLarge", ultraDamages},
    { "LowFive", lowFives},
    { "HighFive", highFives},
    { "HoopsSwishGoal", swishs},
    { "BicycleHit", bicycleHits},
    { "OwnGoal", ownGoals}
};

static enum otherStats {
    shootingPercentage,
    gameShootingPercentage,
    kDRatio,
    gameKDRatio,
    possibleExterminations,
    missedExterminationPercent,
    winPercentage,
    numOtherStats
};

static std::string indexStringMapOther[numOtherStats] = {
    "shootingPercentage",
    "gameShootingPercentage",
    "kDRatio",
    "gameKDRatio",
    "possibleExterminations",
    "missedExterminationPercent",
    "winPercentage"
};

static const char* indexStringMapOtherChar[numOtherStats] = {
    "shootingPercentage",
    "gameShootingPercentage",
    "kDRatio",
    "gameKDRatio",
    "possibleExterminations",
    "missedExterminationPercent",
    "winPercentage"
};

static std::string indexStringMapRenderOther[numOtherStats] = {
    "Shooting Percentage: ",
    "Game Shooting Percentage: ",
    "KD Ratio: ",
    "Game KD Ratio: ",
    "Possible Exterminations: ",
    "Missed Extermination Percent: ",
    "Win Percentage: "
};

static const int NUMCAREERSTATS = 44;

static std::string indexStringMapCareer[44] = {
    "FirstTouch",
    "Shot",
    "Goal",
    "Center",
    "HatTrick",
    "Clear",
    "Win",
    "MVP",
    "Assist",
    "Demolish",
    "Save",
    "Loss",
    "AerialHit",
    "EpicSave",
    "LongGoal",
    "PoolShot",
    "BicycleHit",
    "BackwardsGoal",
    "JuggleHit",
    "Playmaker",
    "OvertimeGoal",
    "Savior",
    "FewestBallTouches",
    "CarTouches",
    "FastestGoal",
    "SlowestGoal",
    "FurthestGoal",
    "MostBallTouches",
    "MostBoostPickups",
    "Demolition",
    "BicycleGoal",
    "AerialGoal",
    "FreezeSave",
    "OwnGoal",
    "TurtleGoal",
    "FewestBoostPickups",
    "BreakoutDamage",
    "BreakoutDamageLarge",
    "TimePlayed",
    "LowFive",
    "HoopsSwishGoal",
    "BallHit",
    "HighFive",
    "BoostPickups"
};

static std::string indexStringMapRenderCareer[44] = {
    "Career First Touches: ",
    "Career Shots: ",
    "Career Goals: ",
    "Career Centers: ",
    "Career Hat Tricks: ",
    "Career Clears: ",
    "Career Wins: ",
    "Career MVPs: ",
    "Career Assists: ",
    "Career Demolitions: ",
    "Career Saves: ",
    "Career Losses: ",
    "Career Aerial Hits: ",
    "Career Epic Saves: ",
    "Career Long Goals: ",
    "Career Pool Shots: ",
    "Career Bicycle Hits: ",
    "Career Backwards Goals: ",
    "Career Juggle Hits: ",
    "Career Playmakers: ",
    "Career Overtime Goals: ",
    "Career Saviors: ",
    "Career Fewest Ball Touches: ",
    "Career Car Touches: ",
    "Career Fastest Goals: ",
    "Career Slowest Goals: ",
    "Career Furthest Goals: ",
    "Career Most Ball Touches: ",
    "Career Most Boost Pickups: ",
    "Career Exterminations: ",
    "Career Bicycle Goals: ",
    "Career Aerial Goals: ",
    "Career Freeze Saves: ",
    "Career Own Goals: ",
    "Career Turtle Goals: ",
    "Career Fewest Boost Pickups: ",
    "Career Breakout Damage: ",
    "Career Breakout Damage Large: ",
    "Career Time Played: ",
    "Career Low Fives: ",
    "Career Hoops Swish Goals: ",
    "Career Ball Hits: ",
    "Career High Fives: ",
    "Career Boost Pickups: "
};

