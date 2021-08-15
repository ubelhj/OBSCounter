#include "pch.h"
#include "OBSCounter.h"
#include "Maps.h"

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

        i++;
    }
}