#include "ofApp.h"
#include "stateManager.hpp"
#include "OSCManager.h"

//--------------------------------------------------------------
void ofApp::setup(){
    StateManager::getInstance().topics.push_back({"828w2zrrs1bgv36", "war", 0, 0});
    StateManager::getInstance().topics.push_back({"8tf81lyz0wf95g7", "pollution", 0, 0});
    StateManager::getInstance().topics.push_back({"46bsu1y0mhfvg8g", "flooding", 0, 0});
    StateManager::getInstance().topics.push_back({"5eofxxi10os7y64", "migration", 0, 0});
    StateManager::getInstance().topics.push_back({"2xia5v5pg0tv3b8", "poverty", 0, 0});
    StateManager::getInstance().topics.push_back({"u67379k9vu56d8n", "climate", 0, 0});
    StateManager::getInstance().topics.push_back({"20gaoy5np982eaz", "Extremism", 0, 0});
    StateManager::getInstance().topics.push_back({"9w14v34597n24rw", "none", 0, 0});
    
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    mono_bold.load("font/inputmono.ttf", 12);
    font.load("font/mono2.ttf", 12);
    font.setLetterSpacing(.9);
    largeFont.load("font/dots.ttf", 24);

    logo.load("icons/logo_black.png");
    
//    ofEnableDepthTest();
//    ofEnableNormalizedTexCoords();

    baseFont.load("arial.ttf", 10);

    gui.setup("main"); // most of the time you don't need a name
    gui.add(debug.setup("debug", true));
    gui.add(state.setup("state", 0, 0, 3));
    gui.add(history_size.setup("history size", 20, 10, 100));
    gui.add(click_time.setup("time clicking (in frames)", 30, 1, 120));
    bHide = true;

    mediapipe.setup();
    
    analytics_block.setup();
    
    feed.setup();
    consent_transaction.setup();
    consent_content.setup();
    ack_complete.setup();
    ack_topic_found.setup();
    
    
    loadFromAPI();
    reset();
    
    
    OSCManager::getInstance().setup();
    
}

//--------------------------------------------------------------
void ofApp::reset() {
    StateManager::getInstance().setState(0);
    StateManager::getInstance().setEmpathy(-1);
    StateManager::getInstance().setDeduced(-1);
    StateManager::getInstance().history_size = history_size;
    StateManager::getInstance().click_through = 0;
    
    consent_transaction.accepted = -1;
    consent_transaction.accepted_touched = 0;
    consent_content.accepted = -1;
    consent_content.accepted_touched = 0;
    ack_complete.accepted = -1;
    ack_complete.accepted_touched = 0;
    
    analytics_block.reset();
    feed.posts.clear();
    
    for (auto& item : base_posts) {
        item.time_watched = 0;
        item.clicked = 0;
        item.focus_time = 0;
    }
    
    feed.posts.assign(base_posts.begin(), base_posts.end());
    
    feed.reset();
    
}


//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(255);
    
    consent_transaction.press_time = click_time;
    consent_content.press_time = click_time;
    ack_complete.press_time = click_time;
    ack_topic_found.press_time = click_time;
    
    analytics_block.update();
    feed.update();
    mediapipe.update();
    look_at = mediapipe.view_guess;
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(200);
    ofFill();
    int state = StateManager::getInstance().getState();

    for(int i = 0; i < ofGetWidth(); i+= 100) {
        for(int j = 0; j < ofGetHeight(); j+= 100) {
            ofDrawRectangle(50+i+2, 50+j, 1, 5);
            ofDrawRectangle(50+i, 50+j + 2, 5, 1);
        }
    }
    noiseField();
    if(state == 0) {
        // idle
        int s = 700;
        ofSetColor(255);
        logo.draw(ofGetWidth()/2 - s/2 + (s/20), ofGetHeight()/2-s*0.9, s, s * 0.9);
        drawState("00 - AWAITING INTERACTION");
        if(StateManager::getInstance().newPerson) {
            StateManager::getInstance().setState(10);
        }
    }
    if(state == 10) {
        // consent to selling
        ofSetColor(255);
        logo.draw(ofGetWidth()/2 - 110, ofGetHeight()/2-600, 250, 225);
        drawState("10 - INFORMING CONSENT");
        consent_transaction.draw("DO YOU WANT TO SELL YOUR", "EMPATHY", "This machine will emploi techniques known to cause empathy fatigue, reducing your empathy for a subject. // // The subject is chosen algorithmically. // // In exchange, you will receive money.", "Yes", "No");
        if(consent_transaction.accepted == 1) {
            StateManager::getInstance().setState(11);
        } else if(consent_transaction.accepted == 0) {
            reset();
        }
    }
    if(state == 11) {
        // consent to extreme content
        ofSetColor(255);
        logo.draw(ofGetWidth()/2 - 110, ofGetHeight()/2-600, 250, 225);

        drawState("11 - INFORMING CONSENT");
        consent_content.draw("WARNING, THIS CONTENT MAY BE", "EXTREME", "Do you agree to be exposed to content that is extreme/explicit? // // The images show may cause triggers for some viewers. ", "Yes", "No");
        if(consent_content.accepted == 1) {
            StateManager::getInstance().setState(20);
        } else if(consent_content.accepted == 0) {
            reset();
        }
    }
    if(state == 20) {
        // detect
        drawState("20 - DETECTING");
        feed.draw();
        
        post * hovered = feed.getPostOnTarget(look_at);
        hovered->time_watched += (1000 / ofGetFrameRate());
        
        if(ofGetFrameNum() % 10) {
            analytics_block.analyse(&feed.posts);
        }
        if(analytics_block.deduced != -1) {
            StateManager::getInstance().setState(30);
        }
    }
    if(state == 30) {
        // optimise
        feed.draw();
        std::string top = StateManager::getInstance().topics[StateManager::getInstance().getDeduced()].handle;
        ofSetColor(255);
        ack_topic_found.draw("TOPIC DEDUCED", top, "Press continue to reduce empathy. // // We will use  techniques known to cause empathy fatigue // to achieve this. // Aborting will seize paiment", "Continue", "Abort");
        drawState("30 - OPTIMISING");
        if(ack_topic_found.accepted == 1) {
            StateManager::getInstance().setState(40);
        }
        if(ack_topic_found.accepted == 0) {
            reset();
        }
        
    }
    if(state == 40) {
        // enforce
        
        post * hovered = feed.getPostOnTarget(look_at);
        hovered->focus_time += 1;
        
        feed.draw();
        drawState("40 - ENFORCING");
        
        
        if(ofGetFrameNum() % 30) {
            analytics_block.detect_empathy(&feed.posts, feed.triggered);
            feed.triggered = false;
        }
        if( StateManager::getInstance().getEmpathy() == 0 || StateManager::getInstance().click_through > 10) {
            StateManager::getInstance().setState(50);
        }
    }
    if(state == 50) {
        // done
        ofSetColor(255);
        logo.draw(ofGetWidth()/2 - 110, ofGetHeight()/2-600, 250, 225);
        
        drawState("50 - REWARD");
        ack_complete.draw("TRANSACTION", "COMPLETE", "The system detected lowered empathy, often in the form of // rapid scrolling or ignoring the subject images. This concludes the transaction. // // You will receive payout. // // Find your reward in the coin slide.", "", "Restart");
        if(ack_complete.accepted == 0) {
            reset();
        }
    }
    
    if(state == 10) {
    }

    if(state >= 20 && state < 50) {
        analytics_block.draw();
        mediapipe.draw();
    }
    
    if(StateManager::getInstance().debug){
        gui.draw();
        ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 80);
    }
    
    if(error.length() > 0) {
        ofFill();
        ofRectangle r = largeFont.getStringBoundingBox(error, ofGetWidth()/2, ofGetHeight()-200);
        ofSetColor(0);
        ofDrawRectangle(r.position.x - r.width/2, r.position.y, r.width, r.height);
        ofSetColor(255);
        largeFont.drawString(error, ofGetWidth()/2 - r.width/2, ofGetHeight()-200);
    }
}
void ofApp::drawState(string state_message) {
    
    ofPushMatrix();
    ofTranslate(ofGetWidth() - 50, 50);
    ofSetColor(0);
    ofRectangle r = largeFont.getStringBoundingBox(state_message, 0, 0);
    ofDrawRectangle(r.x - r.width, r.y + r.height, r.width, r.height);
    ofSetColor(255);
    largeFont.drawString(state_message, -r.width, r.height);
    ofPopMatrix();
}

void ofApp::noiseField() {
  for(int i = 0; i < ofGetWidth(); i += 40) {
    for(int j = 0; j < ofGetHeight(); j+= 40) {
      float cur = (ofSignedNoise(i * 0.001, j * 0.001, 500 + ofGetFrameNum() * 0.004) + 1)/2;
        
        if( cur > 0.75 && cur < 0.8 ) {
         ofSetColor(0, 0, 248);
         ofFill();
         ofDrawRectangle(i-2, j-2, 4, 4);
       } else if(cur  > 0.7 && cur < 0.75) {
        ofSetColor(194, 253, 80);
        ofNoFill();
        ofDrawCircle(i, j, 10);
        ofFill();
        ofDrawCircle(i, j, 4);
           
      }
    }
  }
  ofSetColor(255);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 'h'){
        StateManager::getInstance().debug = !StateManager::getInstance().debug;
	}
	else if(key == 's'){
		gui.saveToFile("settings.xml");
	}
	else if(key == 'l'){
		gui.loadFromFile("settings.xml");
	}
    
    ofColor toSend;
    if(key == '0' ) { reset(); toSend.setHex(0xFFFFFF); OSCManager::getInstance().sendColor(toSend); }
    if(key == '1' ) { StateManager::getInstance().setState(10); toSend.setHex(0xFF0000); OSCManager::getInstance().sendColor(toSend); }
    if(key == '2' ) { StateManager::getInstance().setState(20); toSend.setHex(0xFFFF00); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '3' ) { StateManager::getInstance().setState(30); toSend.setHex(0x00FF00); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '4' ) { StateManager::getInstance().setState(40); toSend.setHex(0xFF00FF); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '5' ) { StateManager::getInstance().setState(50); toSend.setHex(0x00FFFF); OSCManager::getInstance().sendCoin(); OSCManager::getInstance().sendColor(toSend);   }
    if(key == '6' ) { StateManager::getInstance().setState(60); toSend.setHex(0xFFFFFF); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '7' ) { StateManager::getInstance().setState(70); toSend.setHex(0xFF0000); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '8' ) { StateManager::getInstance().setState(80); toSend.setHex(0xFF0000); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '9' ) { OSCManager::getInstance().sendCoin();  }


}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 99) {
        mediapipe.calibrate();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
//    mediapipe.calibrate();
    
    if(StateManager::getInstance().getState() == 20){
        
        post * clicked = feed.getPostOnTarget(ofVec2f(x, y));
        if(clicked->post_id != -1) {
            std::cout << "topic clicked" << StateManager::getInstance().topics[clicked->topic_id].handle << endl;
            clicked->clicked += 1;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
  feed.trigger();
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
 
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


void ofApp::loadFromAPI() {
    int post_id = 0;
    std::vector<std::string> imageUrls;
    
    std::string baseUrl = "http://127.0.0.1:8090/api/files/pbc_1125843985/";
    ofHttpResponse topic_response = ofLoadURL("http://127.0.0.1:8090/api/collections/topics/records");
    
    
    try {
        ofJson topic_json = ofJson::parse(topic_response.data);
        std::cout << "init topics" << endl;
        for (auto& topic_record : topic_json["items"]) {
            std::string id = topic_record["id"].get<std::string>();
            std::string label = topic_record["label"].get<std::string>();
            std::cout << id << ", " << label << endl;
            
            
            
            ofHttpResponse response = ofLoadURL("http://127.0.0.1:8090/api/collections/posts/records?perPage=60&filter=(field='" +id +"')");
            try {
                ofJson json = ofJson::parse(response.data);
                for (auto& record : json["items"]) {
                    std::string id = record["id"].get<std::string>();
                    std::string image = record["image"].get<std::string>() + "?thumb=400x400";
                    std::string mask = record["mask"].get<std::string>() + "?thumb=200x200";
                    std::string topic = record["field"].get<std::string>();
                    std::string mask_d = record["mask_data"].get<std::string>();
                    int rating = record["rating"].get<int>();
                    if(topic != "") {
                        post p;
                        p.setup( ofToString(baseUrl + id + "/" + image), topic, post_id, baseUrl + id + "/" + mask, rating, mask_d);
                        post_id += 1;
                        base_posts.push_back(p);
                    }
                    
                }
            }
            catch (std::exception exc) {
                std::cout << "could not fetch posts" << endl;
                error = "could not fetch posts";
            }
            
            
            
            
            
            
        }
    }
    catch (std::exception exc) {
        std::cout << "could not fetch topics" << endl;
        error = "could not fetch topics";
    }
    
    ofRandomize(base_posts);
    ofRandomize(base_posts);
    ofRandomize(base_posts);
    ofRandomize(base_posts);
    ofLog() << base_posts.size() << " posts loaded";
    
}
