//
//  BillBoard.h
//
//  Created by Patricio Gonzalez Vivo on 9/15/13.
//
//

#pragma once

#include "UIClass.h"

class BillBoard : public ofPoint, public ofImage, public UIClass  {
public:
    
    void    setupUI();
    string  getClassName(){ return appName; }
    string  appName;
    
    ofPoint getPos();
    
    void    draw();
    
protected:
    void    guiEvent(ofxUIEventArgs &e);
    void    rotateTo(ofPoint _dir);
};