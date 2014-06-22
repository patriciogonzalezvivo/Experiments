//
//  Prototype01.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#include "Prototype01.h"

void Prototype01::selfSetup(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    colorEdgesShader.loadFrag(getDataPath()+"shaders/colorEdges.frag");
    colorExtractionShader.loadFrag(getDataPath()+"shaders/colorExtraction.frag");
    complementColorShader.loadFrag(getDataPath()+"shaders/complementColor.frag");
    
    medianFilter.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    colorExtractedFbo.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    complementColorFbo.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    bApplyMedian = false;
}

void Prototype01::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    
    guiAdd(colorEdgesShader);
    guiAdd(colorExtractionShader);
    guiAdd(complementColorShader);
}

void Prototype01::selfSetupSystemGui(){
    sysGui->addSlider("median radius", 0.1, 2.5, &medianFilter.radius);
    sysGui->addSlider("n clusters", 1, 20, &nPaletteColors);
    
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

vector<ofVec2f> getRYB_WHEEL(){
    vector<ofVec2f> RYB_WHEEL;
    RYB_WHEEL.push_back(ofVec2f(0, 0));
    RYB_WHEEL.push_back(ofVec2f(15, 8));
    RYB_WHEEL.push_back(ofVec2f(30, 17));
    RYB_WHEEL.push_back(ofVec2f(45, 26));
    RYB_WHEEL.push_back(ofVec2f(60, 34));
    RYB_WHEEL.push_back(ofVec2f(75, 41));
    RYB_WHEEL.push_back(ofVec2f(90, 48));
    RYB_WHEEL.push_back(ofVec2f(105, 54));
    RYB_WHEEL.push_back(ofVec2f(120, 60));
    RYB_WHEEL.push_back(ofVec2f(135, 81));
    RYB_WHEEL.push_back(ofVec2f(150, 103));
    RYB_WHEEL.push_back(ofVec2f(165, 123));
    RYB_WHEEL.push_back(ofVec2f(180, 138));
    RYB_WHEEL.push_back(ofVec2f(195, 155));
    RYB_WHEEL.push_back(ofVec2f(210, 171));
    RYB_WHEEL.push_back(ofVec2f(225, 187));
    RYB_WHEEL.push_back(ofVec2f(240, 204));
    RYB_WHEEL.push_back(ofVec2f(255, 219));
    RYB_WHEEL.push_back(ofVec2f(270, 234));
    RYB_WHEEL.push_back(ofVec2f(285, 251));
    RYB_WHEEL.push_back(ofVec2f(300, 267));
    RYB_WHEEL.push_back(ofVec2f(315, 282));
    RYB_WHEEL.push_back(ofVec2f(330, 298));
    RYB_WHEEL.push_back(ofVec2f(345, 329));
    RYB_WHEEL.push_back(ofVec2f(360, 0));
    
    return RYB_WHEEL;
}

ofColor rotateRYB(ofColor col,int theta) {
    
    float h = (float) col.getHue()/255.0 * 360;
    vector<float> hsb;
    hsb.push_back((float) col.getHue()/255.0) ;
    hsb.push_back((float) col.getSaturation()/255.0) ;
    hsb.push_back((float) col.getBrightness()/255.0) ;
    theta %= 360;
    
    float resultHue = 0;
    
    vector<ofVec2f> RYB_WHEEL = getRYB_WHEEL();
    
    for (int i = 0; i < RYB_WHEEL.size() - 1; i++) {
        ofVec2f p = RYB_WHEEL[i];
        ofVec2f q = RYB_WHEEL[i + 1];
        if (q.y < p.y) {
            q.y += 360;
        }
        if (p.y <= h && h <= q.y) {
            resultHue = p.x + (q.x - p.x) * (h - p.y) / (q.y - p.y);
            break;
        }
    }
    
    //fmod = %, ie remainder
    
    // And the user-given angle (e.g. complement).
    resultHue = fmod((resultHue + theta),360);
    
    // For the given angle, find out what hue is
    // located there on the artistic color wheel.
    for (int i = 0; i < RYB_WHEEL.size() - 1; i++) {
        ofVec2f p = RYB_WHEEL[i];
        ofVec2f q = RYB_WHEEL[i + 1];
        if (q.y < p.y) {
            q.y += 360;
        }
        if (p.x <= resultHue && resultHue <= q.x) {
            h = p.y + (q.y - p.y) * (resultHue - p.x) / (q.x - p.x);
            break;
        }
    }
    
    hsb[0] = fmod(h, 360) / 360.0f;
    
    ofColor newCol;
    newCol.setHsb(hsb[0]*255, hsb[1]*255, hsb[2]*255);
    return newCol;
}

ofColor getComplement(ofColor col){
    return rotateRYB(col,180);
}

void Prototype01::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    bool bSamplePalette = false;
    
    if (name == "n clusters"){
        bSamplePalette = true;
    } else if (name.find('.') != string::npos){
        ofxUIToggle *t = (ofxUIToggle*)e.widget;
        if (t != NULL){
            if (t->getValue()){
                img.loadImage(getDataPath()+"images/"+name);
                colorExtractedFbo.allocate(img.getWidth(),img.getHeight());
                bSamplePalette = true;
            }
        }
    }
    
    if(bSamplePalette && img.isAllocated()){
        ofImage smallImg;
        smallImg = img;
        smallImg.resize(img.getWidth()*0.25, img.getHeight()*0.25);
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
    }
    
    bApplyMedian = true;
}

void Prototype01::selfSetupRenderGui(){
    rdrGui->addLabel("Guides");
    rdrGui->addSlider("Rule of Thirds",0.0,1.0, &fThirdsGrid);
}

void Prototype01::guiRenderEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
}

void Prototype01::selfUpdate(){
    if(bApplyMedian){
        medianFilter.allocate(img.getWidth(), img.getHeight());
        medianFilter << img.getTextureReference();
        bApplyMedian = false;
    }
    
    if (colorExtractionShader.bEnable){
        colorExtractedFbo.begin();
        ofClear(0,0);
        colorExtractionShader.begin();
        colorExtractionShader.getShader().setUniform3f("colorTarget",
                                                       colorTarget.r,
                                                       colorTarget.g,
                                                       colorTarget.b);
        medianFilter.draw(0,0);
        colorExtractionShader.end();
        colorExtractedFbo.end();
    }
    
    if (complementColorShader.bEnable){
        complementColorFbo.begin();
        ofClear(0,0);
        complementColorShader.begin();
        complementColorShader.getShader().setUniform3f("colorTarget",
                                                       complementColorColor.r,
                                                       complementColorColor.g,
                                                       complementColorColor.b);
        medianFilter.draw(0,0);
        complementColorShader.end();
        complementColorFbo.end();
    }
}

void Prototype01::selfDraw(){
    ofPushMatrix();
    ofPushStyle();
    ofSetColor(255);
    ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
    colorEdgesShader.begin();
    medianFilter.draw(medianFilter.getWidth()*-0.5, medianFilter.getHeight()*-0.5);
    colorEdgesShader.end();
    
    if(colorExtractionShader.bEnable){
        colorExtractedFbo.draw(medianFilter.getWidth()*-0.5, medianFilter.getHeight()*-0.5);
    }
    
    if(complementColorShader.bEnable){
        complementColorFbo.draw(medianFilter.getWidth()*-0.5, medianFilter.getHeight()*-0.5);
    }
    
    float margin = 15;
    if(fThirdsGrid > 0.0){
        ofPushMatrix();
        ofPushStyle();
        ofTranslate(medianFilter.getWidth()*-0.5, medianFilter.getHeight()*-0.5);
        ofSetColor(255,fThirdsGrid*255.0);
        ofLine(medianFilter.getWidth()*(1.0/3.0), 0.0-margin,
               medianFilter.getWidth()*(1.0/3.0), medianFilter.getHeight()+margin);
        ofLine(medianFilter.getWidth()*(2.0/3.0), 0.0-margin,
               medianFilter.getWidth()*(2.0/3.0), medianFilter.getHeight()+margin);
        
        ofLine(0.0-margin, medianFilter.getHeight()*(1.0/3.0),
               medianFilter.getWidth()+margin,medianFilter.getHeight()*(1.0/3.0));
        ofLine(0.0-margin, medianFilter.getHeight()*(2.0/3.0),
               medianFilter.getWidth()+margin,medianFilter.getHeight()*(2.0/3.0));
        ofPopStyle();
        ofPopMatrix();
    }
    
    float paletteSize = 10;
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(medianFilter.getWidth()*0.5, medianFilter.getHeight()*0.5);
    for(int i = 0; i < palette.size(); i++){
        ofSetColor(palette[i]);
        ofCircle(margin, i*(-paletteSize*2.0-5)-paletteSize*0.5-5, paletteSize);
    }
    ofPopStyle();
    ofPopMatrix();
    
    ofSetColor(colorTarget);
    ofRect(medianFilter.getWidth()*0.5+margin-paletteSize, medianFilter.getHeight()*-0.5+paletteSize-9, paletteSize*2.0, paletteSize*2.0);
    ofSetColor(complementColorColor);
    ofRect(medianFilter.getWidth()*0.5+margin-paletteSize, medianFilter.getHeight()*-0.5+paletteSize*3.0-9, paletteSize*2.0, paletteSize*2.0);
    
    ofPopStyle();
    ofPopMatrix();
}

void Prototype01::selfPostDraw(){
    ofxPro::selfPostDraw();
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
    ofImage screen;
    screen.grabScreen(data.x-1, data.y-1, 3, 3);
    colorTarget.set(screen.getColor(1, 1));
    complementColorColor.set(getComplement(screen.getColor(1, 1)));
}

void Prototype01::selfMouseReleased(ofMouseEventArgs& data){
}