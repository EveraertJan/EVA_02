//
//  OSCManager.h
//  EVA_01
//
//  Created by Jan Everaert on 04/06/2025.
//
#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

class OSCManager : public ofBaseApp{
    public:
        static OSCManager& getInstance();
        void setup();
        void draw();
        void test();
        void sendColor(ofColor c);
        void sendCoin();
        
        ofTrueTypeFont mono_bold;
        
        ofxOscReceiver receiver;
        ofxOscSender sender;
    
    
    private:
        OSCManager() = default;
        OSCManager(const OSCManager&) = delete;
        OSCManager& operator=(const OSCManager&) = delete;

};
