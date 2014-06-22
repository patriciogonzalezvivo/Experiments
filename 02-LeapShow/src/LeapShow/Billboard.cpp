//
//  BillBoard.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/15/13.
//
//

#include "BillBoard.h"

void BillBoard::setupUI(){
    gui->addSlider("Angle", -PI, PI, &x);
    gui->addSlider("Radio", 0.001, 1.0, &y);
}

ofPoint BillBoard::getPos(){
    return ofPoint(y*cos(x)*1500.0, y*sin(x)*1500.0, 0);
}

void BillBoard::draw(){
    if (bEnable){
        ofPushMatrix();
        ofPushStyle();
        
        ofTranslate( getPos() );
        rotateTo( getPos() );
        ofRotate(90, 1, 0, 0);
        ofSetColor(255);
        ofImage::draw(getWidth()*-0.25,getHeight()*-0.25,getWidth()*0.5,getHeight()*0.5);
        
        ofPopStyle();
        ofPopMatrix();
    }
}

void BillBoard::guiEvent(ofxUIEventArgs &e){
    
}

void BillBoard::rotateTo(ofPoint _dir){
    ofVec3f newTarget = _dir;
    newTarget.normalize();
    ofVec3f up( 0, -1, 0 );
    ofVec3f axis = up.crossed( newTarget );
    axis.normalize();
    float angle = up.angle( newTarget );
    ofRotate( angle, axis.x, axis.y, axis.z );
}