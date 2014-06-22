//
//  LeapShow.h
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#pragma once

#include "ofxPro.h"
#include "UIFog.h"
#include "UIMapBackground.h"

#include "UIShader.h"

#include "LeapMotion.h"

#include "UI3DGrid.h"

#include "BillBoard.h"
#include "Navigator.h"

#include "UIText.h"

class LeapShow : public ofx3DPro {
public:
    
    string getSystemName(){
		return "LeapShow";
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
    
    LeapMotion  leap;
    void        leapSwipe( Leap::SwipeGesture &swipe );
    void        appGetFocus( int &_index );
    
    vector<BillBoard*>   apps;
    
    float               swipeZThreshold, swipeXThreshold;
    bool                bGestureStart;
    
    UI3DGrid            grid;
    
    Navigator           nav;
    
    UIShader            shader;
    ofTexture           shaderTexture;
};