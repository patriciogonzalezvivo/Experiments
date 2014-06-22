#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
	
    project = new Tracer2D();
	project->setup();
	project->play();
    
    ofSetFullscreen(true);
}

//--------------------------------------------------------------
void ofApp::update(){}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_F1){
        project->stop();
        delete project;
        project = new Tracer3D();
        project->setup();
        project->play();
    } else if(key == OF_KEY_F2){
        project->stop();
        delete project;
        project = new SlitCam();
        project->setup();
        project->play();
    } else if (key == OF_KEY_F3){
        project->stop();
        delete project;
        project = new Tracer2D();
        project->setup();
        project->play();
    } else if (key == OF_KEY_F4){
        project->stop();
        delete project;
        project = new LinesMaker();
        project->setup();
        project->play();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
