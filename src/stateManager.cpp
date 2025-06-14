//
//  stateManager.cpp
//  EVA_01
//
//  Created by Jan Everaert on 07/06/2025.
//

#include "stateManager.hpp"
#include "OSCManager.h"

StateManager& StateManager::getInstance() {
    static StateManager instance;
    return instance;
}

void StateManager::setState(int newState) {
    currentState = newState;
    
    ofColor toSend;
    if(newState == 0) {
        toSend.setHex(0xFFFFFF); OSCManager::getInstance().sendColor(toSend);
    }
    if(newState == 10) {
        toSend.setHex(0xFF0000); OSCManager::getInstance().sendColor(toSend);
    }
    if(newState == 20) {
        toSend.setHex(0xFFFF00); OSCManager::getInstance().sendColor(toSend);
    }
    if(newState == 30) {
        toSend.setHex(0x00FF00); OSCManager::getInstance().sendColor(toSend);
    }
    if(newState == 40) {
        toSend.setHex(0xFF00FF); OSCManager::getInstance().sendColor(toSend);
    }
    if(newState == 50) {
        toSend.setHex(0x00FFFF); OSCManager::getInstance().sendColor(toSend);
        OSCManager::getInstance().sendCoin();
    }
    if(newState == 60) {
        toSend.setHex(0xFFFFFF); OSCManager::getInstance().sendColor(toSend);
    }
}

int StateManager::getState() const {
    return currentState;
}

void StateManager::setDeduced(int newDeduced) {
    deduced = newDeduced;
}
int StateManager::getDeduced() const {
    return deduced;
}

void StateManager::setEmpathy(float newempathy) {
    if(newempathy == -1) {
        // reset
        empathy_history.clear();
        for(int i = 0; i < 20; i++) {
            empathy_history.push_back(1);
        }
    }
    empathy_history.push_back(newempathy);
    if(empathy_history.size() > history_size) {
        empathy_history.erase(empathy_history.begin());
    }
}
float StateManager::getEmpathy() const {
    float tot = 0;
    for(auto & h : empathy_history) {
        tot  += h;
    }
    return tot / empathy_history.size();
}

void StateManager::setNoPerson(int newNoPerson) {
    no_person = newNoPerson;
    if(no_person > ofGetFrameRate() * 5) {
        resetNecessary = true;
        currentState = 0;
    }
}
int StateManager::getNoPerson() const {
    return no_person;
}



void StateManager::reset_topic_focus() {
    for(int i= 0; i < topics.size(); i++) {
        topics[i].focus_time = 0;
    }
}
