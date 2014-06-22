//
//  Prototype04.h
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIMapBackground.h"

#include "PingPong.h"
#include "UIShader.h"

#include "ofxMedian.h"
#include "ofxEdgeDetect.h"

class Prototype04 : public ofxPro {
public:
    
    string getSystemName(){return "Prototype04";}
    
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
    
protected:
    ofTexture       image;
    
    ofxMedian       median;
    ofxEdgeDetect   edge;
    
    UIShader        seamsGridShader;
    PingPong        seamsGrid;
    vector<int>     seams;
    
    ofImage         extractTexture;
    ofFbo           extract;
    UIShader        extractShader;
    
    bool            bLoad;
    bool            bMakeGrid;
    bool            bGetSeams;
    bool            bExtract;
};