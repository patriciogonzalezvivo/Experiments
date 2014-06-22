//
//  Prototype01.h
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIFog.h"
#include "UIMapBackground.h"

#include "UIShader.h"

#include "ofxMedian.h"
#include "ofxOpenCv.h"

class Prototype01 : public ofxPro {
public:
    
    string getSystemName(){
		return "Prototype01";
	}
    
    void selfSetupGuis();
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void selfSetup();
    void selfUpdate();
    void selfDraw();
    void selfPostDraw();

    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
protected:
    ofImage         img;
    ofxUIRadio      *images;
    
    vector<ofColor> palette;
    float           nPaletteColors;
    
    ofxMedian       medianFilter;
    bool            bApplyMedian;
    
    UIShader        colorEdgesShader;
    
    UIShader        colorExtractionShader;
    ofFbo           colorExtractedFbo;
    ofFloatColor    colorTarget;
    
    UIShader        complementColorShader;
    ofFbo           complementColorFbo;
    ofFloatColor    complementColorColor;
    
    float           fThirdsGrid;
};