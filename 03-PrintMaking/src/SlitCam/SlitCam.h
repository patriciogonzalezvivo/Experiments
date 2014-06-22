//
//  SlitCam.h
//  PrintMaking
//
//  Created by Patricio Gonzalez Vivo on 11/3/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIShader.h"
#include "UIvPlotter.h"

class SlitCam : public ofxPro {
public:
    
    string getSystemName(){
		return "SlitCam";
	}
    
    void selfSetup();
    
    void selfSetupGui();
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfBegin();
    void selfUpdate();
    
    void selfDraw();
    
	void selfEnd();
    void selfExit();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
protected:
    
    ofVideoGrabber video;
    ofShader    shader;
    PingPong    pingPong;
    
    ofPoint     srcHeader;     // x = position / y = amplitud
    float       dstHeader;
    float       grainSize;
};