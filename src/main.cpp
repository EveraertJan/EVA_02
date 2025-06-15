#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	//Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	ofGLFWWindowSettings settings;
    settings.setGLVersion(3, 2);
#ifdef __APPLE__
    settings.setSize(1200, 1920);
    settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN
#elif __linux__
    settings.setSize(1200, 1920);
    settings.windowMode = OF_FULLSCREEN; //can also be 
    
#endif

	auto window = ofCreateWindow(settings);

	ofRunApp(window, make_shared<ofApp>());
	ofRunMainLoop();

}
