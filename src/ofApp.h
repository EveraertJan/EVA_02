#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "ofxGui.h"
#include "analytics.h"
#include "ephemeral_feed.h"
#include "dialog.hpp"
// #include "eyetrack.h"
#include "mediapipeTracker.h"

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void noiseField();
    void drawState(string state_message);
    void reset();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    int getAverage(std::vector<int> in);
    
    void inputSourceChanged(int &inputSource);
    void loadFromAPI();
    
    
    ofImage img;
    ofxCvHaarFinder eye_finder;
    ofxCvHaarFinder face_finder;
    ofxCvGrayscaleImage grayImg;
    ofxCvContourFinder contourFinder;
    
    ofVideoGrabber vidGrabber;
    ofTexture videoTexture;
    
    ofVideoPlayer movie;
    
    ofPixels framePixels;
    ofImage frameImage;
    
    int camWidth;
    int camHeight;
    
    std::vector<int> x_avgs;
    std::vector<int> y_avgs;
    std::vector<int> w_avgs;
    std::vector<int> h_avgs;
    
    ofImage logo;
    
    ofVec2f look_at;
    
    
    ofxToggle debug;
    ofxToggle debug_cv;
    ofxToggle debug_sound;
    ofxToggle live_input;
    ofxToggle enable_cv;
    ofxIntSlider treshold;
    ofxIntSlider sample_size;
    ofxIntSlider click_time;
    ofxIntSlider state;
    ofxIntSlider input_source;
    ofxIntSlider offset_x;
    ofxIntSlider offset_y;
    ofxFloatSlider scale_x;
    ofxFloatSlider scale_y;
    
    
    ofxIntSlider history_size;
    
    ofxFloatSlider variance_threshold;
    ofxPanel gui;
    bool bHide;
    
    
    analytics analytics_block;
    // eyetrack eyetracker;
    ephemeral_feed feed;
    mediapipeTracker mediapipe;
    dialog consent_transaction;
    dialog consent_content;
    dialog ack_topic_found;
    dialog ack_complete;
    
    vector<post> base_posts;

    std::string error = "";
    
    int last_touch;
    int time_passed_since_last;
};
