//
//  Tracer2D.h
//  PrintMaking
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIShader.h"
#include "UIvPlotter.h"

#include "DraggableRectangle.h"
#include "Director.h"
#include "Walker.h"
#include "lineWalker.h"

#include "ofxCv.h"

class Tracer2D : public ofxPro {
public:
    
    string getSystemName(){ return "Tracer2D";}
    
    void selfSetupGuis();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetup();
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
    
    void loadImage(string _path);
    
protected:
    
    ofImage         image;
    ofxUIDropDownList *images;
    bool            bImage;
    
    UIShader        shader;
    
    ofPixels        pixels;
    
    //  Patterns Filler
    //
    vector<Walker>  walkers;
    Director        director;
    vector<ofPolyline> walkersLines;
    float           walkersSpeed;
    float           walkersSpaceBetween;
    float           walkersStep;
    bool            bWalkers;
    ofxUIDropDownList *patterns;
    string          patternSelected;
    bool            patternAdd;
    bool            patternClean;
    bool            bPatterns;
    
    //  TSP
    //
    //  Particle System
    //
    vector<Particle*>   particles;
    ofRectangle         particlesArea;
    float               particlesSpeed;
    float               particlesRepulsion;
    float               particlesMin, particlesMax;
    bool                bInvert,bPopulate,bClear;
    
//    vector<ofPoint>     points;
    ofPolyline          tspPath;
    int                 *tspRoute;
//    float               tspDensity;
    float               tspThreshold;
    int                 tspRouteLength;
    int                 tspRouteStep;
    bool                bTraceTSP;
    bool                bTsp;
    
    //  Line Tracer
    //
    void                removeIslands(ofPixels& img);
    vector<ofPolyline>  getPaths(ofPixels& img, float minGapLength = 2, int minPathLength = 0);
    vector<ofPolyline>  contourLines;
    float               fCannyThreshold1, fCannyThreshold2;
    float               minPathLength;
    float               minGapLength;
    float               contourSpace;
    float               contourSmooth;
    bool                bContour;
    bool                bTrace;
    
    // lineWalkers Filler
    //
    vector<lineWalker> lineWalkers;
    bool bLineWalkers;
    float lineSpace, lineWalkerAngle;
    
    //  Print
    //
    UIvPlotter  plotter;
    bool        bPrint;
    bool        bClearAll;
//    bool        bViewMovePaths;
};