//
//  styleManager.hpp
//  EVA_01
//
//  Created by Jan Everaert on 15/06/2025.
//

#pragma once
#include "ofMain.h"


class StyleManager {
    public:
        static StyleManager& getInstance();
        void setup();
    
        void drawStringHighlight(std::string content, int x, int y, ofTrueTypeFont & f, bool centered);
    
        ofColor blue = ofColor::fromHex(0x33e7ff);
        ofColor green = ofColor::fromHex(0x00FF64);
    
        ofColor red = ofColor::fromHex(0xFF385D);
        ofColor oldBlue = ofColor(194, 253, 80);
    
    
        ofTrueTypeFont base_font;
        ofTrueTypeFont debug_font;
        ofTrueTypeFont large_font;
        ofTrueTypeFont bold_font;
        ofTrueTypeFont state_font;
        ofTrueTypeFont mid_font;
        
    private:
        StyleManager() = default;
        StyleManager(const StyleManager&) = delete;
        StyleManager& operator=(const StyleManager&) = delete;

    
};

// ofColor blue = ofColor(65, 144, 201);
// ofColor green = ofColor(171, 203, 67).setHex(0x88ff7c);
// ofColor red = ofColor(255, 0, 0).setHex(0xfa0060);
// ofColor green = ofColor::fromHex(0x88ff7c); // blueish green
