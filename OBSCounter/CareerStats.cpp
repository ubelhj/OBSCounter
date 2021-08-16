#include "pch.h"
#include "OBSCounter.h"

// calls all write functions at once
void OBSCounter::writeCareerStats() {
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
        careerStatTotal[i] = careerValue.private_ + careerValue.ranked + careerValue.unranked;

        // writes the private stat
        std::ofstream privateFile(fileLocation / "Career" / ("Private" + indexStringMapCareer[i] + ".txt"));
        privateFile << careerStatPrivate[i];
        privateFile.close();
        // writes the ranked stat
        std::ofstream rankedFile(fileLocation / "Career" / ("Ranked" + indexStringMapCareer[i] + ".txt"));
        rankedFile << careerStatRanked[i];
        rankedFile.close();
        // writes the casual stat
        std::ofstream casualFile(fileLocation / "Career" / ("Casual" + indexStringMapCareer[i] + ".txt"));
        casualFile << careerStatCasual[i];
        casualFile.close();
        // writes the total stat
        std::ofstream totalFile(fileLocation / "Career" / ("Total" + indexStringMapCareer[i] + ".txt"));
        totalFile << careerStatTotal[i];
        totalFile.close();

        i++;
    }
}