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
    
    test();
}


void OSCManager::draw() {
}
void OSCManager::sendColor(ofColor c) {
    ofxOscMessage message;
    message.setAddress("/LED");
    ofLog() << "color" << c.r << "," << c.g << "," << c.b;
    message.addIntArg(c.r);
    message.addIntArg(c.g);
    message.addIntArg(c.b);
    sender.sendMessage(message);
    ofLog() << "trigger LED";
}
void OSCManager::sendCoin() {
    ofxOscMessage message;
    message.setAddress("/sending_coin");
    message.addIntArg(255);
    sender.sendMessage(message);
    ofLog() << "trigger coin";
}
void OSCManager::test() {
    ofColor test = 0xf905ff;
    sendColor(test);
    sendCoin();
    ofLog() << "trigger TEST";
}


