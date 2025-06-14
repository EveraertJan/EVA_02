//
//  StatisticsManager.cpp
//  EVA_01
//
//  Created by Jan Everaert on 14/06/2025.
//

#include "statisticsManager.hpp"

StatisticsManager& StatisticsManager::getInstance() {
    static StatisticsManager instance;
    return instance;
}
//
//void StatisticsManager::setState(int newState) {
////    no_person = newNoPerson;
////    if(no_person > ofGetFrameRate() * 30) {
////        resetNecessary = true;
////        currentState = 0;
////    }
//}
//int StatisticsManager::getState()const {
//    return 0;
//}
//


void StatisticsManager::reset(){
    
    
    posts_watched_analysis=0;
    posts_watched_enforcement=0;
    empathy_history.clear();
    click_throughs = 0;
    clicks = 0;
    reason = "";
    looking_away = 0;
    
}
