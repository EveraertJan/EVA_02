#pragma once

#include "ofMain.h"
#include "post.h"
#include "comments.h"

class ephemeral_feed : public ofBaseApp{
	public:
		void setup();
		void update();
        void draw();
		void loadPosts(const std::string& dirPath);
    
        post * getPostOnTarget(ofVec2f target);
        void reset();
    
        void sort_feed();
        void shuffle_feed();
        void mark_feed();
    
        void trigger();
        void cube();
        void grid();
        void collage();

        ofTrueTypeFont font;
        ofTrueTypeFont mono_bold;
        ofTrueTypeFont largeFont;
        ofImage logo;

        float zoom;
        float zoom_target;

        vector<post> posts;
        vector<post> visible_posts;
        ofVec3f position;
        ofVec3f target;

        ofSoundPlayer click;

        int post_id = 0;
        int focus_id = 0;


        comments comment_module;
    
        int FORM = -1;
    
        bool DEBUG = false;
        int autoTrigger = -1;
        bool triggered = false;
    
    
    
        post dud;

};
