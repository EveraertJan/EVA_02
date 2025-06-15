//
//  dialog.cpp
//  EVA_01
//
//  Created by Jan Everaert on 23/05/2025.
//

#include "dialog.hpp"
#include "stateManager.hpp"
#include "styleManager.hpp"


void dialog::setup() {
    logo.load("icons/logo_black.png");
}
void dialog::update() {
    
}
void dialog::draw(string question, string highlight,  string subtext, string agree, string decline, bool warning) {
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    if(warning) {
        ofSetColor(StyleManager::getInstance().red);
    } else {
        ofSetColor(StyleManager::getInstance().green);
    }
    ofDrawRectangle(-700, -450, 1400, 860);
    ofTranslate(-180, -400);
    ofSetColor(0);
    drawHighlightedText(question, "SELL", 80, 600, StyleManager::getInstance().bold_font);
    ofSetColor(255);
    StyleManager::getInstance().large_font.drawString(highlight, -300, 180);
    ofSetColor(0);
    drawHighlightedText(subtext, "", 300, 600, StyleManager::getInstance().mid_font);
    
    if(StateManager::getInstance().state_running > 10) {
        
        ofRectangle accept_area;
        if(agree.length() > 0) {
            ofSetColor(0);
            accept_area = ofRectangle(-300, 580, 290, 60);
            ofDrawRectangle(accept_area);
            accept_area.x += ofGetWidth()/2 - 180;
            accept_area.y += ofGetHeight()/2 - 400;
            ofSetColor(255);
            StyleManager::getInstance().bold_font.drawString(agree, -150  - StyleManager::getInstance().bold_font.stringWidth(agree)/2, 580 + 40);
        }
        ofRectangle decline_area;
        if(decline.length() > 0) {
            ofNoFill();
            ofSetColor(0);
            decline_area = ofRectangle(10, 580, 290, 60);
            ofDrawRectangle(decline_area);
            decline_area.x += ofGetWidth()/2 - 180;
            decline_area.y += ofGetHeight()/2 - 400;
            StyleManager::getInstance().bold_font.drawString(decline, 150  - StyleManager::getInstance().bold_font.stringWidth(decline)/2, 580 + 40);
        }
        
        if(ofGetMousePressed()) {
            int x = ofGetMouseX();
            int y = ofGetMouseY();
            int offset_x = -100;
            int offset_y = -200-400;
            int ymin = ofGetHeight()/2-240 + 520;
            
            
            if(accept_area.inside(x, y)) {
                accepted = 1;
            }
            if(decline_area.inside(x, y)) {
                accepted = 0;
            }
            
            
        } else {
            accepted_touched = 0;
        }
    }
    
    ofPopMatrix();
    
    
}



void dialog::drawHighlightedText(string text, string highlightWord, float y, float maxWidth, ofTrueTypeFont &font) {
    std::istringstream stream(text);
    string word, currentLine;
    float lineHeight = font.getLineHeight() * 0.95;
    vector<string> words;
    while (stream >> word) words.push_back(word);
    float xStart = -maxWidth / 2;
    float cursor = xStart;
    for (size_t i = 0; i < words.size(); i++) {
        if(words[i].find('//') != string::npos) {
            y += lineHeight;
            cursor = xStart;
        } else {
            string nextWord = words[i] + " ";
            float wordWidth = font.stringWidth(nextWord);
            if (cursor + wordWidth > xStart + maxWidth) {
                y += lineHeight;
                cursor = xStart;
            }
            
            if (words[i] == highlightWord) {
                float h = font.getLineHeight();
                ofSetColor(0);
                ofDrawRectangle(cursor - 10, y - h + font.getDescenderHeight() + 14, wordWidth, h);
                ofSetColor(255);
                font.drawString(nextWord, cursor, y);
            } else {
                ofSetColor(0);
                font.drawString(nextWord, cursor, y);
            }
            cursor += wordWidth;
        }
    }
}
