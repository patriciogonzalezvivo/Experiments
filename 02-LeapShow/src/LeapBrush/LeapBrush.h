//
//  LeapBrush.h
//
//  Created by Patricio Gonzalez Vivo on 10/4/13.
//  Reference:
//              http://www.youtube.com/watch?v=ow7eEWh37iU
//              http://www.youtube.com/watch?v=UOMI1JKfWwc
//

#pragma once

#include "ofxPro.h"

#include "PingPong.h"
#include "UIShader.h"

#include "UIPage.h"
#include "LeapMotion.h"

#include "Brush.h"

#include "ofxGaussianBlur.h"
#include "ofxNormals.h"

class LeapBrush : public ofxPro {
public:
    
    void setupBackground();
    string getSystemName(){
		return "LeapBrush";
	}
    
    void selfSetupGuis();

    void selfSetupSystemGui();
    
    void selfSetup();

    void selfBegin();
    void selfUpdate();
    void selfDraw();
    void selfEnd();
    
    void selfReSizeWindow(ofResizeEventArgs &args);
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
protected:
    void onFingerAdd(LeapFinger &_finger);
    void onFingerMove(LeapFinger &_finger);
    void onFingerDel(LeapFinger &_finger);
    
    LeapMotion      leap;
    float           pressOffSetZ;
    
    ofPoint         paperCoorners[2];
    ofPoint         paperHolder;
    int             nFingerOnPaper;
    
    map<int,BrushRef> brushes;
    ofTexture       brushTexture;
    float           brushWidth, brushNumber, brushDamp, brushK, brushKonsistency;
    float           brushRepRad, brushRepPct, lineWidth;
    
    ofColor         color;
    
    //  FILTERS
    //
    ofFbo           colorAdded;
    UIShader        colorAddShader;
    
    UIShader        absorveShader;
    PingPong        water;
    float           waterExpansion;
    float           waterAmount;
    
    ofxGaussianBlur blur;
    ofxNormals      normals;
    
    UIShader        noiseShader;
    ofFbo           noise;
    
    UIShader        displaceShader;
    PingPong        canvas;
    
    UIPage          page;
    
    bool            bClean;
};