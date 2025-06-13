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
    mono_bold.load("font/inputmono.ttf", 12);
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
    message.setAddress("/coin");
    message.addIntArg(255);
    sender.sendMessage(message);
    ofLog() << "trigger coin";
}
void OSCManager::test() {
    ofxOscMessage message;
    message.setAddress("/LED");
    ofColor test = 0xf905ff;
    
    message.addIntArg(test.r);
    message.addIntArg(test.g);
    message.addIntArg(test.b);
    sender.sendMessage(message);
    ofLog() << "trigger LED";
}


