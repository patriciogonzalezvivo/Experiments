//
//  LeapShow.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#include "LeapShow.h"

void LeapShow::selfSetup(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    string imagesPath = getDataPath()+"images/" ;
    ofDirectory dir;
    int nFiles = dir.listDir( imagesPath );
    if(nFiles) {
        for(int i=0; i<dir.numFiles(); i++) {
            BillBoard *newFakeApp = new BillBoard();
            if ( newFakeApp->loadImage( imagesPath + "/" + dir.getFile(i).getFileName() ) ){
                newFakeApp->appName = dir.getFile(i).getBaseName();
                apps.push_back(newFakeApp);
            }
        }
    } else {
        ofLog(OF_LOG_ERROR, "Could not find fakeApps images at " + imagesPath);
    }
    
    leap.open();
    leap.startGestures();
    ofAddListener(leap.onSwipe, this, &LeapShow::leapSwipe );
    
    shader.loadFrag(getDataPath()+"shaders/shader.frag");
    
    ofEnableArbTex();
    ofLoadImage(shaderTexture, getDataPath()+"texture.jpg");
    
    bGestureStart = false;
}

void LeapShow::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(grid);
    guiAdd(shader);
    
    guiAdd(leap);
    guiAdd(nav);
    
    for (int i = 0; i < apps.size(); i++) {
        guiAdd( *apps[i] );
    }
}

void LeapShow::leapSwipe( Leap::SwipeGesture &_swipe ){
    if ( _swipe.state() == SwipeGesture::STATE_START ){
        
        ofPoint palmNormal = ofPoint(_swipe.hands()[0].palmNormal().x,
                                     _swipe.hands()[0].palmNormal().y,
                                     _swipe.hands()[0].palmNormal().z);
        ofPoint palmDir = ofPoint(_swipe.hands()[0].palmVelocity().x,
                                  _swipe.hands()[0].palmVelocity().y,
                                  _swipe.hands()[0].palmVelocity().z);
    
        
        if(!bGestureStart && palmNormal.dot(palmDir) >= 0){;
            
            if ( _swipe.direction().z < -swipeZThreshold ){
                nav.searchForCloseBack();
            } else if ( _swipe.direction().z > swipeZThreshold ){
                nav.searchForCloseFront();
            } else if ( _swipe.direction().x > swipeXThreshold ){
                if(!nav.searchForSameLevelLeft()){
                    nav.searchForAnyLeft();
                }
            } else if ( _swipe.direction().x < -swipeXThreshold ){
                if(!nav.searchForSameLevelRight()){
                    nav.searchForAnyRight();
                }
            }
            
            bGestureStart = true;
        }
        
    } else if ( _swipe.state() == SwipeGesture::STATE_UPDATE ){
        
        
    } else if ( _swipe.state() == SwipeGesture::STATE_STOP ){
        if(bGestureStart){
            bGestureStart = false;
        }
    }
    
}

void LeapShow::appGetFocus( int &_index ){

}

void LeapShow::selfSetupSystemGui(){
    sysGui->addLabel("Swipe");
    sysGui->addSlider("Swipte_Z_Threshold", 0.0, 1.0, &swipeZThreshold);
    sysGui->addSlider("Swipte_X_Threshold", 0.0, 1.0, &swipeXThreshold);
}

void LeapShow::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name == "DEBUG"){
        if (bDebug){
            camera->disableMouseInput();
        } else {
            nav.saveSettings(getDataPath()+"gui.xml","NAVIGATOR");
            camera->enableMouseInput();
        }
    }
}

void LeapShow::selfSetupRenderGui(){
    
}

void LeapShow::guiRenderEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
}

void LeapShow::selfBegin(){
    fog.linkColor( background );
    nav.loadSettings(getDataPath()+"gui.xml","NAVIGATOR");
    nav.linkApps(&apps);
    nav.linkCamera(camera->getCameraPtn());
    ofAddListener(nav.appGetFocus, this, &LeapShow::appGetFocus);
}


void LeapShow::selfUpdate(){
    nav.update();
    
    string selectedAppName;
    if ( nav.getFocus() != NULL){
        selectedAppName = nav.getFocus()->getClassName();
    }
}

void LeapShow::selfDraw(){
    ofPushMatrix();
    ofTranslate(0, -ofGetScreenHeight()*0.5-10);
    grid.draw();
    ofPopMatrix();
    
    ofRotateX(-90);
    
    for(int i = 0; i < apps.size(); i++){
        apps[i]->draw();
    }
    
}

void LeapShow::selfPostDraw(){

    shader.begin();
    shader.getShader().setUniformTexture("tex1", shaderTexture, 1);
    shader.getShader().setUniform2f("textureResolution", shaderTexture.getWidth() , shaderTexture.getHeight());
    ofxPro::selfPostDraw();
    shader.end();
    
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5, -500);
    ofScale(1,-1,1);
    ofSetColor(255,0,0);
    leap.draw();
    ofPopStyle();
    ofPopMatrix();
    
    nav.draw();
    
    if(bDebug){
        nav.debugDraw();
    }
}

void LeapShow::selfEnd(){
    for(int i = apps.size()-1; i >= 0; i--){
        delete apps[i];
        apps.erase(apps.begin()+i);
    }
    leap.close();
}

void LeapShow::selfExit(){
    leap.close();
}

void LeapShow::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key == OF_KEY_LEFT){
        if( !nav.searchForSameLevelLeft()){
            nav.searchForAnyLeft();
        }
    } else if(args.key == OF_KEY_RIGHT){
        if ( !nav.searchForSameLevelRight()){
            nav.searchForAnyRight();
        }
    } else if(args.key == OF_KEY_UP){
        if( !nav.searchForCloseFront()){
//             nav.searchForAnyFront();
        }
    } else if(args.key == OF_KEY_DOWN){
        if ( !nav.searchForCloseBack()){
//            nav.searchForAnyBack();
        }
    }
}
void LeapShow::selfKeyReleased(ofKeyEventArgs & args){
	
}

void LeapShow::selfMouseDragged(ofMouseEventArgs& data){
    ofPoint mouse = ofPoint(data.x,data.y);
    
	if (!bDebug){
        nav.mouseDragged(mouse);
    } else {
        nav.mouseDraggedDebug(mouse);
    }
}

void LeapShow::selfMouseMoved(ofMouseEventArgs& data){
	if (nav.inside( data.x, data.y) ){
        camera->disableMouseInput();
        
        ofPoint mouse = ofPoint(data.x,data.y);
        nav.mouseMove( mouse );
        
    } else {
        if (!bDebug){
            camera->enableMouseInput();
        }
    }
}

void LeapShow::selfMousePressed(ofMouseEventArgs& data){
	if (nav.inside( data.x, data.y) ){
        ofPoint mouse = ofPoint(data.x,data.y);
        nav.mousePressed( mouse );
    }
}

void LeapShow::selfMouseReleased(ofMouseEventArgs& data){
    
    if (!bDebug){
        if (nav.inside( data.x, data.y) ){
            camera->disableMouseInput();
            
            ofPoint mouse = ofPoint(data.x,data.y);
            nav.mouseReleased( mouse );
        }
    } else {
        camera->disableMouseInput();
    }
}