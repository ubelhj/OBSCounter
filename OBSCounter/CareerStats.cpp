#include "pch.h"
#include "OBSCounter.h"

// calls all write functions at once
void OBSCounter::writeCareerStats() {
    gameWrapper->Execute([this](...) mutable {
        writeCareerStatsWrapped();
        });
}

void OBSCounter::writeCareerStatsWrapped() {
    auto careerStats = CareerStatsWrapper::GetStatValues();

    for (CareerStatsWrapper::StatValue careerValue : careerStats) {
        auto eventTypePtr = eventDictionaryCareer.find(careerValue.stat_name);

        int i;

        if (eventTypePtr != eventDictionaryCareer.end()) {
            i = eventTypePtr->second;
        } else {
            cvarManager->log("missing stat: " + careerValue.stat_name);
            continue;
        }

        careerStatPrivate[i] = careerValue.private_;
        careerStatRanked[i] = careerValue.ranked;
        careerStatCasual[i] = careerValue.unranked;
        careerStatTotal[i] = careerValue.private_ + careerValue.ranked + careerValue.unranked + careerStatTotalOffset[i];

        // writes the private stat
        writeFile(fileLocation / "Career" / ("Private" + statStringsCareer[i] + ".txt"), careerStatPrivate[i]);
        // writes the ranked stat
        writeFile(fileLocation / "Career" / ("Ranked" + statStringsCareer[i] + ".txt"), careerStatRanked[i]);
        // writes the casual stat
        writeFile(fileLocation / "Career" / ("Casual" + statStringsCareer[i] + ".txt"), careerStatCasual[i]);
        // writes the total stat
        writeFile(fileLocation / "Career" / ("Total" + statStringsCareer[i] + ".txt"), careerStatTotal[i]);

    }

    // special case for compound stat Game
    careerStatPrivate[Game] = careerStatPrivate[Win] + careerStatPrivate[Loss];
    careerStatRanked[Game] = careerStatRanked[Win] + careerStatRanked[Loss];
    careerStatCasual[Game] = careerStatCasual[Win] + careerStatCasual[Loss];
    careerStatTotal[Game] = careerStatTotal[Win] + careerStatTotal[Loss] + careerStatTotalOffset[Game];

    // writes the private stat
    writeFile(fileLocation / "Career" / ("Private" + statStringsCareer[Game] + ".txt"), careerStatPrivate[Game]);
    writeFile(fileLocation / "Career" / ("Ranked" + statStringsCareer[Game] + ".txt"), careerStatRanked[Game]);
    writeFile(fileLocation / "Career" / ("Casual" + statStringsCareer[Game] + ".txt"), careerStatCasual[Game]);
    writeFile(fileLocation / "Career" / ("Total" + statStringsCareer[Game] + ".txt"), careerStatTotal[Game]);

    // second loop for averages to ensure wins and losses were correct
    for (CareerStatsWrapper::StatValue careerValue : careerStats) {
        auto eventTypePtr = eventDictionaryCareer.find(careerValue.stat_name);

        int i;

        if (eventTypePtr != eventDictionaryCareer.end()) {
            i = eventTypePtr->second;
        }
        else {
            cvarManager->log("missing stat: " + careerValue.stat_name);
            continue;
        }

        careerStatAverage[i] = divide(careerStatTotal[i], careerStatTotal[Game]);

        writeFileAverage(fileLocation / "Career" / ("Average" + statStringsCareer[i] + ".txt"), careerStatAverage[i]);
    }
}