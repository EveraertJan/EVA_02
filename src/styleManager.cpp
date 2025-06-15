//
//  styleManager.cpp
//  EVA_01
//
//  Created by Jan Everaert on 15/06/2025.
//

#include "styleManager.hpp"

StyleManager& StyleManager::getInstance() {
    static StyleManager instance;
    return instance;
}


void StyleManager::setup() {
    ofTrueTypeFont::setGlobalDpi(72);
}

void StyleManager::drawStringHighlight(std::string content, int x, int y, ofTrueTypeFont & f, bool centered){
    ofRectangle bounding = f.getStringBoundingBox(content, x, y);
    ofPushStyle();
    ofFill();
    ofPushMatrix();
    if(centered) {
        ofTranslate(bounding.x-bounding.getWidth()/2, bounding.y);
    } else {
        ofTranslate(bounding.x, bounding.y);
        
    }
    ofSetColor(0);
    ofDrawRectangle(-10, -bounding.getHeight(), bounding.getWidth()+20, bounding.getHeight()*1.24);
    ofSetColor(255);
    f.drawString(content, 0, 0);
    ofPopMatrix();
    ofPopStyle();
}

