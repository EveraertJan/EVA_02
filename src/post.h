#pragma once

#include "ofMain.h"
#include "ofxBlur.h"

class post : public ofBaseApp{
	public:
		void setup(string path, string topic, int id, string mask_path, int r, string mask_d);
		void update();
		void draw(ofVec3f look_at, float zoom, int focus_id, bool masked);
        void loadMask();
    
        ofxBlur blur;
    
        ofImage img;
        ofImage mask;
        ofImage gs_img;
        ofImage thumb;
        ofImage like;

        string mask_str;
        string path_in;
        string topic;
        int topic_id;
        int post_id;
        int rating;
        
        int size;

        ofVec3f position;
        ofVec3f screen_position;
        ofVec3f target_position;
        ofTrueTypeFont mono_bold;
        ofURLFileLoader loader;
    
        bool greyScale = false;
        bool colorByTopic = false;
    
        ofVec2f mask_pos;
        ofRectangle mask_rect;
    
        // analytics track
        int time_watched = 0;
        int focus_time = 0;

        int clicked = 0;
        bool in_view;
        bool deleted = false;
        bool blurred = false;
};
