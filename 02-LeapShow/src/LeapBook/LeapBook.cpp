//
//  LeapBook.cpp
//
//  Created by Patricio Gonzalez Vivo on 8/29/13.
//
//

#include "LeapBook.h"

void LeapBook::selfSetup(){
    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    book.loadDirectory(getDataPath()+"pages");
    book.setPosition(ofPoint(0,0));
    
}

void LeapBook::selfSetupGui(){
    guiAdd( grid );
    guiAdd( leap );
}

void LeapBook::selfGuiEvent(ofxUIEventArgs &e){

}

void LeapBook::selfSetupSystemGui(){
}

void LeapBook::guiSystemEvent(ofxUIEventArgs &e){
    
}

void LeapBook::selfSetupRenderGui(){
}

void LeapBook::guiRenderEvent(ofxUIEventArgs &e){

}

void LeapBook::selfPresetLoaded(string presetPath){
	
}

void LeapBook::selfBegin(){
	leap.open();
}

void LeapBook::selfSceneTransformation(){
	
}

void LeapBook::selfUpdate(){

    bool bFirstFinger = true;
    for(int i = 0; i < leap.hands.size(); i++){
        for(int j = 0; j < leap.hands[i]->fingers.size(); j++){
            int id = leap.hands[i]->fingers[j].id;
            
            ofPoint pt = leap.hands[i]->fingers[j];
            
            if (bFirstFinger){
                book.hand = pt;
                bFirstFinger = false;
            }
        }
    }
    
    book.update();
}

void LeapBook::selfDraw(){
    glEnable(GL_DEPTH_TEST);

    ofPushMatrix();
    ofPushStyle();
    
    materials["MATERIAL 1"]->begin();
    
    ofSetColor(255);
    grid.draw();
    
    ofSetColor(255);
    leap.draw();
    
    ofTranslate(0, 0, 10);
    ofSetColor(255);
    book.draw(bDebug);
    
    materials["MATERIAL 1"]->end();
    
	ofPopStyle();
    ofPopMatrix();
    
    glDisable(GL_DEPTH_TEST);
}

void LeapBook::selfDrawDebug(){
}

void LeapBook::selfEnd(){
}

void LeapBook::selfExit(){
}

void LeapBook::selfKeyPressed(ofKeyEventArgs & args){
	
}
void LeapBook::selfKeyReleased(ofKeyEventArgs & args){
	
}

void LeapBook::selfMouseDragged(ofMouseEventArgs& data){
	
}

void LeapBook::selfMouseMoved(ofMouseEventArgs& data){
	
}

void LeapBook::selfMousePressed(ofMouseEventArgs& data){
	
}

void LeapBook::selfMouseReleased(ofMouseEventArgs& data){
	
}