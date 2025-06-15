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

        ofColor blue = ofColor::fromHex(0x00e0ff);
        ofColor green = ofColor::fromHex(0x18ff6d);
    
        ofColor red = ofColor::fromHex(0xfa0060);
        ofColor oldBlue = ofColor(194, 253, 80);
    private:
        StyleManager() = default;
        StyleManager(const StyleManager&) = delete;
        StyleManager& operator=(const StyleManager&) = delete;

    
};

// ofColor blue = ofColor(65, 144, 201);
// ofColor green = ofColor(171, 203, 67).setHex(0x88ff7c);
// ofColor red = ofColor(255, 0, 0).setHex(0xfa0060);
// ofColor green = ofColor::fromHex(0x88ff7c); // blueish green
