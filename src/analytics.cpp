#include "analytics.h"
#include "stateManager.hpp"
#include "styleManager.hpp"


void analytics::setup(){
    font.load("font/mono.ttf", 12);
    empathyFont.load("font/Input_Desktop/InputMono-Medium-Edu.ttf", 124);
    largeFont.load("font/dots.ttf", 36);
    font.setLetterSpacing(.9);

    logo.load("icons/logo_black_simple.png"); // first load image, assuming it's in bin/data folder
    startTime = ofGetCurrentTime();

    
    topics.push_back({"828w2zrrs1bgv36", "war", 0, 0});
    topics.push_back({"8tf81lyz0wf95g7", "pollution", 0, 0});
    topics.push_back({"46bsu1y0mhfvg8g", "flooding", 0, 0});
    topics.push_back({"5eofxxi10os7y64", "migration", 0, 0});
    topics.push_back({"2xia5v5pg0tv3b8", "poverty", 0, 0});
    topics.push_back({"u67379k9vu56d8n", "climate", 0, 0});
    topics.push_back({"20gaoy5np982eaz", "societal divide", 0, 0});
    topics.push_back({"9w14v34597n24rw", "none", 0, 0});
}

void analytics::update(){

}
void analytics::reset(){
    deduced = -1;
    for(int j = 0;  j < topics.size(); j++) {
        topics[j].totalweight = 0;
        topics[j].focus_time = 0;
        
    }
    startTime = ofGetCurrentTime();
}
void analytics::resetCounters()  {
    for(int j = 0;  j < topics.size(); j++) {
        topics[j].totalweight = 0;
        topics[j].focus_time = 0;
    }
}
void analytics::analyse(vector<post>* posts){
    resetCounters();
    float absoluteWeight = 0;
    for (auto& item : *posts) {
        for(int j = 0;  j < topics.size()-1; j++) {
            if(topics[j].name == item.topic) {
                topics[j].totalweight += item.time_watched/800;
                topics[j].totalweight += item.clicked * 15;
                
                absoluteWeight += topics[j].totalweight;
            }
        }
    }
    
    int most_id = 0;
    for(int j = 0;  j < topics.size(); j++) {
        if(topics[j].totalweight > topics[most_id].totalweight) {
            most_id = j;
        }
    }
    
    int second_most_id = 0;
    if(most_id == 0) { second_most_id = 1; }
    for(int j = 0;  j < topics.size(); j++) {
        if(topics[j].totalweight > topics[second_most_id].totalweight && j != most_id) {
            second_most_id = j;
        }
    }
    if(topics[most_id].totalweight > min_watch_time) {
        ofLog() << "topic deduced " << topics[most_id].handle << " " << topics[most_id].totalweight;
        ofLog() << "second deduced " << topics[second_most_id].handle << " " << topics[second_most_id].totalweight;
        StateManager::getInstance().setDeduced(most_id);
        deduced = most_id;
    }
    
    StateManager::getInstance().setCertainty(ofMap(topics[second_most_id].totalweight, 0, topics[most_id].totalweight, 90, 50));
    
    for(int i = 0; i < topics.size(); i++) {
        StateManager::getInstance().topics[i].totalweight = topics[i].totalweight;
    }
    
}

void analytics::detect_empathy(vector<post>* posts, bool triggered){
    resetCounters();
    int deduced = StateManager::getInstance().getDeduced();

    for (auto& item : *posts) {
        if(topics[deduced].name == item.topic) {
            topics[deduced].focus_time += item.focus_time;
        }
        if(topics[topics.size()-1].name == item.topic) {
            topics[topics.size()-1].focus_time += item.focus_time;
        }
    }

    for(int i = 0; i < topics.size(); i++) {
        StateManager::getInstance().topics[i].focus_time = topics[i].focus_time;
    }
    int total_watch_time = topics[deduced].focus_time + topics[topics.size()-1].focus_time;
    
    if(total_watch_time > 10) {
        float difference = (float)topics[deduced].focus_time / ((float)topics[topics.size()-1].focus_time + 0.0001);
        if(difference > 1) {
            difference = 1;
        }
        
        
        ofLog() <<(difference - 0.5) * 0.02 << " on " << StateManager::getInstance().getEmpathy();
        StateManager::getInstance().setEmpathy((difference - 0.5) * 0.02);
    }
    if(triggered) {
       
        if(total_watch_time < 5) {
            StateManager::getInstance().click_through += 1;
        } else {
            StateManager::getInstance().click_through -= 1;
        }
    }
    
    
}

void analytics::draw(){
  ofPushMatrix();
  ofTranslate(0, ofGetHeight() - 170);
  ofSetColor(255);
  ofDrawRectangle(0, -30, ofGetWidth(), 200);

  ofSetColor(255);
  logo.draw(65, 13, 80, 80);

  ofSetColor(200);
  font.drawString("This machine is not to be used by minors or people with disailities. Contact your local administration for more information.", 15, 160);

  drawDatums();
//  drawEmpathyMeasure();
  drawTiming();
  ofPopMatrix();
}


void analytics::drawDatums() {
  ofPushMatrix();
  ofTranslate(ofGetWidth() -50-200+2, 26);
  ofSetColor(50);
//    ofSetColor(StyleManager::getInstance().green);
  font.drawString("ID", 0,0);

  
  font.drawString("TIME", 0,20);
  font.drawString("DATE", 0,40);
  font.drawString("MACHINE", 0,60);


  ofSetColor(0);
  font.drawString(ofToString(StateManager::getInstance().currentPerson), 220 - font.getStringBoundingBox("000000001", 0, 0).width,0);
    
    ofTime t = ofGetCurrentTime();
    std::string timeString = ofToString(ofGetHours()) + ":" + ofToString(ofGetMinutes()) + ":" + ofToString(ofGetSeconds());
  font.drawString(timeString, 220 - font.getStringBoundingBox(timeString, 0, 0).width,20);
    std::string dateString = ofToString(ofGetDay()) + ".0" + ofToString(ofGetMonth()) + "." + ofToString(ofGetYear());
    
  font.drawString(dateString, 224 - font.getStringBoundingBox(dateString, 0, 0).width,40);
  font.drawString("BXL0085", 220 - font.getStringBoundingBox("BXL0085", 0, 0).width ,60);
  ofPopMatrix();
}

void analytics::drawEmpathyBold() {
    int x = 0;
    int y = 0;
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(ofGetWidth() - 180, 100);
    
    ofRotateDeg(90);
    
    ofRectangle e_frame = empathyFont.getStringBoundingBox("EMPATHY MEASURE", x, y);
    ofFill();
    ofSetColor(StyleManager::getInstance().green);
    ofDrawRectangle(e_frame.x, e_frame.y, e_frame.getWidth() * ofMap(StateManager::getInstance().getEmpathy(), 0.2, 1, 0, 1), e_frame.getHeight());
    ofSetColor(0, 0, 0);
    empathyFont.drawString("EMPATHY MEASURE", x, y);
    ofPopStyle();
    ofPopMatrix();
}
void analytics::drawEmpathyMeasure() {
  ofPushMatrix();
  ofPushStyle();

  ofTranslate(552, 2);
  ofFill();
  

  float radius = 42;
  ofVec2f center(50, 50);

  ofSetColor(200);
  for (float a = 0; a <= 270; a += 5.0) {
    float rad = ofDegToRad(a - 90); // Offset to start at top-left
    float x = center.x + radius * cos(rad);
    float y = center.y + radius * sin(rad);
    ofDrawCircle(x, y, 5);
  }

  ofSetColor(0);
  float angle = ofMap( StateManager::getInstance().getEmpathy() * 100, 0, 100, 0, 270);
  for (float a = 0; a <= angle; a += 5.0) {
    float rad = ofDegToRad(a - 90); // Offset to start at top-left
    float x = center.x + radius * cos(rad);
    float y = center.y + radius * sin(rad);
    ofDrawCircle(x, y, 5);
  }

  font.drawString("Empathy", center.x-25, center.y+2);
  ofPopStyle();
  ofPopMatrix();
}

void analytics::drawTiming() {
  ofPushMatrix();
  ofPushStyle();

  ofTranslate(252, 2);
    
    
  double time_passed = ofGetCurrentTime().getAsMilliseconds() - startTime.getAsMilliseconds();
    int hours = floor((ofGetCurrentTime().seconds - startTime.seconds) / 3600);
    int minutes = floor((ofGetCurrentTime().seconds - startTime.seconds) / 60);
    int seconds =(ofGetCurrentTime().seconds - startTime.seconds) % 60;
    std::string hourStr = (hours >= 10) ? ofToString(hours) : "0" + ofToString(hours);
    std::string minStr = (minutes >= 10) ? ofToString(minutes) : "0" + ofToString(minutes);
    std::string secStr = (seconds >= 10) ? ofToString(seconds) : "0" + ofToString(seconds);
    
    std::string passed_string = hourStr + ":" + minStr + ":" + secStr;
  largeFont.drawString(ofToString(passed_string), 0, 42);
  font.drawString("current time", 97, 60);
  font.drawString("avg: 01:20:00", 90, 100);

  ofEnableAlphaBlending();
  ofSetColor(0,0,0,40);
  ofFill();
  ofSetLineWidth(0);
  ofDrawRectangle(0, 65, 200, 18);
  ofDrawRectangle(70, 65, 130, 18);
  ofDrawRectangle(90, 65, 110, 18);
  ofDrawRectangle(130, 65, 70, 18);
  ofDisableAlphaBlending();
  ofPopStyle();
  ofPopMatrix();
}
