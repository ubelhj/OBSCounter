#include "pch.h"
#include "OBSCounter.h"

// The structure of a ticker stat event
struct TickerStruct {
    // person who got a stat
    uintptr_t Receiver;
    // person who is victim of a stat (only exists for demos afaik)
    uintptr_t Victim;
    // wrapper for the stat event
    uintptr_t StatEvent;
};

// structure of a stat event
struct StatEventStruct {
    uintptr_t PRI;
    uintptr_t StatEvent;
    // Count always is int_max. No idea why
    uintptr_t Count;
};

void OBSCounter::statEvent(ServerWrapper caller, void* args) {
    //DEBUGLOG("stat event!");
    if (!gameWrapper->IsInOnlineGame()) {
        //DEBUGLOG("not in online game");
        return;
    }
    auto tArgs = (StatEventStruct*)args;

    auto statEvent = StatEventWrapper(tArgs->StatEvent);
    std::string eventString = statEvent.GetEventName();

    // PRI is always main player for this event
    /*auto pri = PriWrapper(tArgs->PRI);
    DEBUGLOG("is primary: " + std::to_string(pri.IsLocalPlayerPRI()));*/

    // Count always is int_max. No idea why
    /*auto count = tArgs->Count;
    DEBUGLOG("count: " + std::to_string(count));*/

    auto eventTypePtr = eventDictionary.find(eventString);

    int eventType;

    if (eventTypePtr != eventDictionary.end()) {
        eventType = eventTypePtr->second;
        DEBUGLOG("event type: " + eventString);
        DEBUGLOG("event num: {}", eventType);
    } else {
        LOG("missing stat: " + eventString);
        DEBUGLOG("missing stat points: " + std::to_string(statEvent.GetPoints()));
        return;
    }

    int statPoints = statEvent.GetPoints();
    DEBUGLOG("points: " + std::to_string(statPoints));
    if (statPoints > 0) {
        statArray[points] += statPoints;
        statArrayGame[points] += statPoints;
        write(points);
    }
    statArray[eventType]++;
    // adds 1 to the game stat equivalent if applicable
    if (eventType > STATSWITHOUTGAME) {
        statArrayGame[eventType]++;
    }

    // any extra stats needed with more computing
    // shooting % (shot or goal)
    // k/d (demo or death)
    // missed exterm % (demo or exterm)
    // wins / losses (win or loss)
    switch (eventType) {
    case shots:
    case goals:
        writeShootingPercentage();
        break;
    case demos:
        writeKillPercentage();
        __fallthrough;
    case exterms:
        writeMissedExterms();
        break;
    case deaths:
        writeKillPercentage();
        break;
    case wins:
    case losses:
        writeWinPercentage();
        break;
    case saves:
    case epicSaves:
        statArray[totalSaves]++;
        statArrayGame[totalSaves]++;
        write(totalSaves);
        break;
    }

    write(eventType);
}

void OBSCounter::statTickerEvent(ServerWrapper caller, void* args) {
    auto tArgs = (TickerStruct*)args;
    //LOG("stat ticker event!");
    if (!gameWrapper->IsInOnlineGame()) {
        //LOG("not in online game");
        return;
    }

    // separates the parts of the stat event args
    auto receiver = PriWrapper(tArgs->Receiver);
    auto victim = PriWrapper(tArgs->Victim);
    auto statEvent = StatEventWrapper(tArgs->StatEvent);
    // name of the stat as shown in rocket league 
    //  (Demolition, Extermination, etc.)
    auto eventString = statEvent.GetEventName();

    auto eventTypePtr = eventDictionary.find(eventString);

    int eventType;

    if (eventTypePtr != eventDictionary.end()) {
        eventType = eventTypePtr->second;
        DEBUGLOG("event type: " + eventString);
        DEBUGLOG("event num: {}", eventType);
    }
    else {
        LOG("missing stat: " + eventString);
        DEBUGLOG("missing stat points: {}",  statEvent.GetPoints());
        return;
    }

    // team stats
    ServerWrapper sw = gameWrapper->GetOnlineGame();
    if (!sw) {
        DEBUGLOG("null server");
        return;
    }
    PlayerControllerWrapper primary = sw.GetLocalPrimaryPlayer();
    if (!primary) {
        DEBUGLOG("null primary player");
        return;
    }

    unsigned char playerTeamNum = primary.GetTeamNum2();
    if (playerTeamNum > 1) return;

    unsigned char recieverTeamNum = receiver.GetTeamNum2();
    bool sameTeam = playerTeamNum == recieverTeamNum;

    if (sameTeam) {
        statArrayTeam[eventType]++;
        writeSpecific(eventType, STAT_TEAM);
    } else {
        statArrayOpponent[eventType]++;
        writeSpecific(eventType, STAT_TEAM_OPPONENT);
    }

    switch (eventType) {
    case saves:
    case epicSaves:
        sameTeam ?
            statArrayTeam[totalSaves]++ :
            statArrayOpponent[totalSaves]++;
        write(totalSaves);
        break;
    case demos:
        bool sameTeamDead = playerTeamNum == victim.GetTeamNum2();
        sameTeamDead ?
            statArrayTeam[deaths]++ :
            statArrayOpponent[deaths]++;

        if (isPrimaryPlayer(victim)) {
            DEBUGLOG("player died");
            statArray[deaths]++;
            statArrayGame[deaths]++;
            writeKillPercentage();
            return;
        }

        write(deaths);
        break;
    }
}

// checks if the player who received a stat is the user's player
bool OBSCounter::isPrimaryPlayer(PriWrapper receiver) {
    // anything down this conversion line can be null, so always checks
    if (!gameWrapper->IsInOnlineGame()) {
        DEBUGLOG("not in online game");
        return false;
    }

    ServerWrapper sw = gameWrapper->GetOnlineGame();

    if (sw.IsNull()) {
        DEBUGLOG("null server");
        return false;
    }

    auto primary = sw.GetLocalPrimaryPlayer();

    if (primary.IsNull()) {
        DEBUGLOG("null primary player");
        return false;
    }

    auto primaryPri = primary.GetPRI();

    if (primaryPri.IsNull()) {
        DEBUGLOG("null primary pri");
        return false;
    }

    if (receiver.IsNull()) {
        DEBUGLOG("null receiver");
        return false;
    }

    auto receiverID = receiver.GetUniqueIdWrapper();
    auto primaryID = primaryPri.GetUniqueIdWrapper();

    return receiverID.GetUID() == primaryID.GetUID();
}

// called whenever a new game begins, and resets all game stats to 0
void OBSCounter::startGame() {
    DEBUGLOG("started game");
    // if the last game didnt end, doesn't start game
    // can lead to issues if the user ragequits. No good workaround yet
    if (!endedGame) {
        DEBUGLOG("last game didn't end");
        return;
    }
    endedGame = false;

    statArray[games]++;

    // resets all game stats to 0 
    for (int i = 0; i < NUMSTATS; i++) {
        statArrayGame[i] = 0;
    }

    writeAll();
}

void OBSCounter::endGame() {
    if (!gameWrapper->IsInOnlineGame()) {
        DEBUGLOG("not in online game");
        return;
    }

    ServerWrapper sw = gameWrapper->GetOnlineGame();

    if (sw.IsNull()) {
        DEBUGLOG("null server");
        return;
    }

    auto primary = sw.GetLocalPrimaryPlayer();

    if (primary.IsNull()) {
        DEBUGLOG("null primary player");
        return;
    }

    auto primaryNum = primary.GetTeamNum2();

    auto winner = sw.GetWinningTeam();

    if (winner.IsNull()) {
        DEBUGLOG("null winner");
        return;
    }

    auto winningNum = winner.GetTeamNum2();

    if (winningNum != primaryNum) {
        statArray[losses]++;
        write(losses);
    }

    PriWrapper pri = primary.GetPRI();

    if (!pri) {
        DEBUGLOG("null player pri");
        return;
    }

    int truePoints = pri.GetMatchScore();

    int diff = truePoints - statArrayGame[points];

    // accounts for too many or too little points
    if (diff != 0) {
        statArray[points] += diff;
        statArrayGame[points] += diff;
        write(points);
    }

    endedGame = true;
}

// called every second to log car location on offense or defense
// also makes sure points are synced
void OBSCounter::checkCarLocation() {
    //DEBUGLOG("updated time");
    if (!gameWrapper->IsInOnlineGame()) {
        // DEBUGLOG("not in online game");
        return;
    }

    ServerWrapper sw = gameWrapper->GetOnlineGame();

    if (sw.IsNull()) {
        //DEBUGLOG("null server");
        return;
    }

    auto car = gameWrapper->GetLocalCar();

    if (car.IsNull()) {
        //DEBUGLOG("null car");
        return;
    }

    if (car.GetCollisionComponent().IsNull()) {
        return;
    }

    PriWrapper pri = car.GetPRI();

    if (pri) {
        // checks that points are accurate
        int truePoints = pri.GetMatchScore();

        int diff = truePoints - statArrayGame[points];

        // accounts for too many or too little points
        if (diff != 0) {
            statArray[points] += diff;
            statArrayGame[points] += diff;
            write(points);
        }
    }

    auto loc = car.GetLocation();

    auto team = car.GetTeamNum2();
    // offense for team 0 is y > 0
    // offense for team 1 is y < 0

    // inverts team 1 so both have the same offense / defense
    if (team) {
        loc = loc * -1;
    }

    if (loc.Y > 0) {
        statArray[offenseTime]++;
        statArrayGame[offenseTime]++;
        writeTimeStat(offenseTime);
    }
    else {
        statArray[defenseTime]++;
        statArrayGame[defenseTime]++;
        writeTimeStat(defenseTime);
    }
    statArray[timePlayed]++;
    statArrayGame[timePlayed]++;
    writeTimeStat(timePlayed);
}