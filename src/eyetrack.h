#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"

#include "ofxGui.h"

class eyetrack : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw(bool hide_gui);
		

		void eyeTrack(bool  draw);
		int getAverage(std::vector<int> in);

		void inputSourceChanged(int &inputSource);
		

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

		ofTrueTypeFont baseFont;

		int look_x = 0;
		int look_y = 0;



		ofxToggle debug;
		ofxToggle debug_cv;
		ofxToggle debug_sound;
		ofxToggle live_input;
		ofxToggle enable_cv;
        ofxToggle rotate_video;
		ofxIntSlider treshold;
		ofxIntSlider sample_size;
		ofxIntSlider input_source;
		ofxFloatSlider variance_threshold;
		ofxPanel cv_gui;	

		
};
