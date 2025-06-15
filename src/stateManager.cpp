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
        toSend.setHex(0x00FF10); OSCManager::getInstance().sendColor(toSend);
    }
    if(newState == 11) {
        toSend.setHex(0xFF005D); OSCManager::getInstance().sendColor(toSend);
    }
    if(newState == 20) {
        toSend.setHex(0xff00FF); OSCManager::getInstance().sendColor(toSend);
    }
    if(newState == 30) {
        toSend.setHex(0xFF005D); OSCManager::getInstance().sendColor(toSend);
    }
    if(newState == 40) {
        toSend.setHex(0x00FFFF); OSCManager::getInstance().sendColor(toSend);
    }
    if(newState == 50) {        
        OSCManager::getInstance().sendCoin();
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

void StateManager::setCertainty(float newCertainty) {
    if(newCertainty > 90) {
        newCertainty = 95 - ofRandom(10);
    }
    certainty = newCertainty;
}


void StateManager::setEmpathy(float newempathy) {
    
    empathy += newempathy;
    
    if(empathy > 1) {
        empathy = 1;
    }
    if(empathy < 0) {
        empathy = 0;
    }
}
float StateManager::getEmpathy() const {
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
}
int StateManager::getNoPerson() const {
    return no_person;
}

void StateManager::setNumFaces(int num_faces) {
    if(num_faces == 0) {
        no_person++;
        if(no_person  > ofGetFrameRate() * 30 && currentState > 0) {
            resetNecessary = true;
            setState(00);
        }
    } else {
        if(currentState == 0) {
            currentPerson = floor(ofRandom(1000000));
            ofLog() << "reset person";
            setState(10);
        }
        if(no_person > ofGetFrameRate() * 10) {
            currentPerson = floor(ofRandom(1000000));
            ofLog() << "reset person";
            setState(10);
        }
        no_person = -1;
    }
}


void StateManager::reset_topic_focus() {
    for(int i= 0; i < topics.size(); i++) {
        topics[i].focus_time = 0;
    }
}
