//
//  Prototype03.h
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIFog.h"
#include "UIMapBackground.h"

#include "UI3DGrid.h"

#include "UILUT.h"

#include "ofxMedian.h"
#include "ofxOpenCv.h"

#include "Particle.h"
#include "Spring.h"

class Prototype03 : public ofx3DPro {
public:
    
    string getSystemName(){return "Prototype03";}
    
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
    vector<ofTexture> imgPalettes;
    
    UILUT       lut;
    
    void        reMakeCube();
    vector< vector< vector<Particle*> > > points;
    vector<Particle*> nodes;
    vector<Spring> springs;
    
    float       springines,lengthPct,speed;
    
    float       springsAlpha;
    float       cubeSize,cubeSqDef;
    float       cubesScale,cubesAlpha;
    int         cubeDef,cubeIndex;
    float       pointSize;
    
    void        reMakeLUT();
    float       lutSize;
    bool        bMakeLUT;
    
    vector<ofPoint> palette;
    ofxMedian   median;
    float       numColors;
    float       paletteSize,paletteAlpha;
    float       paletteAtractPct,paletteAtractRadio;
    float       paletteRepelPct,paletteRepelRadio;
    bool        bExtractPalette, bClearPalette;
};