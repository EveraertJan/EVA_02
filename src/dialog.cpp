//
//  dialog.cpp
//  EVA_01
//
//  Created by Jan Everaert on 23/05/2025.
//

#include "dialog.hpp"

void dialog::setup() {
    
    font.load("font/mono2.ttf", 16);
    font.setLetterSpacing(.9);
    largeFont.load("font/inputmono.ttf", 68);
    mono_bold.load("font/inputmono.ttf", 24);
}
void dialog::update() {
    
}
void dialog::draw(string question, string highlight,  string subtext, string agree, string decline) {
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2-240);
    ofSetColor(195, 247, 40);
    ofDrawRectangle(-320, 0, 640, 600);
    ofSetColor(0);
    drawHighlightedText(question, "SELL", 80, 600, mono_bold);
    largeFont.drawString(highlight, -largeFont.stringWidth(highlight)/2, 200);
    drawHighlightedText(subtext, "", 300, 600, font);
    
    if(agree.length() > 0) {
        ofSetColor(0);
        ofDrawRectangle(-300, 520, 290, 60);
        ofSetColor(255);
        mono_bold.drawString(agree, -150  - mono_bold.stringWidth(agree)/2, 520 + 40);
    }
    if(decline.length() > 0) {
        ofNoFill();
        ofSetColor(0);
        ofDrawRectangle(10, 520, 290, 60);
        mono_bold.drawString(decline, 150  - mono_bold.stringWidth(decline)/2, 520 + 40);
    }
    
    ofPopMatrix();
    
    if(ofGetMousePressed()) {
        accepted_touched += 1;
        if(accepted_touched > ofGetFrameRate() /2){
            int x = ofGetMouseX();
            int y = ofGetMouseY();
            int ymin = ofGetHeight()/2-240 + 520;
            if(y > ymin && y < ymin + 60) {
                if(x < ofGetWidth()/2 && x > ofGetWidth()/2 - 370) {
                    accepted = 1;
                }
                if(x > ofGetWidth()/2 && x < ofGetWidth()/2 + 370) {
                    accepted = 0;
                }
            }
        }
    } else {
        accepted_touched = 0;
    }
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
