//
//  Prototype03.h
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIMapBackground.h"

#include "UIShader.h"

#include "ofxBlur.h"
#include "ofxGaussianBlur.h"
#include "ofxNormals.h"

#include "PingPong.h"

class Prototype03 : public ofxPro {
public:
    
    string getSystemName(){return "Prototype03";}
    
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
    void loadFile(string _file);
    //  INPUT
    //
    ofImage         image;
    ofImage         brush;
    
    //  FILTERS
    //
    PingPong        water;
    ofxNormals      normals;
    ofFbo           noise;
    PingPong        canvas;
    
    UIShader        absorveShader;
    UIShader        noiseShader;
    UIShader        displaceShader;
    
    float           brushSize,waterAmount;
    bool            bImage;
};