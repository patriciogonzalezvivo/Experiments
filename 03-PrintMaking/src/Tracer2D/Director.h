//
//  Director.h
//
//  Created by Patricio Gonzalez Vivo on 11/1/13.
//
//

#pragma once
#include "DraggableRectangle.h"
#include "UIClass.h"

class Director: public DraggableRectangle {
public:
    
    Director();
    void setFont(ofxUIFont *_font);
    
    void clear();
    ofPoint getDirForStep(int _index, float _scale);
    
    bool mousePressed(ofPoint _mouse);
    bool mouseDragged(ofPoint _mouse);
    
    float brigtness;
    void draw();
    
    vector<ofPoint> points;
    
private:
    void loadCustomValues(ofxXmlSettings &_XML);
    void saveCustomValues(ofxXmlSettings &_XML);
    ofxUIFont *font;
    
    ofPoint fromScreen(ofPoint _pos);
    ofPoint toScreen(ofPoint _pos);
    
    ofPoint lastCLick;
};