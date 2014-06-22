//
//  SingleBrush.cpp
//
//  Created by Patricio Gonzalez Vivo on 10/4/13.
//
//

#include "SingleBrush.h"


void SingleBrush::selfSetup(){
    ofSetCircleResolution(36);
    ofEnableAlphaBlending();
    ofDisableSmoothing();

    ofSetFullscreen(true);
    
    video.initGrabber(640, 480);
    
    colorAddShader.loadFrag(getDataPath()+"shaders/colorAdd.frag");
    noiseShader.loadFrag(getDataPath()+"shaders/noise.frag");
    absorveShader.loadFrag(getDataPath()+"shaders/absorve.frag");
    displaceShader.loadFrag(getDataPath()+"shaders/displace.frag");
    edge.loadFrag(getDataPath()+"shaders/edge.frag");
    
    ofDisableArbTex();
    ofLoadImage(brushTexture, getDataPath()+"brush.png");
    ofEnableArbTex();
    
    int width = ofGetScreenWidth();
    int height = ofGetScreenHeight();
    colorAdded.allocate(width,height);
    water.allocate(width, height,GL_RGB);
    flow.allocate(width, height);
    noise.allocate(width,height);
    canvas.allocate(width, height);
    edgeFbo.allocate(640, 480);
    
    bColorSample = true;
    
    color.set(0);
}

void SingleBrush::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(colorAddShader);
    guiAdd(absorveShader);
    guiAdd(noiseShader);
    guiAdd(displaceShader);
    guiAdd(edge);
}

void SingleBrush::selfSetupSystemGui(){
    sysGui->addLabel("Brush");
    sysGui->addSlider("brush_width",0, 50, &brushWidth);
    sysGui->addSlider("number_of_lines", 1, 100, &brushNumber);
    sysGui->addSlider("line_width", 1, 10, &brush.lineWidth);
    sysGui->addSlider("damp", 0, 0.5, &brush.damp);
    sysGui->addSlider("k", 0.0, 0.5, &brush.k);
    sysGui->addSlider("Konsistency", 0.0, 0.01, &brush.kRandomness);
    sysGui->addSlider("repulsion_rad", 0.1, 20, &brush.repRad);
    sysGui->addSlider("repulsion_pct", 0.0, 1.0, &brush.repPct);
    
    sysGui->addLabel("Water");
    sysGui->addSlider("Water Amount", 1, 255, &waterAmount);
    sysGui->addSlider("Water Expansion", 1.0, 10.0, &brush.maskExpansion);
    
    sysGui->addLabel("Flow");
    sysGui->addSlider("Flow_Scale", 0.0,5.0, &flow.scale);
    sysGui->addSlider("Flow_Fade", 0.0, 1.0, &flow.fade);
    sysGui->addSlider("Flow_lambda", 0.0, 5.0, &flow.lambda);
    sysGui->addSlider("Flow_lambda", 0.0, 5.0, &flow.offset);
    
    sysGui->addLabel("Color_Palette");
    sysGui->addIntSlider("n clusters", 1, 20, &nPaletteColors);
}

float luminance(ofColor col) {
    return col.r * 0.299f *255+ col.g * 0.587f *255+ col.b * 0.114f*255;
}

ofColor getDarkest(vector<ofColor> &colors) {
    ofColor darkest;
    float minBrightness = FLT_MAX;
    for (int i=0;i<colors.size();i++) {
        ofColor c = colors[i];
        float luma = luminance(c);
        if (luma < minBrightness) {
            darkest = c;
            minBrightness = luma;
        }
    }
    return darkest;
}

float distanceBetween(ofColor a,ofColor b) {
    float hue = a.getHue()/255.0f *TWO_PI;
    float hue2 = b.getHue()/255.0f * TWO_PI;
    ofVec3f v1((cos(hue) * a.getSaturation()/255.0f),
               (sin(hue) * a.getSaturation()/255.0f), a.getBrightness()/255.0f);
    ofVec3f v2((cos(hue2) * b.getSaturation()/255.0f),
               (sin(hue2) * b.getSaturation()/255.0f), b.getBrightness()/255.0f);
    return v1.distance(v2);
}

void sortByDistance(vector<ofColor> &colors) {
    if (colors.size() == 0) {
        return;
    }
    int totalCount = colors.size();
    ofColor root = getDarkest(colors);
    // put it in the sorted list as starting element.
    vector<ofColor> sorted;
    sorted.push_back(root);
    
    // Remove the darkest color from the stack,
    
    vector<ofColor> stack(colors);
    vector<ofColor>::iterator it =  std::find(stack.begin(), stack.end(), root);
    // ofLog()<<"erase darkest "<<(*it)<<endl;
    //if(it != stack.end()){
    stack.erase(it);
    // }
    
    
    // Now find the color in the stack closest to that color.
    // Take this color from the stack and add it to the sorted list.
    // Now find the color closest to that color, etc.
    int sortedCount = 0;
    
    while (stack.size() > 1) {
        ofColor closest = stack[0];
        ofColor lastSorted = sorted[sortedCount];
        float distance = distanceBetween(closest, lastSorted);
        // ofLog()<<"searching: "<<endl;
        int foundId=0;
        for (int i = stack.size() - 1; i >= 0; i--) {
            ofColor c = stack[i];
            float d = distanceBetween(c, lastSorted);
            //  ofLog()<<"dist: "<<d<< " stack.size()  "<<stack.size()<< " sortedCount "<<sortedCount<<" totalCount "<<totalCount <<endl;
            if (d < distance) {
                closest = c;
                distance = d;
                foundId=i;
            }
        }
        sorted.push_back(closest);
        stack.erase(stack.begin()+foundId);
        
        sortedCount++;
    }
    sorted.push_back(stack[0]);
    
    colors = sorted;
}

void SingleBrush::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    bool bSamplePalette = false;
    
    if (name == "n clusters"){
        
    }
}

void SingleBrush::selfBegin(){
    colorAdded.begin();
    ofClear(0,0);
    colorAdded.end();
    
    canvas.beginBoth();
    ofClear(0,0);
    background->draw();
    canvas.endBoth();
    
    ofAddListener(ofEvents().windowResized, this, &SingleBrush::selfReSizeWindow);
    
    bClean = true;
}

void SingleBrush::selfUpdate(){
    ofPushStyle();
    ofSetColor(255);
    
    video.update();
    
    if (bClean){
        colorAdded.begin();
        ofClear(0,0);
        colorAdded.end();
        
        brush.clear();
        canvas.beginBoth();
        background->draw();
        canvas.endBoth();
        
        bColorSample = true;
        bClean = false;
    }
    
    if(bColorSample && video.isInitialized()){
        ofImage smallImg;
        smallImg.setFromPixels(video.getPixels(), video.getWidth(), video.getHeight(), OF_IMAGE_COLOR);
        smallImg.resize(video.getWidth()*0.25, video.getHeight()*0.25);
        const int colorCount = nPaletteColors;
        const int sampleCount = smallImg.getHeight() * smallImg.getWidth();
        cv::Mat colorSamples( sampleCount, 1, CV_32FC3 );
        
        // get our pixels
        unsigned char * pixels = smallImg.getPixels();
        
        // clear our list of colors
        palette.clear();
        
        // build our matrix of samples
        cv::MatIterator_<cv::Vec3f> sampleIt = colorSamples.begin<cv::Vec3f>();
        for(int i=0; i<sampleCount; i++){
            int pos = i * 3;
            *sampleIt = cv::Vec3f( pixels[pos], pixels[pos+1], pixels[pos+2] );
            sampleIt++;
        }
        
        // call kmeans
        cv::Mat labels, clusters;
        cv::kmeans( colorSamples, colorCount, labels, cv::TermCriteria(), 2, cv::KMEANS_RANDOM_CENTERS, clusters ); //cv::TermCriteria::COUNT, 8, 0
        
        for( int i = 0; i < colorCount; ++i ){
            ofColor clusterColor = ofColor( clusters.at<cv::Vec3f>(i,0)[0], clusters.at<cv::Vec3f>(i,0)[1], clusters.at<cv::Vec3f>(i,0)[2] );
            palette.push_back(clusterColor);
        }
        
        sortByDistance(palette);
        
        bColorSample = false;
    }
    
    //  UPDATE
    //
    brush.update();
    //  RENDER
    //
    {
        //  Drawing on Canvas
        //
        {
            if(brush.bDown){
                if(brush.bInk){
                    colorAdded.begin();
                    ofClear(0, 0);
                    brushTexture.bind();
                    brush.draw();
                    brushTexture.unbind();
                    colorAdded.end();
                }
                
                water.swap();
                water.dst->begin();
                water.src->draw(0,0);
                ofSetColor(waterAmount);
                brush.drawMask();
                water.dst->end();
            }
            
            int width = ofGetWidth();
            int height = ofGetHeight();
            
            canvas.swap();
            canvas.dst->begin();
            colorAddShader.begin();
            colorAddShader.getShader().setUniformTexture("backbuffer", *(canvas.src), 0);
            colorAddShader.getShader().setUniformTexture("colorAddedTexture", colorAdded, 1);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
            glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
            glTexCoord2f(width, height); glVertex3f(width, height, 0);
            glTexCoord2f(0,height);  glVertex3f(0,height, 0);
            glEnd();
            colorAddShader.end();
            canvas.dst->end();
            
            //  Water Effect
            //
            water.swap();
            water.dst->begin();
            ofClear(0,255);
            absorveShader.begin();
            water.src->draw(0, 0);
            absorveShader.end();
            water.dst->end();
            
//            normals << *(water.dst);
            flow << *(water.dst);
            
            noise.begin();
            noiseShader.begin();
            noiseShader.getShader().setUniformTexture("normalsTexture", flow, 1);
            noiseShader.getShader().setUniformTexture("waterTexture", *(water.dst), 2);
            water.dst->draw(0,0);
            noiseShader.end();
            noise.end();
            
            canvas.swap();
            canvas.dst->begin();
            displaceShader.begin();
            displaceShader.getShader().setUniformTexture("normals", noise, 1);
            displaceShader.getShader().setUniformTexture("dampMap", *(water.dst), 2);
            canvas.src->draw(0, 0);
            displaceShader.end();
            canvas.dst->end();
        }
        
        
        
    }
    ofPopStyle();
}

void SingleBrush::selfDraw(){
    
    ofSetColor(255);
    canvas.dst->draw(0,0);
//    
//    ofPushMatrix();
//    int width = video.getWidth();
//    int height = video.getHeight();
//    ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
//    ofTranslate(width*-0.5, height*-0.5);
//    edge.begin();
//    edge.getShader().setUniformTexture("tex0", video, 0);
//    glBegin(GL_QUADS);
//    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
//    glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
//    glTexCoord2f(width, height); glVertex3f(width, height, 0);
//    glTexCoord2f(0,height);  glVertex3f(0,height, 0);
//    glEnd();
//    edge.end();
//    ofPopMatrix();
    
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.95);
    float size = 10;
    float margin = 3;
    ofTranslate((-size*palette.size())+(margin*(palette.size()-1)), 0);
    for(int i = 0; i < palette.size(); i++){
        ofSetColor(palette[i]);
        ofCircle(i*(size*2.0+margin*2.0), 0, size);
    }
    
    ofPopMatrix();
    ofPopStyle();
    
    if( bDebug ){
        ofSetColor(255);
        ofFill();
        brush.drawDebug();
        ofPushMatrix();
        ofTranslate(ofGetWidth(), 0);
        ofScale(0.25, 0.25);
//        colorAdded.draw(-colorAdded.getWidth(), 0);
        video.draw(-video.getWidth()*2.0, 0, video.getWidth()*2.0, video.getHeight()*2.0);
        water.dst->draw(-water.getWidth(),colorAdded.getHeight());
//        normals.draw(-normals.getWidth(),colorAdded.getHeight()*2.0);
        flow.draw(-flow.getWidth(),colorAdded.getHeight()*2.0);
        noise.draw(-noise.getWidth(),colorAdded.getHeight()*3.0);
        ofPopMatrix();
    }
}

void SingleBrush::selfKeyPressed(ofKeyEventArgs & args){
    if (args.key == ' '){
        bClean = true;
    }
}

void SingleBrush::selfKeyReleased(ofKeyEventArgs & args){
}

void SingleBrush::selfMouseMoved(ofMouseEventArgs& data){
}

void SingleBrush::selfMousePressed(ofMouseEventArgs& data){
    brush.init(brushNumber);
    brush.setBrushWidth(brushWidth);
    brush.setColor(color, 0.1);
    brush.begin();
    brush.set(ofPoint(data.x,data.y));
    
    if(data.button == 0){
        brush.bInk = true;
    } else {
        brush.bInk = false;
    }
}

void SingleBrush::selfMouseDragged(ofMouseEventArgs& data){
    brush.set(ofPoint(data.x,data.y));
}

void SingleBrush::selfMouseReleased(ofMouseEventArgs& data){
    brush.end();
    brush.clear();
    colorAdded.begin();
    ofClear(0,0);
    colorAdded.end();
}

//    if (nFingerOnPaper == -1){
//        if (pos.distance(paperCoorners[0])<300 || pos.distance(paperCoorners[1])<300){
//            paperHolder = pos;
//            nFingerOnPaper = _finger.id;
//            return;
//        }
//    } else if(nFingerOnPaper == _finger.id){
//        if (pos.distance(paperHolder)<200){
//            paperHolder = pos;
//            
//            if(paperHolder.y < 50){
//                nFingerOnPaper = -1;
//                paperHolder.set(ofGetWidth(), ofGetHeight());
//                page.setHandAt(paperHolder);
//                page.reset();
//                background->color.bri = ofRandom(0.8,1.0);
//                
//                if(brushes[ _finger.id ]!=NULL){
//                    brushes[ _finger.id ]->end();
//                }
//                bClean = true;
//            }
//        } else {
//            nFingerOnPaper = -1;
//            paperHolder.set(ofGetWidth(), ofGetHeight());
//            page.setHandAt(paperHolder);
//            if(brushes[ _finger.id ]!=NULL){
//                brushes[ _finger.id ]->end();
//            }
//        }
//        return;
//    }


void SingleBrush::selfReSizeWindow(ofResizeEventArgs &args){
    colorAdded.allocate(args.width,args.height);
    water.allocate(args.width, args.height,GL_RGB);
    flow.allocate(args.width, args.height);
    noise.allocate(args.width,args.height);
    canvas.allocate(args.width, args.height);
}

