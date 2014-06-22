//
//  Prototype02.h
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIFog.h"
#include "UIMapBackground.h"

#include "UI3DGrid.h"

#include "ofxMedian.h"
#include "ofxOpenCv.h"

class Prototype02 : public ofx3DPro {
public:
    
    string getSystemName(){
		return "Prototype02";
	}
    
    void selfSetupGuis();
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void selfSetup();
    
    void selfBegin();

    void selfUpdate();
    
    void selfDraw();

    void selfPostDraw();
    
	void selfEnd();
    void selfExit();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
protected:
    
    UI3DGrid    grid;
    
    ofxUIRadio  *images;
    ofImage     img;
    ofxMedian   median;
    
    vector<ofPoint> spectrum;
    vector<ofFloatColor> palette;
    
    float       numColors;
    float       cubeSize, sampleSize, paletteSize;
    float       distThreshold;
    
    bool        bUpdatePalette;
};