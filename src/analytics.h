#pragma once

#include "ofMain.h"
#include "post.h"

class analytics : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        void reset();
        
        void resetCounters();
    
        void analyse(vector<post>* posts);
        void detect_empathy(vector<post>* posts, bool triggered);
		
        void drawTiming();
        void drawWeights();
        void drawEmpathyMeasure();
        void drawDatums();

        ofTrueTypeFont font;
        ofTrueTypeFont largeFont;
        ofImage logo;
    
        ofTime startTime;
    
        struct Topic {
            string name;
            string handle;
            int totalweight;
            int focus_time;
        };
        vector<Topic> topics;
        
        int deduced = -1;
        int min_watch_time = 50;
};
