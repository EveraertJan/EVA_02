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

        ofColor blue = ofColor::fromHex(0x99F3FF);
        ofColor green = ofColor::fromHex(0x0AFF64);
    
        ofColor red = ofColor::fromHex(0xFF5C9A);
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
