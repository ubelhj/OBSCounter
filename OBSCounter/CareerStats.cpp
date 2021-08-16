#include "pch.h"
#include "OBSCounter.h"

// calls all write functions at once
void OBSCounter::writeCareerStats() {
    auto careerStats = CareerStatsWrapper::GetStatValues();

    bool validStats = true;
    if (careerStats.size() != NUMCAREERSTATS) {
        LOG("bad career stats values. DM JerryTheBee");
        validStats = false;
    }

    int i = 0;
    for (CareerStatsWrapper::StatValue careerValue : careerStats) {
        //cvarManager->log(careerValue.stat_name);
        if (!validStats) {
            LOG("{} private {}, ranked {}, casual {}", 
                careerValue.stat_name, careerValue.private_, careerValue.ranked, careerValue.unranked);
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