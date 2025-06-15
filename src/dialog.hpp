//
//  dialog.hpp
//  EVA_01
//
//  Created by Jan Everaert on 23/05/2025.
//
#pragma once

#include "ofMain.h"

class dialog : public ofBaseApp{
    public:
        void setup();
        void update();
        void draw(string question, string highlight, string subtext, string agree, string decline);
        void drawHighlightedText(string text, string highlightWord, float y, float maxWidth, ofTrueTypeFont &font);
    
    int accepted = -1;
    int accepted_touched = 0;
    int press_time = 12;
};
