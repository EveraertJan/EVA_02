#include "ofApp.h"
#include "stateManager.hpp"
#include "OSCManager.h"
#include "statisticsManager.hpp"
#include "styleManager.hpp"

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
    ofSetFrameRate(30);

    logo.load("icons/logo_black.png");
    
    gui.setup("main"); // most of the time you don't need a name
    gui.add(debug.setup("debug", true));
    gui.add(state.setup("state", 0, 0, 3));
    gui.add(history_size.setup("history size", 20, 10, 100));
    gui.add(click_time.setup("time clicking (in frames)", 3, 1, 120));
    gui.add(offset_x.setup("tracking offset x", 0, -1000, 1000));
    gui.add(offset_y.setup("tracking offset y", 0, -1000, 1000));
    gui.add(scale_x.setup("scale x", 1, 0, 3));
    gui.add(scale_y.setup("scale y", 1, 0, 3));
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
    StatisticsManager::getInstance().reset();
    StyleManager::getInstance().setup();
    
    StyleManager::getInstance().base_font.load("font/mono2.ttf", 16);
    StyleManager::getInstance().base_font.setLetterSpacing(1.05);
    StyleManager::getInstance().mid_font.load("font/mono2.ttf", 20);
    StyleManager::getInstance().mid_font.setLineHeight(28);
    StyleManager::getInstance().large_font.load("font/inputmono.ttf", 76);
    StyleManager::getInstance().bold_font.load("font/inputmono.ttf", 24);
    StyleManager::getInstance().debug_font.load("font/inputmono.ttf", 12);
    StyleManager::getInstance().state_font.load("font/dots.ttf", 24);
    
    ofDisableArbTex();
    
#if __linux__
    StateManager::getInstance().debug = false;
#endif
}

//--------------------------------------------------------------
void ofApp::reset() {
    StatisticsManager::getInstance().reset();
    
    StateManager::getInstance().setState(0);
    StateManager::getInstance().setEmpathy(1);
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
    
    mediapipe.offset.x = offset_x;
    mediapipe.offset.y = offset_y;
    mediapipe.scale.x = scale_x;
    mediapipe.scale.y = scale_y;
    
    ofBackground(255);
    StateManager::getInstance().state_running++;
    
    
    int state = StateManager::getInstance().getState();
    if(state == 10) {
        consent_transaction.press_time = click_time;
    }
    if(state == 11) {
        consent_content.press_time = click_time;
    }
    if(state  == 50){
    ack_complete.press_time = click_time;
    }
    if(state == 30) {
        ack_topic_found.press_time = click_time;
    }

    analytics_block.update();
    feed.update();
    mediapipe.update();
    look_at = mediapipe.view_guess;
    
    if(StateManager::getInstance().resetNecessary) {
        StateManager::getInstance().resetNecessary = false;
        reset();
    }
    
    
    if(ofGetFrameNum() % 60 * 10 == 0 && base_posts.size() == 0) {
        
        loadFromAPI();
        reset();
        OSCManager::getInstance().setup();
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(255);
    ofSetColor(200);
    ofFill();
    int state = StateManager::getInstance().getState();

    for(int i = 0; i < ofGetWidth(); i+= 100) {
        for(int j = 0; j < ofGetHeight(); j+= 100) {
            ofSetColor(200);
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
        drawState("AWAITING INTERACTION");
        if(StateManager::getInstance().newPerson) {
            ofLog() << "spotted new person";
            StateManager::getInstance().setState(10);
        }
    }
    if(state == 10) {
        // consent to selling
        drawState("INFORMING CONSENT");
        stringstream ss;
        ss << std::endl << "If you agree, the Empathy Vending Aid (EVA) will buy a fraction of your empathy. By mimicking social media algorithms, the installation identifies what you care about and diminishes your emotional engagment. " << " // // ";
        ss << std::endl << "You will receive money, but care less about the chosen subject.";
        consent_transaction.draw("DO YOU WANT TO SELL YOUR", "EMPATHY", ss.str(), "Yes", "No", false);
        if(consent_transaction.accepted == 1) {
            StateManager::getInstance().setState(11);
        } else if(consent_transaction.accepted == 0) {
            reset();
        }
    }
    if(state == 11) {
        // consent to extreme content
        drawState("INFORMING CONSENT");
        consent_content.draw("WARNING", "EXTREME CONTENT", "This process may involve exposure to graphic, disturbing, or otherwise extreme imagery, including content that some users may find deeply unsettling or offensive. Viewer discretion is strongly advised. Please confirm that you are prepared to proceed.", "Yes", "No", true);
        if(consent_content.accepted == 1) {
            StateManager::getInstance().setState(20);
        } else if(consent_content.accepted == 0) {
            reset();
        }
    }
    if(state == 20) {
        // detect
        drawState("DETECTING");
        feed.draw();
        
        post * hovered = feed.getPostOnTarget(look_at);
        hovered->time_watched += (1000 / ofGetFrameRate());
        hovered->focused = true;
        
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
        ack_topic_found.draw("TOPIC DEDUCED", top, "Press continue to reduce empathy. // // We will use  techniques known to cause empathy fatigue to achieve apathy. // Pressing 'abort', or walking away will seize paiment", "Continue", "Abort", true);
        ofSetColor(0);
        ofFill();
        drawState("OPTIMISING");
        
        if(StateManager::getInstance().state_running > 60) {
            if(ack_topic_found.accepted == 1) {
                StateManager::getInstance().setState(40);
            }
            if(ack_topic_found.accepted == 0) {
                reset();
            }
        }
        
    }
    if(state == 40) {
        // enforce
        if(StateManager::getInstance().state_running > 60) {
            
            post * hovered = feed.getPostOnTarget(look_at);
            hovered->focus_time += 1;
            hovered->focused = true;
            if(hovered->post_id != -1) {
                if(hovered->topic != StateManager::getInstance().topics[7].name) {
//                    StateManager::getInstance().setEmpathy(0.003);
                } else {
                    StateManager::getInstance().setEmpathy(-0.003);
                    
                }
            }
        }
        StatisticsManager::getInstance().empathy_history.push_back(StateManager::getInstance().getEmpathy());
        feed.draw();
        drawState("ENFORCING");
        
        analytics_block.drawEmpathyBold();
        
//        if(ofGetFrameNum() % 30) {
//            analytics_block.detect_empathy(&feed.posts, feed.triggered);
//            feed.triggered = false;
//        }
        
        
        ofDrawBitmapStringHighlight(ofToString(StatisticsManager::getInstance().looking_away), ofVec2f(ofGetWidth()-40, ofGetHeight()-50));
        ofDrawBitmapStringHighlight(ofToString(StateManager::getInstance().click_through), ofVec2f(ofGetWidth()-40, ofGetHeight()-70));
        ofDrawBitmapStringHighlight(ofToString(StateManager::getInstance().getEmpathy()), ofVec2f(ofGetWidth()-40, ofGetHeight()-90));
        
        
        if( StatisticsManager::getInstance().looking_away > 200) {
            StatisticsManager::getInstance().reason = "looking away";
            StateManager::getInstance().setEmpathy(-0.003);
        }
        if(StateManager::getInstance().click_through >= 10) {
            StatisticsManager::getInstance().reason = "boredom, rapid scrolling";
            ofLog() << "rapid scrolling";
            StateManager::getInstance().setEmpathy(-1);
            StateManager::getInstance().setState(50);
            
        }
        if( StateManager::getInstance().getEmpathy() < 0.2) {
            if(StatisticsManager::getInstance().looking_away< 200) {
                StatisticsManager::getInstance().reason = "distraction, ignoring subject";
            }
            ofLog() << "look elsewhere";
            StateManager::getInstance().setState(50);
        }
    }
    if(state == 50) {
        // done
        ofSetColor(255);
        logo.draw(ofGetWidth()/2 - 110, ofGetHeight()/2-600, 250, 225);
        
        
        if(StateManager::getInstance().state_running > 40) {
            drawState("REWARD");
            
            stringstream ss;
            ss << std::endl << "The system detected signs of lowered empathy, in the form of:" << " // // ";
            ss << std::endl << StatisticsManager::getInstance().reason << " // // ";
            ss << std::endl << "Find your reward in the coin slide."  << " //";
//            ss << std::endl << "Clicks: "  << ofToString(StatisticsManager::getInstance().clicks)  << " //" ;
//            ss << std::endl << "Rapid scrolling instances: " <<  ofToString(StateManager::getInstance().click_through) << " times";
            
            ack_complete.draw("", "COMPLETE", ss.str(), "", "Restart", false);
            if(ack_complete.accepted == 0) {
                reset();
            }
        }
        std::vector<double> history = StatisticsManager::getInstance().empathy_history;
        ofPushStyle();
        ofSetColor(255);
        
        ofPath p;
        p.setFilled(false);
        p.setStrokeColor(ofColor( 60 ));
        p.setStrokeWidth(2);
        p.setColor(ofColor(255));
        double lastY = ofGetHeight()/2 - 200;
        double lastX =  ofGetWidth() / 2 - 300;
        p.moveTo(lastX, lastY);
        for(int i = 0; i < history.size(); i++) {
            double y = ofGetHeight()/2 - 100 - ofMap(history[i], 0, 1, 0, 100);
            double x = ofGetWidth() / 2 + ofMap(i, 0, history.size(), 0, 600) - 300;
            
            p.lineTo(x, y);
        }
        p.setStrokeWidth(4.);
        p.draw();
        StyleManager::getInstance().mid_font.drawString("Empathy measurement", ofGetWidth() / 2 - 300, ofGetHeight()/2 - 210);
        ofPopStyle();
    }
    
    if(state == 10) {
    }

    if(state == 50) {
        analytics_block.draw();
    }
    mediapipe.draw();
    
    
    
    if(StateManager::getInstance().debug){
        gui.draw();
        ofSetColor(StyleManager::getInstance().green);
        
        ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, ofGetHeight() - 20);
    }
    
    if(error.length() > 0) {
        ofFill();
        ofRectangle r = StyleManager::getInstance().state_font.getStringBoundingBox(error, ofGetWidth()/2, ofGetHeight()-200);
        ofSetColor(0);
        ofDrawRectangle(r.position.x - r.width/2, r.position.y, r.width, r.height);
        ofSetColor(255);
        StyleManager::getInstance().state_font.drawString(error, ofGetWidth()/2 - r.width/2, ofGetHeight()-200);
    }
}
void ofApp::drawState(string state_message) {
    ofPushMatrix();
    ofTranslate(ofGetWidth() - 50, 50);
    ofSetColor(0);
    ofRectangle r = StyleManager::getInstance().state_font.getStringBoundingBox(state_message, 0, 0);
    ofDrawRectangle(r.x - r.width, r.y + r.height, r.width, r.height);
    ofSetColor(255);
    StyleManager::getInstance().state_font.drawString(state_message, r.x-r.width, r.height);
    ofPopMatrix();
}

void ofApp::noiseField() {
  for(int i = 0; i < ofGetWidth(); i += 40) {
    for(int j = 0; j < ofGetHeight(); j+= 40) {
      float cur = (ofSignedNoise(i * 0.001, j * 0.001, 500 + ofGetFrameNum() * 0.004) + 1)/2;
        
        if( cur > 0.75 && cur < 0.8 ) {
            ofSetColor(StyleManager::getInstance().blue);
            ofFill();
            ofDrawRectangle(i-2, j-2, 4, 4);
       } else if(cur  > 0.7 && cur < 0.75) {
            ofSetColor(StyleManager::getInstance().green);
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
//    if(StateManager::getInstance().getState() == 40 || StateManager::getInstance().getState() < 20 ) {
//        if(mediapipe.view_cache.size() > 0) {
//            offset_y = y - mediapipe.view_cache[mediapipe.view_cache.size()-1].y;
//        }
//    }
    
    if(StateManager::getInstance().getState() == 20){
        
        post * clicked = feed.getPostOnTarget(ofVec2f(x, y));
        if(clicked->post_id != -1) {
            clicked->clicked += 1;
        }
    }
    time_passed_since_last =ofGetCurrentTime().seconds - last_touch;
    last_touch = ofGetCurrentTime().seconds;
    
    if(StateManager::getInstance().getState() == 40) {
        if(time_passed_since_last < 1) {
            StateManager::getInstance().click_through += 1;
            StateManager::getInstance().setEmpathy(-0.1);
            if(StateManager::getInstance().click_through > 10) {
                StateManager::getInstance().setState(50);
            }
        }
        else if(time_passed_since_last > 5) {
            StateManager::getInstance().click_through -= 5;
        }
        StatisticsManager::getInstance().click_throughs++;
        
    }
    if(StateManager::getInstance().getState() < 50 ) {
        StatisticsManager::getInstance().clicks++;
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
                    std::string mask = record["mask"].get<std::string>() + "?thumb=400x400";
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
