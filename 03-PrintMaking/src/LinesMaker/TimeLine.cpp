//
//  TimeLine.cpp
//  drawingTool
//
//  Created by Patricio Gonzalez Vivo on 4/27/13.
//
//

#include "TimeLine.h"

TimeLine::TimeLine(){
    height = TIMELINE_HEIGHT;
    bPlay = false;
    lastTime = -1;
}

void TimeLine::play(){
    bPlay = true;
//    lastTime = -1.0;
}

void TimeLine::pause(){
    lastTime = ofGetElapsedTimef();
    bPlay = false;
}

void TimeLine::update(){
    float now = ofGetElapsedTimef();
    float diff = now - lastTime;
    
    if (bPlay){
        if ( header >= lenght){
            lenght += diff;
        }
        header += diff;
    }
    
    lastTime = now;
}

void TimeLine::setPosition(float _pct){
    header = lenght*_pct;
}

void TimeLine::setLength(float _lengt){
    lenght = _lengt;
}

float TimeLine::getTime(){
    return header;
}

float TimeLine::getPosition(){
    return header/lenght;
}

float TimeLine::getLength(){
    return lenght;
}

float TimeLine::getLastTime(){
    return lastTime;
}

bool TimeLine::getIsPlaying(){
    return bPlay;
}

bool TimeLine::inside(int _x, int _y){
    if ( _y > ofGetHeight()-height ){
        return true;
    } else {
        return false;
    }
}

bool ascendSort(float i,float j) { return (i<j); }

void TimeLine::addBookmark(float _pct){
    
    if ( _pct == -1.0)
        _pct = getPosition();
    
    bookmarks.push_back(_pct);
    
    sort(bookmarks.begin(), bookmarks.end(), ascendSort);
}

void TimeLine::draw(){
    
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(0, ofGetHeight()-height);
    
    //  Box
    //
    ofSetColor(0,50);
    ofRect(0,0,ofGetWidth(),height);
    ofSetColor(255,100);
    ofLine(0,0,ofGetWidth(),0);
    
    //  Header
    //
    ofSetColor(255, 0,0,200);
    drawMark(getPosition(),false,false);
    
    //  Bookmarks
    //
    ofSetColor(255,0,0,100);
    for(int i = 0; i < bookmarks.size(); i++){
        drawMark(bookmarks[i],false,true);
    }
    
    ofPopMatrix();
    ofPopStyle();
}

void TimeLine::drawMark(float _pct, bool _showSec, bool _showArrow){
    float pos = _pct * (float)ofGetWidth();
    float size = 0;
    if (_showArrow){
        size = MARK_SIZE;
        ofBeginShape();
        ofVertex(pos-size,0);
        ofVertex(pos+size,0);
        ofVertex(pos,size);
        ofEndShape();
    }
    ofLine(pos, size, pos, height);
    
    if (_showSec)
        ofDrawBitmapString( ofToString( _pct * lenght ,3,7,'0'), pos-27,-3) ;
}