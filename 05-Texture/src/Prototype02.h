//
//  Prototype02.h
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIMapBackground.h"

#include "UIShader.h"

class Prototype02 : public ofxPro {
public:
    
    string getSystemName(){return "Prototype02";}
    
    void selfSetupGuis();
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void selfSetup();
    void selfUpdate();
    void selfDraw();

    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void audioReceived 	(float * input, int bufferSize, int nChannels);
    
protected:
//    ofVideoGrabber  video;
    ofImage         image;
    ofFbo           offSetTexture;
    
    ofxUIRadio      *sourceFiles;
    ofTexture       texture;
    UIShader        extractShader;
    
    UIShader        distortShader;
};