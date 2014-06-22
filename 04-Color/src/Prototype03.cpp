//
//  Prototype03.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#include "Prototype03.h"

void Prototype03::selfSetup(){
    ofEnableAlphaBlending();
    
    lutSize = 32;
    cubeSize = 255.0;
    cubeDef  = 2;
    
    reMakeCube();
    
    bExtractPalette = false;
    median.allocate(ofGetScreenWidth(), ofGetScreenHeight());
}

void Prototype03::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(grid);
    guiAdd(lut);
}

void Prototype03::selfSetupSystemGui(){
    sysGui->addSlider("median radius", 0.0, 5.0, &median.radius);
    sysGui->addSlider("n Colors", 1, 20, &numColors);
    sysGui->addButton("Extract Palette", &bExtractPalette);
    sysGui->addButton("Clear Palette", &bClearPalette);
    sysGui->addSpacer();
    
    sysGui->addSlider("CUBE def", 1, 4, &cubeSqDef);
    sysGui->addSlider("Springiness", 0.0, 0.25, &springines);
    sysGui->addSlider("Spirng Lenght", 0.0, 1.0, &lengthPct);
    sysGui->addSlider("Atract Radio", 0.0, 255.0, &paletteAtractRadio);
    sysGui->addSlider("Atract Scale", 0.0, 0.25, &paletteAtractPct);
    sysGui->addSlider("Repel Radio", 0.0, 255.0, &paletteRepelRadio);
    sysGui->addSlider("Repel Scale", 0.0, 1.0, &paletteRepelPct);
    sysGui->addSlider("Speed", 0.0, 1.0, &speed);
    
    sysGui->addSpacer();
    sysGui->addButton("Generate LUT", &bMakeLUT);
}

void Prototype03::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if(name == "CUBE def" ){
        reMakeCube();
    } else if(name == "Springiness"|| name == "Spirng Lenght"){
        for(int i=0;i<springs.size();i++){
            springs[i].k = springines;
            springs[i].length = ((float)cubeSize/(float)cubeDef)*lengthPct;;
        }
    }
}

void Prototype03::selfSetupRenderGui(){
    rdrGui->addSlider("Particle size", 0.01, 50.0, &pointSize);
    
    rdrGui->addSlider("Drag Cubes Alpha", 0.0, 1.0, &cubesAlpha);
    rdrGui->addSlider("Drag Cubes Scale", 0.0, 1.0, &cubesScale);
    
    rdrGui->addSlider("Springs Alpha", 0.0, 1.0, &springsAlpha);
    
    rdrGui->addSlider("Palette size", 0, 1.0, &paletteSize);
    rdrGui->addSlider("Palette Alpha", 0, 1.0, &paletteAlpha);
    
    
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
            images = rdrGui->addRadio("Images", backgroundsList);
        }
    }
}

void Prototype03::guiRenderEvent(ofxUIEventArgs &e){
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
}

void Prototype03::selfBegin(){
    fog.linkColor( background );
}

void Prototype03::reMakeCube(){
    cubeDef = powf(2,cubeSqDef);
    float scale = (cubeSize/(cubeDef-1));
    
    springs.clear();
    nodes.clear();
    for (int x=0; x<points.size(); x++) {
        for(int y=0; y<points[x].size(); y++){
            for(int z=0; z<points[x][y].size(); z++){
                delete points[x][y][z];
            }
        }
    }
    points.clear();
    
    for (int x=0; x<cubeDef; x++){
        vector< vector<Particle*> > YZ;
        points.push_back(YZ);
        
        for(int y=0; y<cubeDef; y++){
            vector<Particle*> Z;
            points[x].push_back(Z);
            
            for(int z=0; z<cubeDef; z++){
                bool bNode = false;
                if ((x==0||x==cubeDef-1) ||
                    (y==0||y==cubeDef-1) ||
                    (z==0||z==cubeDef-1)){
                    bNode = true;
                }
                
                Particle *p = new Particle();
                p->set(x*scale,y*scale,z*scale);
                p->color.set(((float)x)/((float)cubeDef),
                             ((float)y)/((float)cubeDef),
                             ((float)z)/((float)cubeDef),
                             1.0);
                p->size = 0;
                points[x][y].push_back(p);
                
                if(bNode){
                    p->bFix = true;
                    nodes.push_back(p);
                }
                
                if(x>0){
                    Spring xS;
                    xS.A = points[x][y][z];
                    xS.B = points[x-1][y][z];
                    xS.k = springines;
                    xS.length = ((float)cubeSize/(float)cubeDef)*lengthPct;
                    springs.push_back(xS);
                }
                
                if(y>0){
                    Spring yS;
                    yS.A = points[x][y][z];
                    yS.B = points[x][y-1][z];
                    yS.k = springines;
                    yS.length = ((float)cubeSize/(float)cubeDef)*lengthPct;
                    springs.push_back(yS);
                }
                
                if(z>0){
                    Spring zS;
                    zS.A = points[x][y][z];
                    zS.B = points[x][y][z-1];
                    zS.k = springines;
                    zS.length = ((float)cubeSize/(float)cubeDef)*lengthPct;
                    springs.push_back(zS);
                }
            }
        }
    }
    
    cubeIndex = -1;
}

void Prototype03::selfUpdate(){
    
    if(bMakeLUT){
        
        ofImage cubeImage;
        int lutWidth =(int)lutSize*(int)lutSize;
        int lutHeight = (int)lutSize;
        cubeImage.allocate(lutWidth,lutHeight, OF_IMAGE_COLOR);
        
        for(int x=0; x<lutSize; x++){
            for(int y=0; y<lutSize; y++){
                for(int z=0; z<lutSize; z++){
                    ofPoint cubePct;
                    
                    cubePct.x = ((float)x)/(lutSize);
                    cubePct.y = ((float)y)/(lutSize);
                    cubePct.z = ((float)z)/(lutSize);
                    
                    ofPoint cubePos = cubePct;
                    cubePos *= ((int)cubeDef)-1.0;

                    float indexX,indexY,indexZ;
                    float pctX = modf(cubePos.x , &indexX);
                    float pctY = modf(cubePos.y , &indexY);
                    float pctZ = modf(cubePos.z , &indexZ);
                    
                    ofPoint A;
                    if(pctX>0.0 && indexX+1 < (int)cubeDef){
                        A.x = points[indexX][indexY][indexZ]->x*(1.0-pctX) + points[indexX+1][indexY][indexZ]->x*pctX;
                    } else {
                        A.x = points[indexX][indexY][indexZ]->x;
                    }

                    if(pctY>0.0 && indexY+1 < (int)cubeDef){
                        A.y = points[indexX][indexY][indexZ]->y*(1.0-pctY) + points[indexX][indexY+1][indexZ]->y*pctY;
                    }  else {
                        A.y = points[indexX][indexY][indexZ]->y;
                    }

                    if(pctZ>0.0 && indexZ+1 < (int)cubeDef){
                        A.z = points[indexX][indexY][indexZ]->z*(1.0-pctZ) + points[indexX][indexY][indexZ+1]->z*pctZ;
                    } else {
                        A.z = points[indexX][indexY][indexZ]->z;
                    }
                    
                    cubeImage.setColor((y*lutSize)+x, z, ofColor(A.x,A.y,A.z));
                }
            }
        }
        
        cubeImage.saveImage("GUI/LUTs/custom.png");
        lut.loadCube("GUI/LUTs/custom.png");
        
        bMakeLUT = false;
    } else if(bExtractPalette){

        median.begin();
        img.draw(0,0, img.getWidth()*0.25, img.getHeight()*0.25);
        median.end();
        median.update();
        
        ofTexture imgPalette;
        imgPalette = median.getTextureReference();
        imgPalettes.push_back(imgPalette);
        
        ofFloatPixels pixels;
        median.getTextureReference().readToPixels(pixels);
        
        const int colorCount = int(numColors);
        const int sampleCount = pixels.getHeight() * pixels.getWidth();
        cv::Mat colorSamples( sampleCount, 1, CV_32FC3 );
        cv::MatIterator_<cv::Vec3f> sampleIt = colorSamples.begin<cv::Vec3f>();
        
        for (int x = 0; x < pixels.getWidth(); x++) {
            for(int y = 0; y < pixels.getHeight(); y++ ){
                ofFloatColor sample = pixels.getColor(x, y);
                
                *sampleIt = cv::Vec3f(sample.r*255.0, sample.g*255.0, sample.b*255.0);
                sampleIt++;
            }
        }

        cv::Mat labels, clusters;
        cv::kmeans( colorSamples, colorCount, labels, cv::TermCriteria(), 2, cv::KMEANS_RANDOM_CENTERS, clusters ); //cv::TermCriteria::COUNT, 8, 0
        
        for( int i = 0; i < colorCount; ++i ){
            ofPoint c = ofPoint( clusters.at<cv::Vec3f>(i,0)[0], clusters.at<cv::Vec3f>(i,0)[1], clusters.at<cv::Vec3f>(i,0)[2] );
            palette.push_back(c);
        }
        
        //bExtractPalette = false;
    } else if (bClearPalette){
        palette.clear();
        imgPalettes.clear();
        bClearPalette = false;
    }else{
        for(int i = 0; i < springs.size();i++){
            springs[i].update();
        }
        
        for(int i =0; i < nodes.size();i++){
            for(int j = 0; j < palette.size(); j++){
                if(paletteAtractPct){
                    nodes[i]->addAttractionForce(palette[j], paletteAtractRadio, paletteAtractPct);
                }
                
                if(paletteRepelPct>0){
                    nodes[i]->addRepulsionForce(palette[j], paletteRepelRadio, paletteRepelPct);
                }
            }
        }
        
        for (int x=0; x<points.size(); x++) {
            for(int y=0; y<points[x].size(); y++){
                for(int z=0; z<points[x][y].size(); z++){
                    points[x][y][z]->update(speed);
                    
                    if(points[x][y][z]->x>255)
                        points[x][y][z]->x = 255;
                    
                    if(points[x][y][z]->y>255)
                        points[x][y][z]->y = 255;
                    
                    if(points[x][y][z]->z>255)
                        points[x][y][z]->z = 255;
                    
                    if(points[x][y][z]->x<0)
                        points[x][y][z]->x = 0;
                    
                    if(points[x][y][z]->y<0)
                        points[x][y][z]->y = 0;
                    
                    if(points[x][y][z]->z<0)
                        points[x][y][z]->z = 0;
                    
                }
            }
        }
    }
}

void Prototype03::selfDraw(){
    glEnable(GL_DEPTH_TEST);
    //materials["MATERIAL 1"]->begin();

    ofPushMatrix();
    ofTranslate(-cubeSize*0.5, -cubeSize*0.5, -cubeSize*0.5);
    
    grid.draw();
    
    float cubeAlpha = 100.0;
    ofNoFill();
    ofSetColor(255,255*cubeAlpha); ofDrawBox(cubeSize*0.5,cubeSize*0.5,cubeSize*0.5, cubeSize);
    
    ofFill();
    ofSetColor(0,cubeAlpha);          ofDrawBox(0,0,0, cubeSize*0.03);
    ofSetColor(255,cubeAlpha);        ofDrawBox(cubeSize,cubeSize,cubeSize, cubeSize*0.03);
    ofSetColor(255,0,0,cubeAlpha);    ofDrawBox(cubeSize,0,0, cubeSize*0.03);
    ofSetColor(0,255,0,cubeAlpha);    ofDrawBox(0,cubeSize,0, cubeSize*0.03);
    ofSetColor(0,0,255,cubeAlpha);    ofDrawBox(0,0,cubeSize, cubeSize*0.03);
    ofSetColor(0,255,255,cubeAlpha);  ofDrawBox(0,cubeSize,cubeSize,  cubeSize*0.03);
    ofSetColor(255,0,255,cubeAlpha);  ofDrawBox(cubeSize,0,cubeSize,  cubeSize*0.03);
    ofSetColor(255,255,0,cubeAlpha);  ofDrawBox(cubeSize,cubeSize,0,  cubeSize*0.03);
    
    
    if(cubesAlpha>0&&cubesScale>0){
        for (int i=0; i<nodes.size(); i++) {
            ofPoint n = *nodes[i];
            ofSetColor(n.x,n.y,n.z, 255.0*cubesAlpha);
            ofDrawBox(n,cubeSize*0.05*cubesScale);
        }
    }
    
    if(pointSize>0){
        glPointSize(pointSize);
        glBegin(GL_POINTS);
        for (int x=0; x<points.size(); x++) {
            for(int y=0; y<points[x].size(); y++){
                for(int z=0; z<points[x][y].size(); z++){
                    ofPoint n = *points[x][y][z];
                    ofSetColor(n.x,n.y,n.z);
                    glVertex3f(n.x,n.y,n.z);
                }
            }
        }
        glEnd();
    }

    if(paletteSize>0&&paletteAlpha>0){
        ofFill();
        for(int i = 0; i < palette.size(); i++){
            ofSetColor(palette[i].x,palette[i].y,palette[i].z,255.0*paletteAlpha);
            ofDrawSphere(palette[i], paletteRepelRadio*paletteSize);
        }
    }
    
    if(springsAlpha>0){
        for(int i = 0; i < springs.size();i++){
            ofSetColor(255,springsAlpha*255.0f);
            springs[i].draw();
        }
    }
    ofPopMatrix();
    
    //materials["MATERIAL 1"]->end();
    glDisable(GL_DEPTH_TEST);
    
    bUpdateCursor = true;
}

void Prototype03::selfPostDraw(){
    ofxPro::selfPostDraw();
    
    if (bDebug){
        ofSetColor(255);
        
        int offset = 0;
        for (int i = 0; i < imgPalettes.size(); i++) {
            imgPalettes[i].draw(offset,ofGetHeight()-imgPalettes[i].getHeight());
            offset += imgPalettes[i].getWidth();
        }
        
        
        lut.getCube().draw(ofGetWidth()-lutSize*lutSize,0);
        
        img.draw(ofGetWidth()-img.getWidth()*2.0,lutSize,
                 img.getWidth(),img.getHeight());
        
        lut.begin();
        img.draw(ofGetWidth()-img.getWidth(),lutSize,
                 img.getWidth(),img.getHeight());
        lut.end();
    }
}

void Prototype03::selfEnd(){
}

void Prototype03::selfExit(){
}

void Prototype03::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key == ' '){
        bMakeLUT = true;
    }
}

void Prototype03::selfKeyReleased(ofKeyEventArgs & args){
}

void Prototype03::selfMouseMoved(ofMouseEventArgs& data){
}

void Prototype03::selfMousePressed(ofMouseEventArgs& data){
    ofPoint mouseCursor = cursor.world+ofPoint(cubeSize*0.5,cubeSize*0.5,cubeSize*0.5);
    
    for (int i=0; i<nodes.size(); i++) {
        if(nodes[i]->distance(mouseCursor)<cubeSize*0.05){
            cubeIndex = i;
            camera->disableMouseInput();
            break;
        }
    }
}

void Prototype03::selfMouseDragged(ofMouseEventArgs& data){
    if(cubeIndex!=-1){
        if(cursor.worldValid){
            ofPoint pmouse(ofGetPreviousMouseX(),-ofGetPreviousMouseY());
            ofPoint mouse(ofGetMouseX(),-ofGetMouseY());
            ofPoint diff = getCameraRef().cameraToWorld(mouse)-getCameraRef().cameraToWorld(pmouse);
            *(nodes[cubeIndex]) +=diff*0.1;//diff.normalize()*cursor.getWorldFrameDifference().length();
        }
    }
}

void Prototype03::selfMouseReleased(ofMouseEventArgs& data){
    if(cubeIndex!=-1){
        camera->enableMouseInput();
        cubeIndex = -1;
    }
}