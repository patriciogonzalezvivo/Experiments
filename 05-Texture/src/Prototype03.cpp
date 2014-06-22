//
//  Prototype03.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#include "Prototype03.h"

void Prototype03::selfSetup(){
    ofSetVerticalSync(true);
    ofSetCircleResolution(36);
    
    noiseShader.loadFrag(getDataPath()+"shaders/noise.frag");
    absorveShader.loadFrag(getDataPath()+"shaders/absorve.frag");
    displaceShader.loadFrag(getDataPath()+"shaders/displace.frag");
    
    brush.loadImage(getDataPath()+"images/brush01.png");
    brush.setAnchorPercent(0.5, 0.5);
    
    loadFile(getDataPath()+"images/image.jpg");
}


void Prototype03::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(absorveShader);
    guiAdd(noiseShader);
    guiAdd(displaceShader);
}

void Prototype03::loadFile(string _file){
    bImage = image.loadImage(_file);

    if (bImage){
        int width = ofGetScreenWidth();
        int height = ofGetScreenHeight();
        
        water.allocate(width, height,GL_RGB);
        normals.allocate(width, height);
        noise.allocate(width,height);
        canvas.allocate(width, height);
    }
}

void Prototype03::selfSetupSystemGui(){
    sysGui->addSlider("Brush Size", 1, 200, &brushSize);
    sysGui->addSlider("Water Amount", 1, 255, &waterAmount);
}

void Prototype03::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
}

void Prototype03::selfSetupRenderGui(){
}

void Prototype03::guiRenderEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
}

void Prototype03::selfUpdate(){
    canvas.swap();
    
    if ( bImage ){
        ofPushStyle();
        canvas.src->begin();
        ofClear(0, 0);
        ofSetColor(0);
        ofNoFill();
        ofCircle(canvas.getWidth()*0.5, canvas.getHeight()*0.5, 100);
//        ofSetColor(255);
//        image.draw(canvas.getWidth()*0.5-image.getWidth()*0.5,canvas.getHeight()*0.5-image.getHeight()*0.5);
        canvas.src->end();
        
        canvas.dst->begin();
        ofClear(0, 0);
        ofSetColor(0);
        ofNoFill();
        ofCircle(canvas.getWidth()*0.5, canvas.getHeight()*0.5, 100);
//        ofSetColor(255);
//        image.draw(canvas.getWidth()*0.5-image.getWidth()*0.5,canvas.getHeight()*0.5-image.getHeight()*0.5);
        canvas.dst->end();
        ofPopStyle();

        water.clear(1.0);
        
        bImage = false;
    } else {
        ofSetColor(255);
        
        //  Water
        //
        water.swap();
        
        water.dst->begin();
        ofClear(0,255);
        absorveShader.begin();
        water.src->draw(0, 0);
        absorveShader.end();
        water.dst->end();
        
        normals << *(water.dst);
        
        noise.begin();
        noiseShader.begin();
        noiseShader.getShader().setUniformTexture("normalsTexture", normals, 1);
        noiseShader.getShader().setUniformTexture("waterTexture", *(water.dst), 2);
        water.dst->draw(0,0);
        noiseShader.end();
        noise.end();
        
        canvas.dst->begin();
        displaceShader.begin();
        displaceShader.getShader().setUniformTexture("normals", noise, 1);
        displaceShader.getShader().setUniformTexture("dampMap", *(water.dst), 2);
        canvas.src->draw(0, 0);
        displaceShader.end();
        canvas.dst->end();
    }
}

void Prototype03::selfDraw(){
    ofSetColor(255);
    canvas.dst->draw(0,0);
    //canvas.dst->draw(ofGetWidth()*0.5-canvas.getWidth()*0.5,ofGetHeight()*0.5-canvas.getHeight()*0.5);
    
    if ( bDebug ){
        ofSetColor(255);
        water.dst->draw(ofGetWidth()-canvas.getWidth()*0.25,canvas.getHeight()*0.0,canvas.getWidth()*0.25,canvas.getHeight()*0.25);
        normals.draw(ofGetWidth()-canvas.getWidth()*0.25,canvas.getHeight()*0.25,canvas.getWidth()*0.25,canvas.getHeight()*0.25);
        noise.draw(ofGetWidth()-canvas.getWidth()*0.25,canvas.getHeight()*0.5,canvas.getWidth()*0.25,canvas.getHeight()*0.25);
    }
}

void Prototype03::selfKeyPressed(ofKeyEventArgs & args){
    if (args.key == ' '){
        bImage = true;
    }
}

void Prototype03::selfKeyReleased(ofKeyEventArgs & args){
}

void Prototype03::selfMouseDragged(ofMouseEventArgs& data){
    canvas.src->begin();
    ofSetColor(0);
    ofNoFill();
    ofCircle(canvas.getWidth()*0.5, canvas.getHeight()*0.5, 100);
    canvas.src->end();
    
    water.dst->begin();
    ofSetColor(255);
    water.src->draw(0,0);
    ofSetColor(waterAmount);
    brush.draw(data.x,data.y,brushSize,brushSize);//ofMap(mouseX,0,width,0,ofGetWidth()),ofMap(mouseY,0,height,0,ofGetHeight()),30,30);
    water.dst->end();
}

void Prototype03::selfMouseMoved(ofMouseEventArgs& data){
}

void Prototype03::selfMousePressed(ofMouseEventArgs& data){
}

void Prototype03::selfMouseReleased(ofMouseEventArgs& data){
}