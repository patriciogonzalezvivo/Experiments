//
//  Prototype01.h
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIMapBackground.h"

#include "UIShader.h"

class Prototype01 : public ofxPro {
public:
    
    string getSystemName(){return "Prototype01";}
    
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
    
    void    loadFile(string _file);
    
protected:
    double  ColourDistance(ofColor e1, ofColor e2);
    
    vector<ofPoint> offsetPts;
    vector<ofPoint> offsetPointsCopy;
    vector<ofPoint> offsetPointsCopySmoothed;
    
    ofxUIRadio      *sourceFiles;
    ofImage         sourceImage;
    
    ofFloatPixels   offSet;
    ofTexture       offSetTexture;
    UIShader        flatShader;
    ofFbo           targetFbo;

    float           threshold,transitionSmoothing,timeSmoothing;
    float           thresholdSmoothing;
    float           kernelSize;
    
    float           counter;
};