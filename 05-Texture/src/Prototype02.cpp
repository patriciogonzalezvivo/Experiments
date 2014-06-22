//
//  Prototype02.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#include "Prototype02.h"

void Prototype02::selfSetup(){
    ofSetVerticalSync(true);
    
//    video.initGrabber(640, 480);
    offSetTexture.allocate(1,480, GL_RGB16F);
    
    distortShader.loadFrag(getDataPath()+"shaders/distort.frag");
    extractShader.loadFrag(getDataPath()+"shaders/extract.frag");
    
    image.loadImage(getDataPath()+"lena.png");
}


void Prototype02::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(distortShader);
    guiAdd(extractShader);
}

void Prototype02::selfSetupSystemGui(){

}

void Prototype02::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
}

void Prototype02::selfSetupRenderGui(){
    vector<string> filesList;
    string path = getDataPath()+"images";
    ofDirectory filesDir(path);
    if(filesDir.exists()){
		filesDir.listDir();
        int total = filesDir.getFiles().size();
        for (int i = 0; i < total; i++) {
            filesList.push_back( filesDir.getName(i) );
        }
        
        if(sourceFiles == NULL){
            sourceFiles = rdrGui->addRadio("Images", filesList);
        }
    }
}

void Prototype02::guiRenderEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name.find('.') != string::npos){
        ofxUIToggle *t = (ofxUIToggle*)e.widget;
        if (t != NULL){
            if (t->getValue()){
                ofLoadImage(texture, getDataPath()+"images/"+name);
            }
        }
    }
}

void Prototype02::selfUpdate(){
//    video.update();
    
    offSetTexture.begin();
    ofClear(0);
    extractShader.begin();
    extractShader.getShader().setUniform2f("resolution", texture.getWidth(),texture.getHeight());
    texture.draw(0, 0);
    extractShader.end();
    offSetTexture.end();
}

void Prototype02::selfDraw(){
    
    
    int width = image.getWidth();
//    int width = video.getWidth();
    int height = image.getHeight();
//    int height = video.getHeight();
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5-width, ofGetHeight()*0.5-height*0.5);
    distortShader.begin();
//    distortShader.getShader().setUniformTexture("sourceTexture", video, 0);
    distortShader.getShader().setUniformTexture("sourceTexture", image, 0);
    distortShader.getShader().setUniformTexture("offsetTexture", offSetTexture, 1);
    distortShader.getShader().setUniform2f("resolution", width, height);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(width*2.0, 0); glVertex3f(width*2.0, 0, 0);
    glTexCoord2f(width*2.0, height); glVertex3f(width*2.0, height, 0);
    glTexCoord2f(0,height);  glVertex3f(0,height, 0);
    glEnd();
    distortShader.end();
    
    ofSetColor(255);
    offSetTexture.draw(width*2.0,0,5,offSetTexture.getHeight());
    
    ofPopMatrix();
    
    if(bDebug){
        float scale = 0.5;
        texture.draw(ofGetWidth()-texture.getWidth()*scale,0,
                     texture.getWidth()*scale,texture.getHeight()*scale);
    }
    
    
}

void Prototype02::selfKeyPressed(ofKeyEventArgs & args){
}

void Prototype02::selfKeyReleased(ofKeyEventArgs & args){
}

void Prototype02::selfMouseDragged(ofMouseEventArgs& data){
}

void Prototype02::selfMouseMoved(ofMouseEventArgs& data){
}

void Prototype02::selfMousePressed(ofMouseEventArgs& data){
}

void Prototype02::selfMouseReleased(ofMouseEventArgs& data){
}