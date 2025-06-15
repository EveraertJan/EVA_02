#include "post.h"
#include "stateManager.hpp"
#include "styleManager.hpp"

void post::setup(string path, string t, int id, string mask_path, int r, string mask_d){
    img.load(path);
    
    like.load("icons/like.png");
    like.resize(50, 50);
    
    std::vector<std::string> s = ofSplitString(mask_d, ",");
    
    mask_pos = ofVec2f(ofToFloat(s[0]), ofToFloat(s[1]));
    mask_rect = ofRectangle(ofToFloat(s[2]), ofToFloat(s[3]), ofToFloat(s[4])-ofToFloat(s[2]), ofToFloat(s[5])-ofToFloat(s[3]));
    topic = t;
    path_in = path;
    int x = id % 60;
    int y = floor(id/60);
    position = ofVec3f(ofGetWidth() / 2, ofGetHeight() / 2, 0);
    target_position = ofVec3f(x * 600, y * 600, 0);
    post_id = id;
    
    
    size = 592;
    
    mask_str = mask_path;
    rating =r;

    gs_img.allocate(img.getWidth(), img.getHeight(), img.getImageType());

    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            ofColor c = img.getColor(x, y);
            int gray = c.getBrightness();
            gs_img.setColor(x, y, ofColor(gray));
        }
    }
    gs_img.update();

    int c = 0;
    for(auto top : StateManager::getInstance().topics) {
        if(top.name == t) {
            topic_id = c;
        }
        c++;
    }
    
    blur.setup(size, size);
    loadMask();
}

void post::update(){
    float speed = 0.12;
    float multiplier = ofMap(ofGetFrameRate(), 0, 60, 1, 0);
    position.x += (target_position.x - position.x) * speed;
    position.y += (target_position.y - position.y) * speed;
    position.z += (target_position.z - position.z) * speed;
}

void post::loadMask() {
    mask.load(mask_str);
    mask.resize(img.getWidth(), img.getHeight());

}

void post::draw(ofVec3f look_at, float zoom, int focus_id, bool masked){
    int state =  StateManager::getInstance().getState();
    
//    if(!deleted) {
        ofPushMatrix();
        
        int x_offset = round(ofGetWidth() / 2 + (position.x - look_at.x)) + 4;
        int y_offset = round(ofGetHeight() / 2 + (position.y - look_at.y));
        int z_offset = round((position.z - look_at.z));
        
        screen_position.x = x_offset;
        screen_position.y = y_offset;
        screen_position.z = z_offset;
        
        if(x_offset > -800 && x_offset < ofGetWidth() + 500
           && y_offset > -800 && y_offset < ofGetHeight() + 500
           && position.z - 200 < look_at.z
           ) {
            
            ofTranslate(x_offset, y_offset, z_offset);
            
            
            if(state == 20) {
                ofTranslate(size/2, size/2, 0);
            }
            if(deleted) {
                ofSetColor(StyleManager::getInstance().red);
                ofPushMatrix();
                ofRotateDeg(45);
                ofDrawRectangle(-20, -4, 40, 8);
                ofDrawRectangle(-4, -20, 8, 40);
                ofPopMatrix();
                ofSetColor(255, 255, 255, 30);
                
            } else {
                ofSetColor(255);
            }
            if(greyScale) {
                int off_x = -mask_rect.x - (mask_rect.width/2);
                int off_y = -mask_rect.y - (mask_rect.height/2);
                if(masked) {
                    ofPushMatrix();
                    
                    mask.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
                    img.getTexture().setAlphaMask(mask.getTexture());
                    ofPushStyle();
                    ofSetColor(200, 0, 0);
                    ofSetColor(StyleManager::getInstance().red);
                    ofNoFill();
                    float scale = (500*500)/(mask_rect.width * mask_rect.height);
                    if(scale > 2) {
                        scale = 2;
                    }
                    ofScale(scale);
                    if(StateManager::getInstance().debug) {
                        ofDrawRectangle(-mask_rect.width/2, -mask_rect.height/2, mask_rect.width, mask_rect.height);
                    }
                    
                    ofPopStyle();
                    ofFill();
                    img.draw(off_x, off_y, size, size);
                    ofPopMatrix();
                } else {
                    ofSetColor(255);
                    gs_img.draw(off_x, off_y, size, size);
                }
            } else {
                img.draw(-size/2, -size/2, size, size);
            }
            
            
            
        
            ofSetColor(255);
            ofFill();
            if(StateManager::getInstance().debug) {
                StyleManager::getInstance().debug_font.drawString(topic,-size/2 + 10, -size/2 + 10);
                StyleManager::getInstance().debug_font.drawString(ofToString(post_id),-size/2 + 10, -size/2 + 30);
                std::string tws = ofToString(StateManager::getInstance().topics[topic_id].totalweight);
                StyleManager::getInstance().debug_font.drawString(tws,-size/2 + 10, -size/2 + 50);
                
                if(x_offset > -20 && x_offset < ofGetWidth() - size/2 && y_offset > 340 && y_offset < 1000 ) {
                    ofSetColor(255);
                    StyleManager::getInstance().debug_font.drawString(ofToString(screen_position.x), 30, 20);
                    StyleManager::getInstance().debug_font.drawString(ofToString(screen_position.y), 30, 40);
                }
            }
            if(state == 20 || state == 40) {
                ofPushStyle();
                
                if(clicked) {
                    ofSetColor(StyleManager::getInstance().green);
                } else {
                    ofSetColor(255, 255, 255, 150);
                    
                }
                like.draw(-like.getWidth()/2, -like.getHeight()/2);
                ofPopStyle();
                ofSetColor(255);
                ofFill();
                if(state == 20 && StateManager::getInstance().state_running < 10) {
                    StyleManager::getInstance().drawStringHighlight("Click to like the post", 0, 50, StyleManager::getInstance().mid_font, true);
                }
                StyleManager::getInstance().drawStringHighlight(ofToString(time_watched), -size/2 + 20, -size/2 + 30, StyleManager::getInstance().debug_font, false);

            }
        }
        ofPopMatrix();
        if(x_offset > -200 && x_offset < ofGetWidth() +200 && y_offset > -200 && y_offset < ofGetHeight() +200 ) {
            in_view = true;
        } else {
            in_view = false;
        }
        
//    }
    ofSetColor(0);
}

