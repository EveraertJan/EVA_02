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
    state_running = 0;
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
        OSCManager::getInstance().sendCoin();
        toSend.setHex(0x00FFFF); 
        OSCManager::getInstance().sendColor(toSend);
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
//    if(newempathy == -1) {
//        // reset
//        empathy_history.clear();
//        for(int i = 0; i < 30; i++) {
//            empathy_history.push_back(1);
//        }
//    }
//    empathy_history.push_back(newempathy);
//    if(empathy_history.size() > history_size) {
//        empathy_history.erase(empathy_history.begin());
//    }
    
    empathy += newempathy;
    
    if(empathy > 1) {
        empathy = 1;
    }
    if(empathy < 0) {
        empathy = 0;
    }
}
float StateManager::getEmpathy() const {
//    float tot = 0;
//    for(auto & h : empathy_history) {
//        tot  += h;
//    }
//    return tot / empathy_history.size();
    return empathy;
}

void StateManager::setNoPerson(int newNoPerson) {
    if(newNoPerson == 1) {
        ofDrawBitmapStringHighlight("no person detected", ofVec2f(ofGetWidth()/2-50, ofGetHeight()-100));
    }
    if(no_person != -1) {
        no_person += newNoPerson;
        if(no_person > ofGetFrameRate() * 30) {
            resetNecessary = true;
            ofLog() << "no person for more than 30";
            currentState = 0;
            no_person = -1;
        }
        
    }
    if(newNoPerson == 0 && (no_person == -1 || no_person > 5)) {
        no_person = 0;
        currentPerson = floor(ofRandom(1000000));
        currentState = 10;
    }
    if(newNoPerson == 0 && currentState == 0 ) {
        no_person = 0;
        currentPerson = floor(ofRandom(1000000));
        currentState = 10;
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
