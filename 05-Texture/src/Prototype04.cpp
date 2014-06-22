//
//  Prototype04.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#include "Prototype04.h"

void Prototype04::selfSetup(){
    ofSetVerticalSync(true);

    seamsGridShader.loadFrag(getDataPath()+"shaders/seamsGrid.frag");
    extractShader.loadFrag(getDataPath()+"shaders/extract.frag");
   
    bLoad = ofLoadImage(image, getDataPath()+"images/image.jpg");
    bMakeGrid = false;
    bGetSeams = false;
    bExtract = false;
}

void Prototype04::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(seamsGridShader);
    guiAdd(extractShader);
}

void Prototype04::selfSetupSystemGui(){
    sysGui->addSlider("Median Radius",0.0,2.0, &median.radius);
    sysGui->addSlider("Edge Radius",0.0,2.0, &edge.radius);
    sysGui->addButton("Extract", &bExtract);

}

void Prototype04::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if(name == "Median Radius" || name == "Edge Radius"){
        bMakeGrid = true;
    }
}

void Prototype04::selfSetupRenderGui(){

}

void Prototype04::guiRenderEvent(ofxUIEventArgs &e){

}

void Prototype04::selfUpdate(){
    if(bLoad){
        edge.allocate(image.getWidth(), image.getHeight());
        median.allocate(image.getWidth(),image.getHeight());
        
        seamsGrid.allocate(image.getWidth(), image.getHeight());
        seamsGrid.clear(1.0);
        
        extractTexture.allocate(1,image.getHeight(),OF_IMAGE_COLOR);
        extract.allocate(image.getWidth()-1, image.getHeight());
        
        bLoad = false;
        bMakeGrid = true;
        bGetSeams = false;
    }
    
    if(bExtract && seams.size()>0 ){
        
        extract.begin();
        extractShader.begin();
        extractShader.getShader().setUniformTexture("extractTexture", extractTexture, 1);
        extractShader.getShader().setUniformTexture("sourceTexture", image, 2);
        extractShader.getShader().setUniform2f("resolution", image.getWidth(), image.getHeight());
        image.draw(0,0);
        extractShader.end();
        extract.end();
        
        image = extract.getTextureReference();
        
        bLoad = true;
        bMakeGrid = false;
        bGetSeams = false;
        bExtract = false;
        
        seams.clear();
    } else if(bGetSeams){
        seams.clear();
        int width = seamsGrid.getWidth();
        int height = seamsGrid.getHeight();
        
        ofPixels pixels;
        seamsGrid.dst->readToPixels(pixels);
        for (int y = height-1; y >= 0; y--) {
            
            int seam ;
            
            if (y == height-1){
                seam = 0;
                int min = 256;
                for (int x = 0; x < width; x++) {
                    int brigtness = pixels.getColor(x, y).r;
                    
                    if (brigtness < min ) {
                        min = brigtness;
                        seam = x;
                    }
                }
            } else {
                seam = seams[seams.size()-1];
                int offset = seams[seams.size()-1]-1;
                int offsetLimit = 3;
                
                if (offset < 0){
                    offset = 0;
                    offsetLimit = 2;
                }
                
                int min = 256;
                for (int x = offset; x < offset+offsetLimit && x < width; x++) {
                    int brigtness = pixels.getColor(x, y).r;
                    
                    if (brigtness < min ) {
                        min = brigtness;
                        seam = x;
                    }
                }
            }
            
            seams.push_back(seam);
            extractTexture.setColor(0, y, ofFloatColor( ((float)seam)/((float)width) ));
        }
        
        extractTexture.update();
        
        bLoad = false;
        bMakeGrid = false;
        bGetSeams = false;
    } else if(bMakeGrid){
        median << image;
        median.update();
        edge << median;
        
        int height = seamsGrid.getHeight();
        for (int i = 0 ; i < height; i++) {
            ofSetColor(255);
            seamsGrid.swap();
            seamsGrid.dst->begin();
            seamsGridShader.begin();
            seamsGridShader.getShader().setUniformTexture("backbuffer", *(seamsGrid.src), 1);
            seamsGridShader.getShader().setUniformTexture("edgeTexture", edge, 2);
            seamsGridShader.getShader().setUniform2f("resolution", edge.getWidth(), edge.getHeight());
            seamsGridShader.getShader().setUniform2f("header", 0.0, (float)i);
            seamsGrid.src->draw(0,0);
            seamsGridShader.end();
            seamsGrid.dst->end();
        }
        
        bLoad = false;
        bMakeGrid = false;
        bGetSeams = true;
    }
    
}

void Prototype04::selfDraw(){
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
    image.draw(image.getWidth()*-0.5, image.getHeight()*-0.5);
    if(bDebug){
        extractTexture.draw(image.getWidth()*-0.5-5, image.getHeight()*-0.5,5,image.getHeight());
    }
    ofPopMatrix();
    
    if(bDebug){
        edge.draw(ofGetWidth()-edge.getWidth()*0.5,0,edge.getWidth()*0.5,edge.getHeight()*0.5);
        seamsGrid.dst->draw(ofGetWidth()-edge.getWidth()*0.5,edge.getHeight()*0.5,edge.getWidth()*0.5,edge.getHeight()*0.5);

        ofPushMatrix();
        ofPushStyle();
        ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
        ofTranslate(image.getWidth()*-0.5, image.getHeight()*0.5);
        ofBeginShape();
        ofSetColor(255, 0, 0);
        ofNoFill();
        ofSetLineWidth(2);
        for(int i = 0; i < seams.size(); i++){
            ofVertex(seams[i], -i);
        }
        ofEndShape();
        ofPopStyle();
        ofPopMatrix();
    }
}

void Prototype04::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key == ' '){
        bExtract = true;
    }
}

void Prototype04::selfKeyReleased(ofKeyEventArgs & args){
}

void Prototype04::selfMouseDragged(ofMouseEventArgs& data){
}

void Prototype04::selfMouseMoved(ofMouseEventArgs& data){
}

void Prototype04::selfMousePressed(ofMouseEventArgs& data){
}

void Prototype04::selfMouseReleased(ofMouseEventArgs& data){
}