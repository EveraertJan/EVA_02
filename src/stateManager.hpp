//
//  stateManager.hpp
//  EVA_01
//
//  Created by Jan Everaert on 07/06/2025.
//
#pragma once
#include "ofMain.h"


class StateManager {
    public:
        static StateManager& getInstance();

        void setState(int newState);
        int getState() const;
    
        void setEmpathy(float newEmpathy);
        float getEmpathy() const;
    
        void setDeduced(int newDeduced);
        int getDeduced() const;
    
        void setNoPerson(int newNoPerson);
        int getNoPerson() const;
    
        void reset_topic_focus();
    
        struct Topic {
            string name;
            string handle;
            int totalweight;
            int focus_time;
        };
        vector<Topic> topics;
        int currentPerson;
        int newPerson=false;
    
        bool debug = true;
    
        int history_size = 30;
        int click_through = 0;
    
    private:
        StateManager() = default;
        StateManager(const StateManager&) = delete;
        StateManager& operator=(const StateManager&) = delete;

        float empathy;
        int currentState = 0;
        int deduced = 0;
        
        vector<float> empathy_history;
    
        int no_person;
};
