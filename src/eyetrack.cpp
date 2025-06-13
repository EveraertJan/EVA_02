#include "eyetrack.h"

//--------------------------------------------------------------
void eyetrack::setup(){
	camWidth = 640;  // try to grab at this size.
	camHeight = 480;

  vector<ofVideoDevice> devices = vidGrabber.listDevices();

  for(size_t i = 0; i < devices.size(); i++){
		if(devices[i].bAvailable){
			ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
		}else{
			ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
		}
	}

  vidGrabber.setDeviceID(0);
	vidGrabber.setDesiredFrameRate(30);
	vidGrabber.setup(camWidth, camHeight);
	videoTexture.allocate(camWidth, camHeight, OF_PIXELS_RGB);

  movie.load("vid_test.mp4");
  movie.setVolume(0);
  movie.play();
  movie.setLoopState(OF_LOOP_NORMAL);
  framePixels.allocate(camWidth, camHeight, movie.getPixels().getNumChannels());

	eye_finder.setup("cascades/haarcascade_left_eye.xml");
	face_finder.setup("cascades/haarcascade_frontalface_default.xml");


	cv_gui.setup("CV debug"); // most of the time you don't need a name
  cv_gui.setPosition(230, 10);
  cv_gui.add(enable_cv.setup("Enable CV", false));
  cv_gui.add(rotate_video.setup("Rotate cam", false));
  cv_gui.add(debug_cv.setup("debug computer vision", false));
  cv_gui.add(debug_sound.setup("debug sound", false));
  cv_gui.add(live_input.setup("live_input", false));
	cv_gui.add(treshold.setup("threshold", 20, 1, 100));
	cv_gui.add(sample_size.setup("sample size", 20, 1, 100));
  input_source.addListener(this, &eyetrack::inputSourceChanged);
	cv_gui.add(input_source.setup("input source", 0, 0, 4));
	cv_gui.add(variance_threshold.setup("variance threshold", 0.5, 0, 10));


}
void eyetrack::inputSourceChanged(int &inputSource){
  vidGrabber.setDeviceID(input_source);
  vidGrabber.setup(camWidth, camHeight);
}
//--------------------------------------------------------------
void eyetrack::update(){
  ofBackground(255);

  if(debug_sound) { movie.setVolume(1); }
  else { movie.setVolume(0); }
  
  if(live_input) {
      vidGrabber.update();
      if (vidGrabber.isFrameNew()) {
      vidGrabber.getPixels().resizeTo(framePixels);
      frameImage.setFromPixels(framePixels);
      }
  } else {
      movie.update();
      if (movie.isFrameNew()) {
      movie.getPixels().resizeTo(framePixels);
      frameImage.setFromPixels(framePixels);
      }
  }
  eye_finder.findHaarObjects(frameImage);

  face_finder.findHaarObjects(frameImage);

  ofSetColor(255);
}

//--------------------------------------------------------------
void eyetrack::draw(bool hide_gui) {

  eyeTrack(debug_cv);

  if(!hide_gui){
    cv_gui.draw();
  }
}

void eyetrack::eyeTrack(bool draw){
  int offset = 200;
  ofSetColor(255);
  frameImage.draw(offset, offset);
  
  ofNoFill();
  ofSetColor(255);
  for(unsigned int i = 0; i < face_finder.blobs.size(); i++) {
    ofRectangle cur = face_finder.blobs[i].boundingRect;
    ofDrawRectangle(cur.x + offset, cur.y + offset, cur.width, cur.height);
  }


  if(eye_finder.blobs.size() > 0) {
    ofRectangle leftest = eye_finder.blobs[0].boundingRect;
    for( int i = 0;  i < eye_finder.blobs.size(); i++) {
      if( eye_finder.blobs[i].boundingRect.x < leftest.x) {
        leftest = eye_finder.blobs[i].boundingRect;
        ofDrawRectangle(eye_finder.blobs[i].boundingRect.x + offset, eye_finder.blobs[i].boundingRect.y + offset, eye_finder.blobs[i].boundingRect.width, eye_finder.blobs[i].boundingRect.height);
      }
    }
    ofRectangle cur = leftest;

    x_avgs.push_back(cur.x);
    y_avgs.push_back(cur.y);
    w_avgs.push_back(cur.width);
    h_avgs.push_back(cur.height);
    if(x_avgs.size() > sample_size) {
      x_avgs.erase( x_avgs.begin() );
      y_avgs.erase( y_avgs.begin() );
      w_avgs.erase( w_avgs.begin() );
      h_avgs.erase( h_avgs.begin() );
    }
	}

  if(x_avgs.size() > 0) {
    ofPixels croppedPixels;
    ofTexture croppedTexture;
    int x = getAverage(x_avgs);
    int y = getAverage(y_avgs);
    int w = getAverage(w_avgs);
    int h = getAverage(h_avgs);
    frameImage.getPixels().cropTo(croppedPixels, x, y, w, h);
    ofDrawCircle(x + w / 2, y + h/2, 10);

    int width = croppedPixels.getWidth();
    int height = croppedPixels.getHeight();
    
    int min = 255;
    int max = 0;
    for (int y = 0; y < height; y+=8) {
      for (int x = 0; x < width; x+=8) {
        float brightness = croppedPixels.getColor(x, y).getBrightness();
        if(brightness > max) { max = brightness; }
        if(brightness < min) { min = brightness; }
      }
    }
  
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) { 
        ofColor color = croppedPixels.getColor(x, y);
        float brightness = color.getBrightness();
        int var = 10;
        float newBright = ((brightness - (min + var)) * 255) / ((max - var) - (min + var));
        if(newBright < 0) { newBright = 0;}
        if(newBright > 255) { newBright = 255;}
        croppedPixels.setColor(x, y, newBright);

        if (newBright < treshold) {
          color = ofColor::white;
        } else {            
          color = ofColor::black; 
        }
        croppedPixels.setColor(x, y, color);
      }
    }
    croppedTexture.loadData(croppedPixels);

    ofPixels pixels = croppedPixels;
    ofPixels redOnly;
    redOnly.allocate(pixels.getWidth(), pixels.getHeight(), OF_IMAGE_GRAYSCALE);

    for (int y = 0; y < croppedTexture.getHeight(); y++) {
      for (int x = 0; x < croppedTexture.getWidth(); x++) {
        redOnly.setColor(x, y,  pixels.getColor(x, y));
      }
    }

    ofxCvGrayscaleImage redBinary;
      redBinary.draw(500, 500);
    redBinary.setFromPixels(redOnly);

    contourFinder.findContours(redBinary, 10, 10000, 10, false);

    vector<cv::Point> allPoints;
    for (int i = 0; i < contourFinder.nBlobs; i++) {
      for (auto &pt : contourFinder.blobs[i].pts) {
        allPoints.push_back(cv::Point(pt.x, pt.y));
      }
    }

    if (allPoints.size() >= 5) {
      cv::RotatedRect ellipse = cv::fitEllipse(allPoints);

      float x_offset = width/2 - ellipse.center.x;
      float y_offset = height/2 - ellipse.center.y;
      float s_offset = ellipse.size.width - ellipse.size.height;
      int g_x = 0;
      int g_y = 0;
      if( x_offset < -variance_threshold){ g_x = -1; }
      if( x_offset > variance_threshold){ g_x = 1; }
      if( y_offset < -variance_threshold){ g_y = -1; }
      if( y_offset > variance_threshold){ g_y = 1; }

      ofPushMatrix();
      ofTranslate(20, ofGetHeight() / 2 + 100);
      redBinary.draw(0, 0);
      ofPushStyle();
      ofFill();
      ofSetColor(ofColor::cyan);
      ofDrawEllipse(ellipse.center.x, ellipse.center.y, ellipse.size.width, ellipse.size.height);
      ofPopStyle();
      ofDrawLine(width/2, height/2, ellipse.center.x, ellipse.center.y);
      ofFill();
      ofDrawBitmapString(x_offset, 220,  0);
      ofDrawBitmapString(y_offset, 220,  20);
      ofDrawBitmapString(ellipse.size.width, 220, 40);
      ofDrawBitmapString(ellipse.size.height, 220, 60);
      ofDrawBitmapString(s_offset, 220, 80);
      ofNoFill();
      ofSetColor(255);
      ofDrawRectangle(0, 0, 10, 10);
      ofDrawRectangle(0, 15, 10, 10);
      ofDrawRectangle(0, 30, 10, 10);
      ofDrawRectangle(15, 0, 10, 10);
      ofDrawRectangle(15, 15, 10, 10);
      ofDrawRectangle(15, 30, 10, 10);
      ofDrawRectangle(30, 0, 10, 10);
      ofDrawRectangle(30, 15, 10, 10);
      ofDrawRectangle(30, 30, 10, 10);
      ofFill();
      ofDrawRectangle(15 + (g_x * 15), 15 + (g_y * 15), 8, 8);
      ofPopMatrix();

    }
  }
  ofSetColor(255, 0, 0);
  look_x = ofGetMouseX();
  look_y = ofGetMouseY();
  ofDrawCircle(look_x, look_y, 10);
}

int eyetrack::getAverage(std::vector<int> in) {
    int count = 0;
    for(int i = 0; i < in.size(); i++) {
        count += in[i];
    }
    return floor(count / in.size());
}
