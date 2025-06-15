#include "comments.h"
#include "styleManager.hpp"

vector<string> wrapTextCenter(string text, float setWidth, ofTrueTypeFont &font) {
    vector<string> lines;
    string currentLine, word;
    std::istringstream stream(text);

    while (stream >> word) {
        string testLine = currentLine.empty() ? word : currentLine + " " + word;
        if (font.stringWidth(testLine) <= setWidth) {
            currentLine = testLine;
        } else {
            lines.push_back(currentLine);
            currentLine = word;
        }
    }
    if (!currentLine.empty()) lines.push_back(currentLine);
    return lines;
}


void comments::setup(){
  font.load("font/mono.ttf", 52);
  largeFont.load("font/dots.ttf", 36);

  loadComments();
}
void comments::update(){
    visible_comments_arr.clear();

    string c = comments_arr[floor(ofRandom(comments_arr.size()))];
    visible_comments_arr.push_back(c);


}
void comments::draw(){
    int spacing = 4;
    int line_spacing = 36;

    ofPushMatrix();
    ofTranslate(100, 100);
    ofSetColor(0);
    
    float y = 0;
    for (const auto &line : wrapTextCenter(visible_comments_arr[0], ofGetWidth() * 0.45, font)) {
        ofRectangle r = font.getStringBoundingBox(line, 0, y);
//        ofColor blue;
//        blue.setHex(0x3FA7D6);
//        ofSetColor(blue);
        
        ofSetColor(StyleManager::getInstance().blue);
        ofDrawRectangle(r.x, r.y+10, r.width, 50);
        ofSetColor(0);
        font.drawString(line, 0, y);
        y += font.getLineHeight();
    }
    
    

    ofPopMatrix();
}



void comments::loadComments() {
  comments_arr.push_back("I feel so bad I can't do anything about this");
  comments_arr.push_back("Can anybody do anything about this?");
  comments_arr.push_back("No-one can do anything, it'll just run its course");
  comments_arr.push_back("What am I supposed to do about this? It's not like I can change anything.");
  comments_arr.push_back("I keep seeing posts like this, but I feel completely powerless to help.");
  comments_arr.push_back("Even if I wanted to take action, I wouldn't know where to start.");
  comments_arr.push_back("I wish I could do something, but it feels so far out of my hands.");
  comments_arr.push_back("Raising awareness is great, but does it even lead to real change?");
  comments_arr.push_back("No matter how much we talk about this, it feels like nothing ever happens.");
  comments_arr.push_back("I can't fix this, and neither can anyone I know. So what's the point?");
  comments_arr.push_back("Sharing this won't solve the problem, but I don't know what else to do.");
  comments_arr.push_back("The issues are just too big for one person to make any difference.");
  comments_arr.push_back("I feel like everything I try to do is just a band-aid on a much bigger wound.");

  // lack of context
  comments_arr.push_back("Nobody truly knows what's actually happening");
  comments_arr.push_back("What is going on here? I haven't seen this on the news");
  comments_arr.push_back("I wish they'd explain more about what's actually happening here.");
  comments_arr.push_back("This video is so shocking, but I don't even know the full story.");
  comments_arr.push_back("What caused this? There's no background information at all.");
  comments_arr.push_back("I feel like I'm missing key details to understand this properly.");
  comments_arr.push_back("Why is no one talking about what led up to this situation?");
  comments_arr.push_back("This post just shows the tragedy, but what happens next?");
  comments_arr.push_back("There's no explanation of who's involved or how this even started.");
  comments_arr.push_back("I don't know if this is recent or something that happened years ago.");
  comments_arr.push_back("They never tell us what happens to the people after the video ends.");
  comments_arr.push_back("Without more context, it's hard to know what to think or feel about this.");

  // lack of resolution
  comments_arr.push_back("Nothing will ever truly fix this");
  comments_arr.push_back("We can do nothing about this");
  comments_arr.push_back("We are powerless to fix this");
  comments_arr.push_back("So what happened after this? Did anyone help them?");
  comments_arr.push_back("This just leaves me wondering if things got better or worse.");
  comments_arr.push_back("They never tell us if the situation was resolved, it's so frustrating.");
  comments_arr.push_back("I hate that posts like this end without any closure.");
  comments_arr.push_back("Okay, but what are the next steps? Is there a solution in sight?");
  comments_arr.push_back("We see the problem, but there's no follow-up on what's being done.");
  comments_arr.push_back("Did anyone step in to fix this, or is it still ongoing?");
  comments_arr.push_back("It feels like these stories always end halfway through.");
  comments_arr.push_back("Without knowing how this ends, it just feels incomplete.");
  comments_arr.push_back("What's the point of sharing this if there's no update or outcome?");

  // negativity bias
  comments_arr.push_back("Is this even real");
  comments_arr.push_back("Why is it always bad news on my feed? It's so draining.");
  comments_arr.push_back("Social media only ever shows the worst side of everything.");
  comments_arr.push_back("It feels like there's no good left in the world when you see posts like this.");
  comments_arr.push_back("All I see is tragedy after tragedy, it's hard to stay positive.");
  comments_arr.push_back("The focus is always on the negatives. Do we ever see stories of hope?");
  comments_arr.push_back("This platform thrives on bad news, doesn't it?");
  comments_arr.push_back("Sometimes it feels like the world is nothing but chaos and pain.");
  comments_arr.push_back("Why do people always share the most depressing things?");
  comments_arr.push_back("Every day it's another disaster, can we ever catch a break?");
  comments_arr.push_back("It's like the algorithm only wants us to see suffering.");

  // scale
  comments_arr.push_back("I can't even comprehend this");
  comments_arr.push_back("This issue is so massive...");
  comments_arr.push_back("There's just so much going wrong in the world, it's overwhelming.");
  comments_arr.push_back("Every day it's a new crisis, how are we supposed to keep up?");
  comments_arr.push_back("It feels like there's no end to the problems we're facing.");
  comments_arr.push_back("How do you even begin to fix issues this massive?");
  comments_arr.push_back("It's exhausting to think about how many things need attention right now.");
  comments_arr.push_back("The world has so many problems, it's impossible to focus on just one.");
  comments_arr.push_back("Sometimes it feels like the entire planet is falling apart.");
  comments_arr.push_back("No matter where you look, there's another catastrophe happening.");
  comments_arr.push_back("The scale of everything going wrong just makes me want to give up.");
  comments_arr.push_back("It's like trying to empty the ocean with a bucket. there's too much to fix.");
}

std::string insertNewlineAfterSpaces(const std::string& input, int spaceLimit = 5) {
    std::string result;
    int spaceCount = 0;

    for (char c : input) {
        result += c;
        if (c == ' ') {
            spaceCount++;
            if (spaceCount == spaceLimit) {
                result += '\n';
                spaceCount = 0;
            }
        }
    }

    return result;
}
