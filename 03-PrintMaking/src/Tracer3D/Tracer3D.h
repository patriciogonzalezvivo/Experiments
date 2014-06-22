//
//  Tracer3D.h
//  PrintMaking
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIShader.h"
#include "UIvPlotter.h"

#include "Particle.h"
#include "DraggableRectangle.h"

#include "ofxCv.h"

#include "UIShell.h"

class Tracer3D : public ofx3DPro {
public:
    
    string getSystemName(){
		return "Tracer3D";
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
    
protected:
    
    UIShell         shell;
    
    //  Post-Processing
    //
    UIShader            shader;
    ofFbo               fbo;
    ofPixels            pixels;
    
    //  Particle System
    //
    vector<Particle*>   particles;
    DraggableRectangle  particlesArea;
    float               particlesSpeed;
    float               particlesRepulsion;
    float               particlesMin, particlesMax;
    bool                bInvert,bPopulate,bClear;
    
    //  TSP
    //
    void                applyTSP();
    ofPolyline          tspPath;
    int                 *tspRoute;
    float               tspThreshold;
    int                 tspRouteLength;
    int                 tspRouteStep;
    bool                bTsp;
    
    //  Line Tracer
    //
    void                removeIslands(ofPixels& img);
    vector<ofPolyline>  getPaths(ofPixels& img, float minGapLength = 2, int minPathLength = 0);
    vector<ofPolyline>  paths;
    float           fCannyThreshold1, fCannyThreshold2;
    float           minPathLength;
    float           minGapLength;
    float           pathSpace;
    float           pathSmooth;
    bool            bTrace;
    
    //  Print
    //
    UIvPlotter  plotter;
    bool        bPrint;
    bool        bViewMovePaths;
};