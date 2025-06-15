//
//  OSCManager.cpp
//  EVA_01
//
//  Created by Jan Everaert on 04/06/2025.
//

#include "OSCManager.h"

OSCManager& OSCManager::getInstance() {
    static OSCManager instance;
    return instance;
}

void OSCManager::setup() {

    sender.setup("192.168.0.196", 9000);
    ofLog() << "setting up the OSC";
    test();
}


void OSCManager::draw() {
}
void OSCManager::sendColor(ofColor c) {
    try {
        ofxOscMessage message;
        message.setAddress("/LED");
        message.addIntArg(c.r);
        message.addIntArg(c.g);
        message.addIntArg(c.b);
        sender.sendMessage(message);
    } catch (std::exception exc) {
        ofLog() << "cannot send color yet";
        setup();
    }
}
void OSCManager::sendCoin() {
    try{
        ofxOscMessage message;
        message.setAddress("/sending_coin");
        message.addIntArg(255);
        sender.sendMessage(message);
    } catch (std::exception exc) {
        ofLog() << "cannot send coin yet";
        setup();
    }
}
void OSCManager::test() {
    ofColor test = 0xf905ff;
    sendColor(test);
    sendCoin();
}


