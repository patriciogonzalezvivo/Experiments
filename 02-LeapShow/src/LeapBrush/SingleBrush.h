//
//  SingleBrush.h
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

#include "Brush.h"

#include "ofxGaussianBlur.h"
#include "ofxFlow.h"

#include "ofxOpenCv.h"

class SingleBrush : public ofxPro {
public:
    
    void setupBackground();
    string getSystemName(){
		return "SingleBrush";
	}
    
    void selfSetupGuis();

    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
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
    Brush           brush;
    
    ofTexture       brushTexture;
    float           brushWidth, brushNumber;
    
    ofColor         color;
    
    //  FILTERS
    //
    ofFbo           colorAdded;
    UIShader        colorAddShader;
    
    UIShader        absorveShader;
    PingPong        water;
    float           waterExpansion;
    float           waterAmount;
    
    ofxFlow         flow;
    
    UIShader        noiseShader;
    ofFbo           noise;
    
    UIShader        displaceShader;
    PingPong        canvas;
    
    ofVideoGrabber  video;
    UIShader        edge;
    ofFbo           edgeFbo;
    
    vector<ofColor> palette;
    int             nPaletteColors;
    
    bool            bColorSample;
    bool            bClean;
};