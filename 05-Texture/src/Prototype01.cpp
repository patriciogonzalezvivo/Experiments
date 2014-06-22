//
//  Prototype01.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#include "Prototype01.h"

void Prototype01::selfSetup(){
    ofSetVerticalSync(true);
    
    flatShader.loadFrag(getDataPath()+"shaders/flat.frag");
    
    loadFile("landscapes/everest-01-min.jpg");
}

void Prototype01::loadFile(string _file){
    bool bImage = sourceImage.loadImage(_file);
    if (bImage){
        targetFbo.allocate(sourceImage.getWidth(), sourceImage.getHeight()*2);
        offSetTexture.clear();
        offsetPointsCopySmoothed.resize(sourceImage.width);
        offSetTexture.allocate(sourceImage.width,1, GL_RGB16F);
        offSet.allocate(sourceImage.width, 1, 3);
    }
}

void Prototype01::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(flatShader);
}

void Prototype01::selfSetupSystemGui(){
    sysGui->addLabel("Flat");
    sysGui->addSlider("Threshold", 0.0, 255.0, &threshold);
    sysGui->addSlider("Threshold_Smoothing", 0.0, 0.2, &thresholdSmoothing);
    sysGui->addSlider("Transition_Smoothing", 0.0, 50.0, &transitionSmoothing);
    sysGui->addSlider("Time_Smoothing", 0.0, 1.0, &timeSmoothing);
    sysGui->addSlider("Kernel_Size", 0.0, 50.0, &kernelSize);
}

void Prototype01::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    counter += 100;
}

void Prototype01::selfSetupRenderGui(){
    vector<string> filesList;
    string path = "landscapes";
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

void Prototype01::guiRenderEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name.find('.') != string::npos){
        ofxUIToggle *t = (ofxUIToggle*)e.widget;
        if (t != NULL){
            if (t->getValue()){
                loadFile("landscapes/"+name);
            }
        }
    }
    
    counter += 100;
}

//  Take from http://www.compuphase.com/cmetric.htm
//
double Prototype01::ColourDistance(ofColor e1, ofColor e2){
    long rmean = ( (long)e1.r + (long)e2.r ) / 2;
    long r = (long)e1.r - (long)e2.r;
    long g = (long)e1.g - (long)e2.g;
    long b = (long)e1.b - (long)e2.b;
    
    return sqrt((((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8));
}

bool sortColor (const ofColor &a, const ofColor &b){
    return a.getBrightness() > b.getBrightness();
}

bool sortPtsByY( const ofPoint & a, const ofPoint & b){
    return a.y > b.y;
}

void Prototype01::selfUpdate(){
    
    if(counter>0){
        ofPixels srcPixels;
        srcPixels = sourceImage.getPixelsRef();
        offsetPts.clear();
        offsetPointsCopy.clear();
        
        int width = srcPixels.getWidth();
        int height = srcPixels.getHeight();
        
        for (int x = 0; x < width; x++){
            
            ofColor a = srcPixels.getColor(x, 0);

            offsetPts.push_back(ofPoint(x, 0));
            offsetPointsCopy.push_back(ofPoint(x,0));
            for(int y = 0; y < height; y++){
                
                ofFloatColor b = srcPixels.getColor(x, y);
                
                if (ColourDistance(a,b) > threshold){
                    offsetPts[x].y = y;
                    break;
                } else {
                    a.lerp(b,thresholdSmoothing);
                }
            }
        }
        
        // size of our kernel (take me +/- kernel size, so 3 = 1+ 3+3, 7 values).
        //
        for (int i = 0; i < offsetPts.size(); i++){
            
            // fill an array with values around i.
            vector < ofPoint > median;
            for (int j = i - kernelSize; j <= i + kernelSize; j++){
                int jSafe = ofClamp(j, 0, offsetPts.size() -1);
                median.push_back(offsetPts[jSafe]);
                
            }
            
            // sort the array
            sort(median.begin(), median.end(), sortPtsByY);
            
            // take the middle value
            ofPoint medVal = median[kernelSize];  // take the middle value;
            
            offsetPointsCopy[i] = medVal;
        }
        
        
        ofPolyline ptsTemp;
        ptsTemp.addVertices(offsetPointsCopy);
        ptsTemp = ptsTemp.getSmoothed(transitionSmoothing);
        
        //float pct = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 1, true);
        
        for (int x = 0; x < width; x++){
            //ptsTemp.getVertices()[x].y += 100 * sin(x /1000.0 + ofGetElapsedTimef());
            
            offsetPointsCopySmoothed [x] = (1.0-timeSmoothing) * offsetPointsCopySmoothed[x] + (timeSmoothing) * ptsTemp.getVertices()[x];
            
            ofFloatColor newOffset;
            newOffset.r = (float)offsetPointsCopySmoothed[x].y/(float)height;
            offSet.setColor(x, 0, newOffset);
        }
        
        offSetTexture.loadData(offSet);
        
        counter--;
    }
    
    {
        targetFbo.begin();
        ofClear(255);
        
        flatShader.begin();
        flatShader.getShader().setUniformTexture("offsetTexture", offSetTexture, 0);
        flatShader.getShader().setUniformTexture("sourceTexture", sourceImage.getTextureReference(), 1);
        flatShader.getShader().setUniform1f("height", (float)sourceImage.getHeight());

        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
        glTexCoord2f(targetFbo.getWidth(), 0); glVertex3f(targetFbo.getWidth(), 0, 0);
        glTexCoord2f(targetFbo.getWidth(), targetFbo.getHeight()); glVertex3f(targetFbo.getWidth(), targetFbo.getHeight(), 0);
        glTexCoord2f(0,targetFbo.getHeight());  glVertex3f(0,targetFbo.getHeight(), 0);
        glEnd();
        
        flatShader.end();
        targetFbo.end();
    }
}

void Prototype01::selfDraw(){
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
    targetFbo.draw(targetFbo.getWidth()*-0.5,targetFbo.getHeight()*-0.5);
    ofPopMatrix();
    
    if(bDebug){
        ofPushMatrix();
        ofTranslate(ofGetWidth()-targetFbo.getWidth()*0.5, 0);
        ofScale(0.5, 0.5);
        
        sourceImage.draw(0, 0);

        ofSetColor(255,0,0);
        ofPolyline line;
        line.addVertices(offsetPts);
        line.draw();
        
        ofSetColor(255,0,255);
        ofPolyline mediuanline;
        mediuanline.addVertices(offsetPointsCopy);
        mediuanline.draw();
        ofPopMatrix();
    }
}

void Prototype01::selfKeyPressed(ofKeyEventArgs & args){
    
}

void Prototype01::selfKeyReleased(ofKeyEventArgs & args){
}

void Prototype01::selfMouseDragged(ofMouseEventArgs& data){
}

void Prototype01::selfMouseMoved(ofMouseEventArgs& data){
}

void Prototype01::selfMousePressed(ofMouseEventArgs& data){
}

void Prototype01::selfMouseReleased(ofMouseEventArgs& data){
}