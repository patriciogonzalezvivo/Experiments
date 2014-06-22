//
//  LinesMaker.h
//
//  Created by Patricio Gonzalez Vivo on 10/16/13.
//
//

#pragma once

#include "ofxPro.h"

#include "UIShader.h"
#include "UI2DGrid.h"

#include "Line.h"
#include "TimeLine.h"

class LinesMaker : public ofxPro {
public:
    
    string getSystemName(){return "LinesMaker";}
    
    void selfSetupGuis();
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void selfSetup();
    
    void selfBegin();
	
    void selfUpdate();
    
    void selfDraw();
    void selfDrawOverlay();
    void selfPostDraw();
    
	void selfEnd();
    void selfExit();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void setupBackground();
    
private:
    bool loadSecuence(string _file );
    bool saveSecuence(string _file = "none");
    
    UIShader        shader;
    ofTexture       shaderTexture;
    UI2DGrid        grid;
    
    TimeLine        player;
    Line            actualLine;
    vector<Line>    storedLines;
    ofPoint         selectOffSet;
    float           lineStroke;
    
    int             nSelected;
    int             nBookmark;
    
    bool            bCursor;
    
    int             width, height;
};