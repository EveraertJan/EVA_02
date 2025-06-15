#include "ephemeral_feed.h"


#include "stateManager.hpp"

void ephemeral_feed::setup(){
  mono_bold.load("font/inputmono.ttf", 16);
  font.load("font/mono2.ttf", 16);
  font.setLetterSpacing(.9);
  largeFont.load("font/dots.ttf", 24);


  zoom = 1;

  position = ofVec3f(0, 0, 0);
  target = ofVec3f(0, 0, -5);

  click.load("sounds/click.mp3");

  cube();
  comment_module.setup();
    
  dud.post_id = -1;

}


void ephemeral_feed::reset() {
    grid();
}

void ephemeral_feed::mark_feed() {
    int deduced = StateManager::getInstance().getDeduced();

    for(int i = 0; i < posts.size(); i++) {
        if(posts[i].topic_id != deduced && posts[i].topic_id != 7){
            posts[i].deleted = true;
        }
    }
}

void ephemeral_feed::trigger() {
    triggered = true;
    int state = StateManager::getInstance().getState();
    focus_id = round( ofRandom(posts.size())) ;
    
    click.play();

    if(posts.size() > 0) {
        target = posts[focus_id].position;
    }

    comment_module.update();
    
    if(state == 30 || state == 40) {
        autoTrigger = 250;
    } else {
        autoTrigger = -1;
    }
    if(state == 40) {
        StateManager::getInstance().reset_topic_focus();
        
        for(int i = 0; i < posts.size(); i++) {
            posts[i].focus_time = 0;
        }
    }
  
}
void ephemeral_feed::update(){
    int state = StateManager::getInstance().getState();
    int deduced = StateManager::getInstance().getDeduced();
    if(state == 20 && FORM != 1) {
        shuffle_feed();
        grid();
        FORM = 1;
    }
    if(state == 30 && FORM != 2) {
        sort_feed();
        cube();
        FORM = 2;
        mark_feed();
        std::cout << deduced << endl;
    }
    if(state == 40 && FORM != 1) {
//        shuffle_feed();
        collage();
        FORM = 1;
    }
    if(posts.size() > 0){
        
        target = posts[focus_id].position;
        for( int i = 0; i < posts.size(); i++) {
            posts[i].update();
        }
        
        int speed_in_frames = round(ofMap(ofGetFrameRate(), 0, 60, 1, 8));
        
        position.x  += (target.x - position.x) / speed_in_frames;
        position.y  += (target.y - position.y) / speed_in_frames;
        position.z  += (target.z - position.z) / speed_in_frames;
    }
}
void ephemeral_feed::draw(){
    int state = StateManager::getInstance().getState();
    ofPushMatrix();
    visible_posts.clear();
    if(state == 40) {
        if(posts[focus_id].target_position.x < 0) {
            ofTranslate(-200, 0);
        } else {
            ofTranslate(200, 0);
        }
        int y =floor(posts[focus_id].target_position.y);
        if( y % 1200 > 600) {
            ofTranslate(0, -200);
        } else {
            ofTranslate(0, 200);
        }
    }
    
    for(int i = 0; i < posts.size(); i++) {
        posts[i].draw(position, zoom, focus_id, state >= 40);
        if(posts[i].position.x > 0 && posts[i].position.x < ofGetWidth() && posts[i].position.y > 0 && posts[i].position.y < ofGetHeight()) {
            visible_posts.push_back(posts[i]);
        }
    }
    ofPopMatrix();
    if(state > 30) {
        comment_module.draw();
    }

}



bool comp( post & a, post & b){
   return a.post_id < b.post_id;
}

void ephemeral_feed::sort_feed() {
    ofSort(posts, &comp); // with ofSort the entire vector is passed in rather than a range
}

void ephemeral_feed::shuffle_feed(){
    ofRandomize(posts);
}

post * ephemeral_feed::getPostOnTarget(ofVec2f target) {
    ofNoFill();
    if(StateManager::getInstance().debug) {
        ofSetColor(0, 255, 255);
        ofDrawCircle(target.x, target.y, 50, -10);
    }
    int state = StateManager::getInstance().getState();
    for( int i = 0; i < posts.size(); i++) {
        if(posts[i].in_view) {
            if(state == 20) {
            
                int x = posts[i].screen_position.x + posts[i].size/2;
                int y = posts[i].screen_position.y + posts[i].size/2;
                
                if(StateManager::getInstance().debug) {
                    ofSetColor(0, 0, 255);
                    ofDrawCircle(x, y, 5, 20);
                }
                if(ofDist(x, y, target.x, target.y) <  300){
                    if(StateManager::getInstance().debug) {
                        ofSetColor(255, 0, 255);
                        ofDrawCircle(x, y, 1, 200);
                    }
                    return &posts[i];
                }
            } else if(state == 40) {
                int ox = 0;
                int oy = 0;
                if(posts[focus_id].target_position.x < 0) {
                    ox = -200;
                } else {
                    ox = 200;
                }
                int yvy =floor(posts[focus_id].target_position.y);
                if( yvy % 1200 > 600) {
                    oy = -200;
                } else {
                    oy = 200;
                }
                
                
                int x = posts[i].screen_position.x + ox;
                int y = posts[i].screen_position.y + oy;
                if(StateManager::getInstance().debug) {
                    ofSetColor(0, 0, 255);
                    ofDrawCircle(x, y, 5, 20);
                }
                if(ofDist(x, y, target.x, target.y) < 300){
                    if(StateManager::getInstance().debug) {
                        ofSetColor(255, 0, 255);
                        ofDrawCircle(x, y, 1, 200);
                    }
                    return &posts[i];
                }
                
            }
        }
        posts[i].focused = false;
    }
    return &dud;
}
void ephemeral_feed::cube() {
//  for( int i = 0; i < posts.size(); i++) {
//    int id = posts[i].post_id;
//    int n = 8;
//    int x = id % n;
//    int y = (id / n) % n;
//    int z = id / (n * n);
//    posts[i].greyScale = false;
//    posts[i].colorByTopic = true;
//    posts[i].size = 240;
//    posts[i].target_position = ofVec3f(x * 400, y * 400, z * 400);
//  }
}

void ephemeral_feed::grid() {
    vector<post> c;
    for(int i = 0; i < posts.size(); i++) {
        if(!posts[i].deleted) {
            c.push_back(posts[i]);
        }
    }
    
    posts = c;

    for( int i = 0; i < posts.size(); i++) {
        int id =i;
        int n = floor(sqrt(posts.size()));
        int x = id % n;
        int y = floor(id / n);
        int z = 0;
        posts[i].greyScale = false;
        posts[i].colorByTopic = false;
        posts[i].size = 594;
        posts[i].target_position = ofVec3f(x * 600, y * 600, z);
    }
}

void ephemeral_feed::collage() {
    vector<post> c;
    vector<post> topic;
    vector<post> extra;
    
    for(int i = 0; i < posts.size(); i++) {
        if(!posts[i].deleted) {
            if(posts[i].topic_id != 7) {
                topic.push_back(posts[i]);
            } else {
                extra.push_back(posts[i]);
            }
        }
    }
    
    ofRandomize(topic);
    ofRandomize(extra);
    ofLog() << "topic: " << topic.size();
    ofLog() << "extra: " << extra.size();
    
    for(int i = 0; i < topic.size(); i++) {
        c.push_back(topic[i]);
        if(i % 3 == 0) {
            c.push_back(extra[floor(i / 4)]);
        }
    }
    
    posts = c;
    int offsetY = 0;
    vector<ofVec2f> pos_arr;
    int offset_dist = 250;
    pos_arr.push_back(ofVec2f(-offset_dist, -offset_dist));
    pos_arr.push_back(ofVec2f(offset_dist, -offset_dist));
    pos_arr.push_back(ofVec2f(-offset_dist, offset_dist));
    pos_arr.push_back(ofVec2f(offset_dist, offset_dist));
    
    
    for( int i = 0; i < posts.size(); i++) {
        ofRandomize(pos_arr);
        ofVec2f base = pos_arr[0];
        pos_arr.erase(pos_arr.begin());
        
        int x = base.x;
        int y = base.y;
        int z = 0;
        
        posts[i].greyScale = true;
        posts[i].colorByTopic = false;
        posts[i].size = 594;
        posts[i].target_position = ofVec3f(x , y + offsetY, z);
        
        if( pos_arr.size() == 0) {
            offsetY += 2400;
            pos_arr.push_back(ofVec2f(-offset_dist, -offset_dist));
            pos_arr.push_back(ofVec2f(offset_dist, -offset_dist));
            pos_arr.push_back(ofVec2f(-offset_dist, offset_dist));
            pos_arr.push_back(ofVec2f(offset_dist, offset_dist));
        }
        
    }
}
