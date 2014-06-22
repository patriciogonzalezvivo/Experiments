//
//  LeapMotion.h
//
//  Created by Patricio Gonzalez Vivo on 8/29/13.
//
//

#pragma once

#include "ofx3DPro.h"

#include "Book.h"
#include "LeapMotion.h"
#include "UI3DGrid.h"

class LeapBook : public ofx3DPro {
public:
    
    string getSystemName(){
		return "LeapBook";
	}
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void selfSetup();
    
    void selfBegin();
	void selfPresetLoaded(string presetPath);
    void selfSceneTransformation();
	
    void selfUpdate();
    
    void selfDraw();
	void selfDrawDebug();
    
	void selfEnd();
    void selfExit();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
protected:
    
    Book            book;
    LeapMotion      leap;
    UI3DGrid        grid;

};