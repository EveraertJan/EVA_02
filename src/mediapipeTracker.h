#pragma once
#include "ofMain.h"
#include "ofxMediaPipeFaceTracker.h"
#include "ofxAssimpModelLoader.h"

#include "ofxGui.h"

class mediapipeTracker : public ofBaseApp{
public:
	void setup() override;
	void update() override;
	void draw() override;
    void inputSourceChanged(int &inputSource);
	void exit() override;
    void calibrate();
	
    void drawGazeDebug();
    ofVec2f polyTrack(
      float orient_x, float orient_y, float orient_z, float orient_w,
      float eye_look_left, float eye_look_right, float eye_look_up, float eye_look_down,
      float face_x, float face_y, float face_z
      );
    ofVec2f estimateGaze(const std::vector<float>& features);

	ofVideoGrabber mGrabber;
    
	ofPixels mVideoPixels;
	ofTexture mVideoTexture;
	std::shared_ptr<ofx::MediaPipe::FaceTracker> faceTracker;
	
	ofFpsCounter mVideoFps;
    
    
    ofxToggle debug;
    ofxIntSlider input_source;
    ofxIntSlider exagerator;
    ofxFloatSlider eyeWeight;
    ofxFloatSlider headWeight;
    ofxPanel cv_gui;
    
    
    glm::quat calib_head_rotation;
    ofVec4f calib_eye;
    ofVec3f calib_head_center;
    ofVec2f calib_clickpoint;
    
    
    vector<ofVec2f> view_cache;
    ofVec2f view_guess;
    
    ofxAssimpModelLoader head;
    ofLight    light;
    
    bool debug_ui = true;
    
    
    // according to chatgpt
    std::vector<glm::vec2> faceLandmarks;

    float gazeX = 0.0f;
    float gazeY = 0.0f;

    int leftEyeCorner1 = 33;
    int leftEyeCorner2 = 133;
    int leftIrisCenter = 468;

    int upperLid = 159;
    int lowerLid = 145;
    
    ofEasyCam cam;
    glm::vec3 lastHitPoint, lastIrisPos, lastGazeDir;
    glm::vec3 lastScreenCenter, lastScreenNormal, lastScreenRight, lastScreenUp;
    float lastScreenWidth = 0.160f, lastScreenHeight = 0.242f;
    
};
