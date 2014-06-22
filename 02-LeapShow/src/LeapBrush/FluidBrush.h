//
//  FluidBrush.h
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

#include "Brush.h"

#include "ofxFluid.h"

#include "ofxGaussianBlur.h"
#include "ofxFlow.h"

class FluidBrush : public ofxPro {
public:
    
    void setupBackground();
    string getSystemName(){
		return "FluidBrush";
	}
    
    void selfSetupGuis();

    void selfSetupSystemGui();
    
    void selfSetup();

    void selfBegin();
    void selfUpdate();
    void selfDraw();
    
    void selfReSizeWindow(ofResizeEventArgs &args);
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
protected:
    //  BRUSH
    //
    Brush           brush;
    ofTexture       brushTexture;
    float           brushWidth, brushNumber;
    ofColor         color;
    
    //  INKing
    //
    ofFbo           colorAdded;
    
    //  Paper
    //
    PingPong        water;
    float           waterExpansion;
    float           waterAmount;
    
    //  FLUID
    //
    ofxFlow         flow;
    
    ofxFluid        fluid;
    float           fluidVelPct;
    float           fluidColorPct;
    
    bool            bClean;
};