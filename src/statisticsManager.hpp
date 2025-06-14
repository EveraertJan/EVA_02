//
//  stats
//  EVA_01
//
//  Created by Jan Everaert on 14/06/2025.
//
#pragma once
#include "ofMain.h"


class StatisticsManager {
    public:
        static StatisticsManager& getInstance();

//        void setState(int newState);
//        int getState() const;
//    
        void reset();
    
    
        int posts_watched_analysis;
        int posts_watched_enforcement;
        std::vector<double> empathy_history;
        int click_throughs;
        int clicks;
        std::string reason;
        int looking_away;
    private:
        StatisticsManager() = default;
        StatisticsManager(const StatisticsManager&) = delete;
        StatisticsManager& operator=(const StatisticsManager&) = delete;

    
};
