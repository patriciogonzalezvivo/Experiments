//
//  Prototype02.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#include "Prototype02.h"

void Prototype02::selfSetup(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    cubeSize = 200;
    distThreshold = 1.0;
    bUpdatePalette = false;
    median.allocate(ofGetScreenWidth(), ofGetScreenHeight());
}

void Prototype02::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(grid);
}

void Prototype02::selfSetupSystemGui(){
    
    sysGui->addSlider("n clusters", 1, 10, &numColors);
    sysGui->addSlider("median radius", 0.0, 5.0, &median.radius);
    sysGui->addSlider("dist threshold", 0.01, 0.5, &distThreshold);
    
    vector<string> backgroundsList;
    string path = getDataPath()+"images";
    ofDirectory backgroundsDir(path);
    if(backgroundsDir.exists()){
		backgroundsDir.listDir();
        int total = backgroundsDir.getFiles().size();
        for (int i = 0; i < total; i++) {
            backgroundsList.push_back( backgroundsDir.getName(i) );
        }
        
        if(images == NULL){
            images = sysGui->addRadio("Images", backgroundsList);
        }
    }
}

void Prototype02::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name.find('.') != string::npos){
        ofxUIToggle *t = (ofxUIToggle*)e.widget;
        if (t != NULL){
            if (t->getValue()){
                img.loadImage(getDataPath()+"images/"+name);
                
                if(img.isAllocated()){
                    median.allocate(img.getWidth()*0.25, img.getHeight()*0.25);
                }
            }
        }
    }
    
    bUpdatePalette = true;
}

void Prototype02::selfSetupRenderGui(){
    rdrGui->addSlider("palette size", 0, 50, &paletteSize);
    rdrGui->addSlider("sample size", 0.1, 50, &sampleSize);
}

void Prototype02::guiRenderEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
}

void Prototype02::selfBegin(){
    fog.linkColor( background );
}

void Prototype02::selfUpdate(){
    if(bUpdatePalette){
        
        median.begin();
        img.draw(0,0, img.getWidth()*0.25, img.getHeight()*0.25);
        median.end();
        median.update();
        
        ofFloatPixels pixels;
        median.getTextureReference().readToPixels(pixels);
    
        palette.clear();
        const int colorCount = int(numColors);
        const int sampleCount = pixels.getHeight() * pixels.getWidth();
        cv::Mat colorSamples( sampleCount, 1, CV_32FC3 );
        cv::MatIterator_<cv::Vec3f> sampleIt = colorSamples.begin<cv::Vec3f>();
        
        spectrum.clear();
        for (int x = 0; x < pixels.getWidth(); x++) {
            for(int y = 0; y < pixels.getHeight(); y++ ){
                
                ofFloatColor sample = pixels.getColor(x, y);
                
                *sampleIt = cv::Vec3f(sample.r*255.0, sample.g*255.0, sample.b*255.0);
                sampleIt++;
                
                ofPoint pColor = ofPoint(sample.r, sample.g, sample.b);
                bool toClose = false;
                for(int i = 0; i < spectrum.size(); i++){
                    if (pColor.distance(spectrum[i]) < distThreshold){
                        spectrum[i] += pColor;
                        spectrum[i] *= 0.5;
                        toClose = true;
                        break;
                    }
                }
                
                if (!toClose){
                    spectrum.push_back(pColor);
                }
            }
        }
        
        cv::Mat labels, clusters;
        cv::kmeans( colorSamples, colorCount, labels, cv::TermCriteria(), 2, cv::KMEANS_RANDOM_CENTERS, clusters ); //cv::TermCriteria::COUNT, 8, 0
        
        for( int i = 0; i < colorCount; ++i ){
            ofFloatColor c = ofColor( clusters.at<cv::Vec3f>(i,0)[0], clusters.at<cv::Vec3f>(i,0)[1], clusters.at<cv::Vec3f>(i,0)[2] );
            palette.push_back(c);
        }

        bUpdatePalette = false;
    }
    
}

void Prototype02::selfDraw(){
    glEnable(GL_DEPTH_TEST);
    materials["MATERIAL 1"]->begin();
    
    ofPushMatrix();
    ofTranslate(-cubeSize*0.5, -cubeSize*0.5, -cubeSize*0.5);
    
    grid.draw();
    
    ofNoFill();
    ofSetColor(255);
    ofDrawBox(cubeSize*0.5,cubeSize*0.5,cubeSize*0.5, cubeSize);
    
    ofFill();
    ofSetColor(0);
    ofDrawBox(0,0,0, cubeSize*0.05);
    
    ofSetColor(255);
    ofDrawBox(cubeSize,cubeSize,cubeSize, cubeSize*0.05);
    
    ofSetColor(255,0,0);
    ofDrawBox(cubeSize,0,0, cubeSize*0.05);
    
    ofSetColor(0,255,0);
    ofDrawBox(0,cubeSize,0, cubeSize*0.05);
    
    ofSetColor(0,0,255);
    ofDrawBox(0,0,cubeSize, cubeSize*0.05);
    
    ofSetColor(0,255,255);
    ofDrawBox(0,cubeSize,cubeSize, cubeSize*0.05);
    
    ofSetColor(255,0,255);
    ofDrawBox(cubeSize,0,cubeSize, cubeSize*0.05);
    
    ofSetColor(255,255,0);
    ofDrawBox(cubeSize,cubeSize,0, cubeSize*0.05);
    
    ofFill();
    for(int i = 0; i < palette.size(); i++){
        ofSetColor(palette[i]);
        ofDrawSphere(palette[i].r*cubeSize, palette[i].g*cubeSize, palette[i].b*cubeSize, paletteSize);
    }
    
    glPointSize(sampleSize);
    glBegin(GL_POINTS);
    for(int i = 0; i < spectrum.size(); i++){
        ofSetColor(ofFloatColor(spectrum[i].x,spectrum[i].y,spectrum[i].z));
        glVertex3f(spectrum[i].x*cubeSize,spectrum[i].y*cubeSize,spectrum[i].z*cubeSize);
    }
    glEnd();

    ofPopMatrix();
    
    materials["MATERIAL 1"]->end();
    glDisable(GL_DEPTH_TEST);
}

void Prototype02::selfPostDraw(){
    ofxPro::selfPostDraw();
    
    if(!bDebug){
        ofPushMatrix();
        ofSetColor(255);
        ofTranslate(0, ofGetHeight());
        img.draw(0, -img.getHeight()*0.5,img.getWidth()*0.5,img.getHeight()*0.5);
        median.draw(img.getWidth()*0.5, -img.getHeight()*0.5,img.getWidth()*0.5,img.getHeight()*0.5);
        ofPopMatrix();
    }
}

void Prototype02::selfEnd(){
}

void Prototype02::selfExit(){
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