#include "mediapipeTracker.h"
#include "stateManager.hpp"

// setting this to avoid typing ofx::MediaPipe::HandTracker
// now we can just use HandTracker
using namespace ofx::MediaPipe;

//--------------------------------------------------------------
void mediapipeTracker::setup(){
	
	// ofSetFrameRate( 60 );
	// ofSetVerticalSync(false);
	
    vector<ofVideoDevice> devices = mGrabber.listDevices();

    for(size_t i = 0; i < devices.size(); i++){
      if(devices[i].bAvailable){
          ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
      }else{
          ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
      }
    }


    mGrabber.setDeviceID(0);
//    mGrabber.setDesiredFrameRate(30);
#ifdef __APPLE__
    mGrabber.setup(1280, 720);
#elif __linux__
    ofLog() << "LINUX SETTINGS";
    mGrabber.setup(1024, 768);

#endif

	
	auto runMode = Tracker::MODE_VIDEO;
	// Tracker::MODE_LIVE_STREAM uses threads, but can be a bit slower when talking between python GIL threads and C++.
	// Helpful when running multiple trackers and not bogging down the entire app.
    // runMode = Tracker::MODE_LIVE_STREAM;
	
	
	faceTracker = make_shared<FaceTracker>();
	ofx::MediaPipe::FaceTracker::FaceSettings fsettings;
	fsettings.maxNum = 2;
	fsettings.minDetectionConfidence = 0.25f;
	fsettings.minPresenceConfidence = 0.25f;
	fsettings.minTrackingConfidence = 0.25f;
	fsettings.runningMode = runMode;
    fsettings.outputFaceBlendshapes = true;
	faceTracker->setup(fsettings);
	
    
    
    cv_gui.setup("CV debug"); // most of the time you don't need a name
    cv_gui.setPosition(230, 10);
    cv_gui.add(debug.setup("debug computer vision", false));
    input_source.addListener(this, &mediapipeTracker::inputSourceChanged);
    cv_gui.add(input_source.setup("input source", 0, 0, 4));
    cv_gui.add(exagerator.setup("exagerate", 200, 0, 500));
    cv_gui.add(eyeWeight.setup("Eye weight", 1.0, 0.0, 1.0));
    cv_gui.add(headWeight.setup("Head weight", 1.0, 0.0, 1.0));
    
    head.load("model/head_v2.glb");
    

}

void mediapipeTracker::inputSourceChanged(int &inputSource){
    //mGrabber.close();
    std::cout << "changing source" << endl;
    //mGrabber.setDeviceID(input_source);
    //mGrabber.setup(1280, 720);

}
//--------------------------------------------------------------
void mediapipeTracker::update(){
	mGrabber.update();
	if( mGrabber.isInitialized() && mGrabber.isFrameNew() ) {
		mVideoFps.newFrame();
		mVideoPixels = mGrabber.getPixels();
        
    #ifdef __linux__
        mVideoPixels.rotate90(true);
    #endif
        mVideoPixels.mirror(false, true);
        
		faceTracker->process(mVideoPixels);
		mVideoTexture.loadData(mVideoPixels);
	}
    
    
    std::shared_ptr<ofx::MediaPipe::Face> face;
    if( mVideoTexture.getWidth() > 0 && mVideoTexture.getHeight() > 0 ) {
        vector<std::shared_ptr<ofx::MediaPipe::Face>> faces = faceTracker->getFaces();
        if(faceTracker->getFaces().size() > 0) {
            face = faces[0];
            float EYE_LOOK_LEFT = round(face->getBlendShape("eyeLookOutLeft").score * 100) / 100;
            float EYE_LOOK_RIGHT = round(face->getBlendShape("eyeLookInLeft").score * 100) / 100;
            float EYE_LOOK_UP = round(face->getBlendShape("eyeLookUpLeft").score * 100) / 100;
            float EYE_LOOK_DOWN = round(face->getBlendShape("eyeLookDownLeft").score * 100) / 100;
            glm::quat orient = face->getOrientation();
            
            ofVec2f tracked = polyTrack(orient.x, orient.y, orient.z, orient.w, EYE_LOOK_LEFT, EYE_LOOK_RIGHT, EYE_LOOK_UP, EYE_LOOK_DOWN, face->getPosition().x, face->getPosition().y, face->getPosition().z);
            
//            view_guess = tracked;
            ofVec2f avg;
            for(auto & it : view_cache) {
                avg.x += it.x;
                avg.y += it.y;
               
            }
            view_guess = ofVec2f(avg.x/view_cache.size(), avg.y / view_cache.size());
            
        } else {
            StateManager::getInstance().setNoPerson( StateManager::getInstance().getNoPerson() + 1 );
        }
    }
    
    if(faceTracker->getFaces().size() > 0) {
        if( StateManager::getInstance().newPerson) {
            // generateID for the person
            StateManager::getInstance().currentPerson = floor(ofRandom(10000));
            StateManager::getInstance().newPerson = false;
        }
    } else {
        StateManager::getInstance().newPerson = true;
    }
}

//--------------------------------------------------------------
void mediapipeTracker::draw(){
    glm::quat orient;
    ofVec2f tracked;
    std::shared_ptr<ofx::MediaPipe::Face> face;
	if( mVideoTexture.getWidth() > 0 && mVideoTexture.getHeight() > 0 ) {
        if(debug) {
            ofRectangle windowRect( 50, 50, ofGetWidth()/2, ofGetHeight()/2 );
            ofRectangle videoRect( 0, 0, mVideoTexture.getWidth(), mVideoTexture.getHeight() );
            videoRect.scaleTo(windowRect);
            ofSetColor(255);
            mVideoTexture.draw(videoRect);
            ofPushMatrix(); {
                ofTranslate( videoRect.x, videoRect.y );
                ofScale( videoRect.getWidth() / mVideoTexture.getWidth(), videoRect.getHeight() / mVideoTexture.getHeight() );
                faceTracker->draw();
            }
            ofPopMatrix();
        }
        vector<std::shared_ptr<ofx::MediaPipe::Face>> faces = faceTracker->getFaces();
        if(faceTracker->getFaces().size() > 0) {
            face = faces[0];
            float EYE_WIDE_LEFT = face->getBlendShape("eyeWideLeft").score;
            float EYE_WIDE_RIGHT = face->getBlendShape("eyeWideRight").score;
            
            
            float EYE_LOOK_LEFT = round(face->getBlendShape("eyeLookOutLeft").score * 100) / 100;
            float EYE_LOOK_RIGHT = round(face->getBlendShape("eyeLookInLeft").score * 100) / 100;
            float EYE_LOOK_UP = round(face->getBlendShape("eyeLookUpLeft").score * 100) / 100;
            float EYE_LOOK_DOWN = round(face->getBlendShape("eyeLookDownLeft").score * 100) / 100;
            orient = face->getOrientation();
            
            if(StateManager::getInstance().debug) {
                ofPushMatrix();
                ofTranslate(ofGetWidth() - 200, 10);
                stringstream ss;
                ss << std::endl << "X" << round(orient.x*100)/100;
                ss << std::endl << "Y" << round(orient.y*100)/100;
                ss << std::endl << "Z" << round(orient.z*100)/100;
                ss << std::endl << "W" << round(orient.w*100)/100;
                ss << std::endl;
                ss << std::endl << "X: " << round(calib_head_rotation.x*100)/100 << " - " << round(orient.x*100)/100 - round(calib_head_rotation.x*100)/100;
                ss << std::endl << "Y: " << round(calib_head_rotation.y*100)/100 << " - " << round(orient.y*100)/100 - round(calib_head_rotation.y*100)/100;
                ss << std::endl << "Z: " << round(calib_head_rotation.z*100)/100 << " - " << round(orient.z*100)/100 - round(calib_head_rotation.z*100)/100;
                ss << std::endl << "W: " << round(calib_head_rotation.w*100)/100 << " - " << round(orient.w*100)/100 - round(calib_head_rotation.w*100)/100;
                ss << std::endl;
                ss << std::endl << "LEFT: " << EYE_LOOK_LEFT;
                ss << std::endl << "RIGHT: " << EYE_LOOK_RIGHT;
                ss << std::endl << "UP: " << EYE_LOOK_UP;
                ss << std::endl << "DOWN: " << EYE_LOOK_DOWN;
                ss << std::endl;
                ss << std::endl << "LEFT: " <<  calib_eye.x << " - " << EYE_LOOK_LEFT - calib_eye.x;
                ss << std::endl << "RIGHT: " << calib_eye.y << " - " << EYE_LOOK_RIGHT - calib_eye.y;
                ss << std::endl << "UP: " << calib_eye.z << " - " << EYE_LOOK_UP - calib_eye.z;
                ss << std::endl << "DOWN: " << calib_eye.w << " - " << EYE_LOOK_DOWN - calib_eye.w;
                ofDrawBitmapStringHighlight(ss.str(), 10, 10 );
                ofPopMatrix();
            }
            
            tracked = polyTrack(orient.x, orient.y, orient.z, orient.w, EYE_LOOK_LEFT, EYE_LOOK_RIGHT, EYE_LOOK_UP, EYE_LOOK_DOWN, face->getPosition().x, face->getPosition().y, face->getPosition().z);
            
            if(StateManager::getInstance().debug){
                
                ofSetColor(0, 255, 255);
                ofNoFill();
                ofDrawCircle(tracked.x, tracked.y, 10);
            }
            
            
            
            
            view_cache.push_back(tracked);
            while(view_cache.size() > 20) {
                view_cache.erase(view_cache.begin());
            }
        }
        
        
        int rounding = 100;
        int size = 20;
        ofVec2f avg;
        for(auto & it : view_cache) {
            avg.x += it.x;
            avg.y += it.y;
            ofSetColor(0, 255, 50);
            ofNoFill();
            
            
            int i = round(it.x / rounding) * rounding;
            int j = round(it.y / rounding) * rounding;
            ofFill();
        
            ofDrawRectangle(50+i+2, 50+j, 1, 5);
            ofDrawRectangle(50+i, 50+j + 2, 5, 1);
                
        }
        if(StateManager::getInstance().debug) {
            ofDrawCircle(avg.x/view_cache.size(), avg.y / view_cache.size(), 20);
        }
        
        if(StateManager::getInstance().debug) {
            stringstream ss;
            ss << "App FPS: " << ofGetFrameRate();
            ss << std::endl << "Video FPS: " << mVideoFps.getFps();
            ss << std::endl << "Face Tracker FPS: " << faceTracker->getFps();
            ofDrawBitmapStringHighlight(ss.str(), 24, 24 );
        }
        
        
	}
    
    if(StateManager::getInstance().debug) {
        cv_gui.draw();
    }
    
    
}
void mediapipeTracker::drawGazeDebug() {
    cam.begin();
    ofSetColor(255, 0, 0);
    ofDrawBitmapString("Iris Pos", lastIrisPos + glm::vec3(0.01f));
    ofDrawBitmapString("Hit Point", lastHitPoint + glm::vec3(0.01f));

    ofSetColor(255, 0, 0);
    ofDrawSphere(lastIrisPos, 0.01); // Make this big for testing

    ofSetColor(0, 255, 0);
    ofDrawLine(lastIrisPos, lastIrisPos + lastGazeDir * 0.2f); // Gaze ray

    ofSetColor(255, 255, 0);
    ofDrawSphere(lastHitPoint, 0.01); // Hit point
    cam.end();
    
    cam.draw();
    ofVec3f t = lastHitPoint * 0.2f;
//    ofLog() << lastHitPoint * 0.2f;
//    ofLog() << lastHitPoint;
    ofSetColor(255, 0, 255);
    ofDrawSphere(t.x, t.y, 10); // Hit point
    
}

//--------------------------------------------------------------
void mediapipeTracker::calibrate() {
    
    int mX = ofGetMouseX();
    int mY = ofGetMouseY();
    calib_clickpoint = ofVec2f(mX, mY);
    
    vector<std::shared_ptr<ofx::MediaPipe::Face>> faces = faceTracker->getFaces();
    if(faceTracker->getFaces().size() > 0) {
        std::shared_ptr<ofx::MediaPipe::Face> face = faces[0];
        float EYE_WIDE_LEFT = face->getBlendShape("eyeWideLeft").score;
        float EYE_WIDE_RIGHT = face->getBlendShape("eyeWideRight").score;
        
        
        float EYE_LOOK_LEFT = round(face->getBlendShape("eyeLookOutLeft").score * 100) / 100;
        float EYE_LOOK_RIGHT = round(face->getBlendShape("eyeLookInLeft").score * 100) / 100;
        float EYE_LOOK_UP = round(face->getBlendShape("eyeLookUpLeft").score * 100) / 100;
        float EYE_LOOK_DOWN = round(face->getBlendShape("eyeLookDownLeft").score * 100) / 100;
        glm::quat orient = face->getOrientation();
        
        
        
        calib_eye = ofVec4f(EYE_LOOK_LEFT, EYE_LOOK_RIGHT, EYE_LOOK_UP, EYE_LOOK_DOWN);
        calib_head_rotation = orient;
////        
//        std::cout << mX << "," << mY << ","; // click
//        std::cout << orient.x << "," << orient.y << ",";
//        std::cout << orient.z << "," << orient.w << ",";
//        std::cout << EYE_LOOK_LEFT << "," << EYE_LOOK_RIGHT << ","; // eye hor
//        std::cout << EYE_LOOK_UP << "," << EYE_LOOK_DOWN << ","; // eye ver
//        std::cout << face->getPosition().x << "," << face->getPosition().y << "," << face->getPosition().z;
//        std::cout << endl;
        
        
        
    }
}



//--------------------------------------------------------------
void mediapipeTracker::exit(){
	faceTracker.reset();
	Tracker::PyShutdown();
}


ofVec2f mediapipeTracker::polyTrack(
        float orient_x, float orient_y, float orient_z, float orient_w,
        float eye_look_left, float eye_look_right, float eye_look_up, float eye_look_down,
        float face_x, float face_y, float face_z
    ) {
    
    // Define a struct to hold the coefficients and intercept for a single dimension (x or y)
    struct GazeModelCoefficients {
        double intercept;
        double orient_x_coef;
        double orient_y_coef;
        double orient_z_coef;
        double orient_w_coef;
        double eye_look_left_coef;
        double eye_look_right_coef;
        double eye_look_up_coef;
        double eye_look_down_coef;
        double face_x_coef;
        double face_y_coef;
        double face_z_coef;
    };

    // Initialize the coefficients for click_x estimation
    // These values are derived from the linear regression model trained on your combined data.
    const GazeModelCoefficients clickXCoefficients = {
        624.995506,       // intercept
        -705.261649,      // orient_x
        -59.687528,       // orient_y
        6029.096039,      // orient_z
        335.985693,       // orient_w
        1085.888231,      // eye_look_left
        -641.354448,      // eye_look_right
        276.281908,       // eye_look_up
        1557.987034,      // eye_look_down
        0.114610,         // face_x
        0.234271,         // face_y
        0.345328          // face_z
    };

    // Initialize the coefficients for click_y estimation
    // These values are derived from the linear regression model trained on your combined data.
    const GazeModelCoefficients clickYCoefficients = {
        12177.609114,     // intercept
        -10862.315272,    // orient_x
        5467.366675,      // orient_y
        2359.993992,      // orient_z
        -8971.711974,     // orient_w
        348.123926,       // eye_look_left
        -1730.025211,     // eye_look_right
        -2508.381106,     // eye_look_up
        1640.955560,      // eye_look_down
        -1.300137,        // face_x
        -0.564866,        // face_y
        -4.510508         // face_z
    };
    
    // Calculate estimated click_x
    double estimated_click_x = clickXCoefficients.intercept +
                               (clickXCoefficients.orient_x_coef * orient_x) +
                               (clickXCoefficients.orient_y_coef * orient_y) +
                               (clickXCoefficients.orient_z_coef * orient_z) +
                               (clickXCoefficients.orient_w_coef * orient_w) +
                               (clickXCoefficients.eye_look_left_coef * eye_look_left) +
                               (clickXCoefficients.eye_look_right_coef * eye_look_right) +
                               (clickXCoefficients.eye_look_up_coef * eye_look_up) +
                               (clickXCoefficients.eye_look_down_coef * eye_look_down) +
                               (clickXCoefficients.face_x_coef * face_x) +
                               (clickXCoefficients.face_y_coef * face_y) +
                               (clickXCoefficients.face_z_coef * face_z);

    // Calculate estimated click_y
    double estimated_click_y = clickYCoefficients.intercept +
                               (clickYCoefficients.orient_x_coef * orient_x) +
                               (clickYCoefficients.orient_y_coef * orient_y) +
                               (clickYCoefficients.orient_z_coef * orient_z) +
                               (clickYCoefficients.orient_w_coef * orient_w) +
                               (clickYCoefficients.eye_look_left_coef * eye_look_left) +
                               (clickYCoefficients.eye_look_right_coef * eye_look_right) +
                               (clickYCoefficients.eye_look_up_coef * eye_look_up) +
                               (clickYCoefficients.eye_look_down_coef * eye_look_down) +
                               (clickYCoefficients.face_x_coef * face_x) +
                               (clickYCoefficients.face_y_coef * face_y) +
                               (clickYCoefficients.face_z_coef * face_z);

//    double click_x = -3612623.322397
//    + (8725033.701805 * orient_x)
//    + (-191661.769266 * orient_y)
//    + (559937.154461 * orient_z)
//    + (-2148743.080575 * orient_w)
//    + (262340.131481 * eye_look_left)
//    + (471274.421154 * eye_look_right)
//    + (323364.225389 * eye_look_up)
//    + (1844372.144324 * eye_look_down)
//    + (94.355594 * face_x)
//    + (-287.968053 * face_y)
//    + (-464.896428 * face_z)
//    + (-5110264.292686 * orient_x*orient_x)
//    + (286057.989458 * orient_x*orient_y)
//    + (-537449.646626 * orient_x*orient_z)
//    + (2101517.397144 * orient_x*orient_w)
//    + (-245654.497754 * orient_x*eye_look_left)
//    + (-480319.763799 * orient_x*eye_look_right)
//    + (-322798.679091 * orient_x*eye_look_up)
//    + (-1820181.051450 * orient_x*eye_look_down)
//    + (-110.338030 * orient_x*face_x)
//    + (279.693163 * orient_x*face_y)
//    + (438.468889 * orient_x*face_z)
//    + (-875179.297827 * orient_y*orient_y)
//    + (-50033.713681 * orient_y*orient_z)
//    + (-6754.622459 * orient_y*orient_w)
//    + (9467.334084 * orient_y*eye_look_left)
//    + (67024.854410 * orient_y*eye_look_right)
//    + (-4935.156616 * orient_y*eye_look_up)
//    + (-87519.144469 * orient_y*eye_look_down)
//    + (-21.100245 * orient_y*face_x)
//    + (-51.860776 * orient_y*face_y)
//    + (-348.575519 * orient_y*face_z)
//    + (-819730.186834 * orient_z*orient_z)
//    + (89791.224004 * orient_z*orient_w)
//    + (-6953.287918 * orient_z*eye_look_left)
//    + (-15994.508230 * orient_z*eye_look_right)
//    + (5860.256418 * orient_z*eye_look_up)
//    + (-8991.562794 * orient_z*eye_look_down)
//    + (1.115329 * orient_z*face_x)
//    + (-3.172753 * orient_z*face_y)
//    + (-18.198404 * orient_z*face_z)
//    + (-1078932.453454 * orient_w*orient_w)
//    + (48880.977573 * orient_w*eye_look_left)
//    + (93486.128426 * orient_w*eye_look_right)
//    + (59754.141110 * orient_w*eye_look_up)
//    + (281436.137219 * orient_w*eye_look_down)
//    + (33.452469 * orient_w*face_x)
//    + (-43.750463 * orient_w*face_y)
//    + (-87.674291 * orient_w*face_z)
//    + (1299.417529 * eye_look_left*eye_look_left)
//    + (-4287.603764 * eye_look_left*eye_look_right)
//    + (-1664.885189 * eye_look_left*eye_look_up)
//    + (-25082.951420 * eye_look_left*eye_look_down)
//    + (-1.007089 * eye_look_left*face_x)
//    + (-1.571919 * eye_look_left*face_y)
//    + (-60.741916 * eye_look_left*face_z)
//    + (-6256.502502 * eye_look_right*eye_look_right)
//    + (-1554.699271 * eye_look_right*eye_look_up)
//    + (11823.321914 * eye_look_right*eye_look_down)
//    + (13.619291 * eye_look_right*face_x)
//    + (36.359476 * eye_look_right*face_y)
//    + (-90.925633 * eye_look_right*face_z)
//    + (2146.604977 * eye_look_up*eye_look_up)
//    + (3854.875902 * eye_look_up*eye_look_down)
//    + (0.790546 * eye_look_up*face_x)
//    + (3.366169 * eye_look_up*face_y)
//    + (4.188960 * eye_look_up*face_z)
//    + (-9325.617324 * eye_look_down*eye_look_down)
//    + (7.805274 * eye_look_down*face_x)
//    + (3.723281 * eye_look_down*face_y)
//    + (-19.343013 * eye_look_down*face_z)
//    + (0.003704 * face_x*face_x)
//    + (0.014823 * face_x*face_y)
//    + (0.052700 * face_x*face_z)
//    + (0.000719 * face_y*face_y)
//    + (-0.003378 * face_y*face_z)
//    + (0.080340 * face_z*face_z);
//    double click_y = -10335557.591210
//    + (12381723.711608 * orient_x)
//    + (-113337.546657 * orient_y)
//    + (471149.209241 * orient_z)
//    + (-358125.040072 * orient_w)
//    + (183201.891319 * eye_look_left)
//    + (1880443.842651 * eye_look_right)
//    + (82097.337661 * eye_look_up)
//    + (401027.667927 * eye_look_down)
//    + (-133.741113 * face_x)
//    + (418.423082 * face_y)
//    + (-489.678331 * face_z)
//    + (-2049420.826110 * orient_x*orient_x)
//    + (34073.791278 * orient_x*orient_y)
//    + (-427232.703165 * orient_x*orient_z)
//    + (346544.974741 * orient_x*orient_w)
//    + (-175499.003222 * orient_x*eye_look_left)
//    + (-1855043.882652 * orient_x*eye_look_right)
//    + (-85682.947777 * orient_x*eye_look_up)
//    + (-407039.198764 * orient_x*eye_look_down)
//    + (117.548242 * orient_x*face_x)
//    + (-417.123339 * orient_x*face_y)
//    + (543.391380 * orient_x*face_z)
//    + (4106545.264946 * orient_y*orient_y)
//    + (6542.974566 * orient_y*orient_z)
//    + (558.472976 * orient_y*orient_w)
//    + (-18500.031517 * orient_y*eye_look_left)
//    + (-293029.458650 * orient_y*eye_look_right)
//    + (45261.026151 * orient_y*eye_look_up)
//    + (159395.137312 * orient_y*eye_look_down)
//    + (-24.467918 * orient_y*face_x)
//    + (67.001406 * orient_y*face_y)
//    + (232.052028 * orient_y*face_z)
//    + (4092179.527713 * orient_z*orient_z)
//    + (12054.379498 * orient_z*orient_w)
//    + (-13357.677450 * orient_z*eye_look_left)
//    + (-220412.083205 * orient_z*eye_look_right)
//    + (-3984.296268 * orient_z*eye_look_up)
//    + (13730.861893 * orient_z*eye_look_down)
//    + (33.144380 * orient_z*face_x)
//    + (-25.974638 * orient_z*face_y)
//    + (-164.580747 * orient_z*face_z)
//    + (4197637.212516 * orient_w*orient_w)
//    + (3937.060879 * orient_w*eye_look_left)
//    + (152801.514418 * orient_w*eye_look_right)
//    + (-349.261968 * orient_w*eye_look_up)
//    + (-242665.719169 * orient_w*eye_look_down)
//    + (-5.088723 * orient_w*face_x)
//    + (77.469170 * orient_w*face_y)
//    + (-15.551856 * orient_w*face_z)
//    + (-7374.059656 * eye_look_left*eye_look_left)
//    + (-69264.890960 * eye_look_left*eye_look_right)
//    + (1453.890467 * eye_look_left*eye_look_up)
//    + (13387.486479 * eye_look_left*eye_look_down)
//    + (2.433213 * eye_look_left*face_x)
//    + (-5.430523 * eye_look_left*face_y)
//    + (-7.319479 * eye_look_left*face_z)
//    + (60.370788 * eye_look_right*eye_look_right)
//    + (7149.012880 * eye_look_right*eye_look_up)
//    + (-69392.571753 * eye_look_right*eye_look_down)
//    + (-36.827065 * eye_look_right*face_x)
//    + (-21.107457 * eye_look_right*face_y)
//    + (106.004507 * eye_look_right*face_z)
//    + (613.410174 * eye_look_up*eye_look_up)
//    + (23005.718715 * eye_look_up*eye_look_down)
//    + (-5.670085 * eye_look_up*face_x)
//    + (-1.347092 * eye_look_up*face_y)
//    + (-6.399740 * eye_look_up*face_z)
//    + (159070.692154 * eye_look_down*eye_look_down)
//    + (32.414354 * eye_look_down*face_x)
//    + (-24.175582 * eye_look_down*face_y)
//    + (-289.559874 * eye_look_down*face_z)
//    + (0.008195 * face_x*face_x)
//    + (0.008698 * face_x*face_y)
//    + (0.043711 * face_x*face_z)
//    + (0.004694 * face_y*face_y)
//    + (0.000047 * face_y*face_z)
//    + (-0.151979 * face_z*face_z);
//    
    return ofVec2f(estimated_click_x, estimated_click_y);
}

ofVec2f mediapipeTracker::estimateGaze(const std::vector<float>& features) {
    // Hardcoded model parameters (replace with your actual trained weights)
    const std::vector<float> weights_x = { -189.06365507, -51.17106482, 289.03099709, 208.5678438,  127.19514685, -78.79068602, 2.29824684, -7.08744912,  14.03192333,  12.23054956, -18.56911732};
    const std::vector<float> weights_y = {-11.49742839,  59.57891945, 145.26929801, -429.46706037,  70.20453156, -55.58448755, -285.63506098, 68.10187095 -122.53137146 -170.12548075,  -128.7590038};
    const float bias_x = 629.258883248733;
    const float bias_y = 942.611675126904;
    
    // Hardcoded standardization parameters
    const std::vector<float> mean = { 9.85465478e-01, 1.04225280e-04, 1.80124275e-02 -1.59665540e-01, 2.68413706e-01, 2.01649746e-02, 3.30279188e-01, 2.24873096e-02, 2.12083604e+02, 5.57994820e+02, 1.61644226e+02 };
    const std::vector<float> scale = { 6.35168621e-03, 1.95338975e-02, 3.46860172e-02, 3.77412298e-02, 1.15950700e-01, 3.10288582e-02, 1.16411764e-01, 1.87734582e-02, 7.87598736e+01, 1.70606296e+02, 2.10769931e+01 };
    
    // Standardize features
    std::vector<float> scaled_features(features.size());
    for (size_t i = 0; i < features.size(); ++i) {
        scaled_features[i] = (features[i] - mean[i]) / scale[i];
    }
    
    // Simple linear model prediction
    float click_x = bias_x;
    float click_y = bias_y;
    for (size_t i = 0; i < scaled_features.size(); ++i) {
        click_x += weights_x[i] * scaled_features[i];
        click_y += weights_y[i] * scaled_features[i];
    }
    
    return ofVec2f(click_x, click_y);
}
