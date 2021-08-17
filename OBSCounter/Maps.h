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

extern std::string indexStringMapRender[numStats];

extern std::string indexStringMapRenderGame[numStats];

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

extern std::string averageStringsRender[numStats];

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

extern std::string indexStringMapRenderOther[numOtherStats];

static enum careerStats {
    FirstTouch,
    Shot,
    Goal,
    Center,
    HatTrick,
    Clear,
    Win,
    MVP,
    Assist,
    Demolish,
    Save,
    Loss,
    AerialHit,
    EpicSave,
    LongGoal,
    PoolShot,
    BicycleHit,
    BackwardsGoal,
    JuggleHit,
    Playmaker,
    OvertimeGoal,
    Savior,
    FewestBallTouches,
    CarTouches,
    FastestGoal,
    SlowestGoal,
    FurthestGoal,
    MostBallTouches,
    MostBoostPickups,
    Demolition,
    BicycleGoal,
    AerialGoal,
    FreezeSave,
    OwnGoal,
    TurtleGoal,
    FewestBoostPickups,
    BreakoutDamage,
    BreakoutDamageLarge,
    TimePlayed,
    LowFive,
    HoopsSwishGoal,
    BallHit,
    HighFive,
    BoostPickups,
    Game,
    NUMCAREERSTATS
};

static const std::map<std::string, int> eventDictionaryCareer = {
    { "FirstTouch", FirstTouch },
    { "Shot", Shot },
    { "Goal", Goal },
    { "Center", Center },
    { "HatTrick", HatTrick },
    { "Clear", Clear },
    { "Win", Win },
    { "MVP", MVP },
    { "Assist", Assist },
    { "Demolish", Demolish },
    { "Save", Save },
    { "Loss", Loss },
    { "AerialHit", AerialHit },
    { "EpicSave", EpicSave },
    { "LongGoal", LongGoal },
    { "PoolShot", PoolShot },
    { "BicycleHit", BicycleHit },
    { "BackwardsGoal", BackwardsGoal },
    { "JuggleHit", JuggleHit },
    { "Playmaker", Playmaker },
    { "OvertimeGoal", OvertimeGoal },
    { "Savior", Savior },
    { "FewestBallTouches", FewestBallTouches },
    { "CarTouches", CarTouches },
    { "FastestGoal", FastestGoal },
    { "SlowestGoal", SlowestGoal },
    { "FurthestGoal", FurthestGoal },
    { "MostBallTouches", MostBallTouches },
    { "MostBoostPickups", MostBoostPickups },
    { "Demolition", Demolition },
    { "BicycleGoal", BicycleGoal },
    { "AerialGoal", AerialGoal },
    { "FreezeSave", FreezeSave },
    { "OwnGoal", OwnGoal },
    { "TurtleGoal", TurtleGoal },
    { "FewestBoostPickups", FewestBoostPickups },
    { "BreakoutDamage", BreakoutDamage },
    { "BreakoutDamageLarge", BreakoutDamageLarge },
    { "TimePlayed", TimePlayed },
    { "LowFive", LowFive },
    { "HoopsSwishGoal", HoopsSwishGoal },
    { "BallHit", BallHit },
    { "HighFive", HighFive },
    { "BoostPickups", BoostPickups }
};

static std::string indexStringMapCareer[NUMCAREERSTATS] = {
    "CareerFirstTouches",
    "CareerShots",
    "CareerGoals",
    "CareerCenters",
    "CareerHatTricks",
    "CareerClears",
    "CareerWins",
    "CareerMVPs",
    "CareerAssists",
    "CareerDemolitions",
    "CareerSaves",
    "CareerLosses",
    "CareerAerialHits",
    "CareerEpicSaves",
    "CareerLongGoals",
    "CareerPoolShots",
    "CareerBicycleHits",
    "CareerBackwardsGoals",
    "CareerJuggleHits",
    "CareerPlaymakers",
    "CareerOvertimeGoals",
    "CareerSaviors",
    "CareerFewestBallTouches",
    "CareerCarTouches",
    "CareerFastestGoals",
    "CareerSlowestGoals",
    "CareerFurthestGoals",
    "CareerMostBallTouches",
    "CareerMostBoostPickups",
    "CareerExterminations",
    "CareerBicycleGoals",
    "CareerAerialGoals",
    "CareerFreezeSaves",
    "CareerOwnGoals",
    "CareerTurtleGoals",
    "CareerFewestBoostPickups",
    "CareerBreakoutDamages",
    "CareerBreakoutDamagesLarge",
    "CareerTimePlayed",
    "CareerLowFives",
    "CareerHoopsSwishGoals",
    "CareerBallHits",
    "CareerHighFives",
    "CareerBoostPickups",
    "CareerGames"
};

static const char * indexStringMapCareerChar[NUMCAREERSTATS] = {
    "First Touches",
    "Shots",
    "Goals",
    "Centers",
    "Hat Tricks",
    "Clears",
    "Wins",
    "MVPs",
    "Assists",
    "Demolitions",
    "Saves",
    "Losses",
    "Aerial Hits",
    "Epic Saves",
    "Long Goals",
    "Pool Shots",
    "Bicycle Hits",
    "Backwards Goals",
    "Juggle Hits",
    "Playmakers",
    "Overtime Goals",
    "Saviors",
    "Fewest Ball Touches",
    "Car Touches",
    "Fastest Goals",
    "Slowest Goals",
    "Furthest Goals",
    "Most Ball Touches",
    "Most Boost Pickups",
    "Exterminations",
    "Bicycle Goals",
    "Aerial Goals",
    "Freeze Saves",
    "Own Goals",
    "Turtle Goals",
    "Fewest Boost Pickups",
    "Breakout Damages",
    "Breakout Damages Large",
    "Time Played",
    "Low Fives",
    "Hoops Swish Goals",
    "Ball Hits",
    "High Fives",
    "Boost Pickups",
    "Games"
};

extern std::string indexStringMapRenderCareerTotal[NUMCAREERSTATS];
extern std::string indexStringMapRenderCareerPrivate[NUMCAREERSTATS];
extern std::string indexStringMapRenderCareerCasual[NUMCAREERSTATS];
extern std::string indexStringMapRenderCareerRanked[NUMCAREERSTATS];
extern std::string indexStringMapRenderCareerAverage[NUMCAREERSTATS];
