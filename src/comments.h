#pragma once

#include "ofMain.h"

class comments : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

        void loadComments();
        std::string insertNewlineAfterSpaces(const std::string& input, int spaceLimit);
		

        ofTrueTypeFont font;
        ofTrueTypeFont largeFont;
        ofImage logo;

        vector<string> comments_arr;
        vector<string> visible_comments_arr;
};
