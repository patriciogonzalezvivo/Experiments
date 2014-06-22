//
//  LeapBrush.cpp
//
//  Created by Patricio Gonzalez Vivo on 10/4/13.
//
//

#include "LeapBrush.h"


void LeapBrush::selfSetup(){
    ofSetCircleResolution(36);
    ofEnableAlphaBlending();
    ofDisableSmoothing();

    pressOffSetZ = 0;
    nFingerOnPaper = -1;
    ofSetFullscreen(true);
    
    colorAddShader.loadFrag(getDataPath()+"shaders/colorAdd.frag");
    noiseShader.loadFrag(getDataPath()+"shaders/noise.frag");
    absorveShader.loadFrag(getDataPath()+"shaders/absorve.frag");
    displaceShader.loadFrag(getDataPath()+"shaders/displace.frag");
    
    ofDisableArbTex();
    ofLoadImage(brushTexture, getDataPath()+"brush.png");
    ofEnableArbTex();
    
    int width = ofGetScreenWidth();
    int height = ofGetScreenHeight();
    colorAdded.allocate(width,height);
    water.allocate(width, height,GL_RGB);
    normals.allocate(width, height);
    noise.allocate(width,height);
    canvas.allocate(width, height);
    blur.allocate(width, height);
    
    page.set(0,0,width,height);
    paperCoorners[0].set(width, height);
    paperCoorners[1].set(0, height);
    
    color.set(0);
}

void LeapBrush::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(leap);
    guiAdd(colorAddShader);
    guiAdd(absorveShader);
    guiAdd(noiseShader);
    guiAdd(displaceShader);
    guiAdd(page);
}

void LeapBrush::selfSetupSystemGui(){
    sysGui->addLabel("Brush");
    sysGui->addSlider("brush_width",0, 50, &brushWidth);
    sysGui->addSlider("number_of_lines", 1, 100, &brushNumber);
    sysGui->addSlider("line_width", 1, 10, &lineWidth);
    sysGui->addSlider("damp", 0, 0.5, &brushDamp);
    sysGui->addSlider("k", 0.0, 0.5, &brushK);
    sysGui->addSlider("Konsistency", 0.0, 1.0, &brushKonsistency);
    sysGui->addSlider("repulsion_rad", 0.1, 20, &brushRepRad);
    sysGui->addSlider("repulsion_pct", 0.0, 1.0, &brushRepPct);
    
    sysGui->addLabel("Water");
    sysGui->addSlider("Water Amount", 1, 255, &waterAmount);
    sysGui->addSlider("Water Expansion", 1.0, 5.0, &waterExpansion);
}

void LeapBrush::selfBegin(){
    colorAdded.begin();
    ofClear(0,0);
    colorAdded.end();
    
    canvas.beginBoth();
    ofClear(0,0);
    background->draw();
    canvas.endBoth();
    
    page.setHandAt(ofPoint(ofGetWidth(),ofGetHeight()*0.5));
    
    leap.open();
    ofAddListener(leap.fingerAdd, this, &LeapBrush::onFingerAdd);
    ofAddListener(leap.fingerMove, this, &LeapBrush::onFingerMove);
    ofAddListener(leap.fingerDel, this, &LeapBrush::onFingerDel);
    ofAddListener(ofEvents().windowResized, this, &LeapBrush::selfReSizeWindow);
    
    bClean = true;
}

void LeapBrush::selfUpdate(){
    ofPushStyle();
    ofSetColor(255);
    
    if(nFingerOnPaper != -1 ||
       paperHolder.distance(paperCoorners[0])>300||
       paperHolder.distance(paperCoorners[1])>300){
        page.setHandAt(paperHolder);
    } else {
        page.setHandAt(ofPoint(ofGetWidth(), ofGetHeight()));
    }
    
    if (bClean){
        for (map<int,BrushRef>::iterator it=brushes.begin(); it!=brushes.end(); ++it){
            if (it->second != NULL){
                it->second->clear();
            }
        }
        
        canvas.beginBoth();
        background->draw();
        canvas.endBoth();
        
        bClean = false;
    }
    
    //  UPDATE
    //
    page.update();
    for (map<int,BrushRef>::iterator it=brushes.begin(); it!=brushes.end(); ++it){
        if(it->second != NULL){
            if(it->second->bKill){
                brushes.erase(it);
            } else {
                it->second->update();
            }
        }
    }
    
    //  RENDER
    //
    {
        //  Drawing on Canvas
        //
        if(brushes.size()>0){

            colorAdded.begin();
            ofClear(0, 0);
            brushTexture.bind();
            for (map<int,BrushRef>::iterator it=brushes.begin(); it!=brushes.end(); ++it){
                if (it->second != NULL){
                    if (it->second->bDown ){
                        it->second->draw();
                    }
                }
            }
            brushTexture.unbind();
            colorAdded.end();
            
            water.swap();
            water.dst->begin();
            water.src->draw(0,0);
            ofSetColor(waterAmount);
            for (map<int,BrushRef>::iterator it=brushes.begin(); it!=brushes.end(); ++it){
                if (it->second != NULL){
                    if (it->second->bDown ){
                        it->second->drawMask();
                    }
                }
            }
            water.dst->end();
            
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
        }
        
        //  Water Effect
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

        canvas.swap();
        canvas.dst->begin();
        displaceShader.begin();
        displaceShader.getShader().setUniformTexture("normals", noise, 1);
        displaceShader.getShader().setUniformTexture("dampMap", *(water.dst), 2);
        canvas.src->draw(0, 0);
        displaceShader.end();
        canvas.dst->end();
        
    }
    ofPopStyle();
}

void LeapBrush::selfDraw(){

    
    glEnable(GL_DEPTH_TEST);
    ofSetColor(255, page.getTransition()*255);
    canvas.dst->getTextureReference().bind();
    page.getMesh().drawFaces();
    canvas.dst->getTextureReference().unbind();
    glDisable(GL_DEPTH_TEST);
    if(page.bDebug){
        page.getMesh().drawWireframe();
    }
    
    ofPushMatrix();
    ofTranslate(0, 0,1);
    
    ofPushStyle();
    ofNoFill();
    float minZ = 0;
    for (int i = 0; i < leap.hands.size(); i++) {
        for (int j = 0; j < leap.hands[i]->fingers.size(); j++) {
            float z = leap.hands[i]->fingers[j].z;
            if ( z < minZ ){
                minZ = z;
            }
        }
    }
    pressOffSetZ = minZ*0.8;
    for (int i = 0; i < leap.hands.size(); i++) {
        for (int j = 0; j < leap.hands[i]->fingers.size(); j++) {
            ofPoint pos = ofPoint(leap.hands[i]->fingers[j].x+ofGetWidth()*0.5,
                                  -leap.hands[i]->fingers[j].y+ofGetHeight()*0.5);
            ofPushStyle();
            if(pos.distance(paperCoorners[0])<500){
                ofNoFill();
                ofSetLineWidth(10);
                ofSetColor(255,ofMap(pos.distance(paperCoorners[0]), 500, 200, 0.0, 255,true));
                ofCircle(paperCoorners[0], 200);
            } else if(pos.distance(paperCoorners[1])<500){
                ofNoFill();
                ofSetLineWidth(10);
                ofSetColor(255,ofMap(pos.distance(paperCoorners[1]), 500, 200, 0.0, 255,true));
                ofCircle(paperCoorners[1], 200);
            }
            ofPopStyle();
            
            float radio = leap.hands[i]->fingers[j].z-minZ;
            if (radio < 50){
                ofPushStyle();
                ofNoFill();
                ofSetLineWidth( (radio/50.0)*10.0 );
                ofSetColor( color ,250-(radio*5));
                ofCircle(pos, brushWidth*0.5 + ((radio>0)?radio+1:1) );
                ofFill();
                ofCircle(pos, brushWidth*0.5);
                ofPopStyle();
            }
        }
    }
    ofPopStyle();
    
    if( bDebug ){
        ofSetColor(255);
        ofFill();

        for (map<int,BrushRef>::iterator it=brushes.begin(); it!=brushes.end(); ++it){
            if (it->second != NULL){
                it->second->drawDebug();
            }
        }
        
        ofPushMatrix();
        ofTranslate(ofGetWidth(), 0);
        ofScale(0.25, 0.25);
        colorAdded.draw(-colorAdded.getWidth(), 0);
        water.dst->draw(-water.getWidth(),colorAdded.getHeight());
        normals.draw(-normals.getWidth(),colorAdded.getHeight()*2.0);
        noise.draw(-noise.getWidth(),colorAdded.getHeight()*3.0);
        ofPopMatrix();
    }
    ofPopMatrix();
}

void LeapBrush::selfEnd(){
    leap.close();
}

void LeapBrush::selfKeyPressed(ofKeyEventArgs & args){
    if (args.key == ' '){
        bClean = true;
    }
}

void LeapBrush::selfKeyReleased(ofKeyEventArgs & args){
}

void LeapBrush::selfMouseMoved(ofMouseEventArgs& data){
}

void LeapBrush::selfMousePressed(ofMouseEventArgs& data){
}

void LeapBrush::selfMouseDragged(ofMouseEventArgs& data){
}

void LeapBrush::selfMouseReleased(ofMouseEventArgs& data){
}

void LeapBrush::onFingerAdd( LeapFinger &_finger ){
    //  Create brush instance
    //
    if ( brushes[ _finger.id ] == NULL ){
        BrushRef newBrush (new Brush());
        brushes[ _finger.id ] = newBrush;
    }
}

void LeapBrush::onFingerMove( LeapFinger &_finger){
    ofPoint pos = ofPoint(_finger.x+ofGetWidth()*0.5,
                          -_finger.y+ofGetHeight()*0.5);
    
    float radio = (_finger.z-pressOffSetZ)*0.5;
    
    if (nFingerOnPaper == -1){
        if (pos.distance(paperCoorners[0])<300 || pos.distance(paperCoorners[1])<300){
            paperHolder = pos;
            nFingerOnPaper = _finger.id;
            return;
        }
    } else if(nFingerOnPaper == _finger.id){
        if (pos.distance(paperHolder)<200){
            paperHolder = pos;
            
            if(paperHolder.y < 50){
                nFingerOnPaper = -1;
                paperHolder.set(ofGetWidth(), ofGetHeight());
                page.setHandAt(paperHolder);
                page.reset();
                background->color.bri = ofRandom(0.8,1.0);
                
                if(brushes[ _finger.id ]!=NULL){
                    brushes[ _finger.id ]->end();
                }
                bClean = true;
            }
        } else {
            nFingerOnPaper = -1;
            paperHolder.set(ofGetWidth(), ofGetHeight());
            page.setHandAt(paperHolder);
            if(brushes[ _finger.id ]!=NULL){
                brushes[ _finger.id ]->end();
            }
        }
        return;
    }
    
    if ( brushes[ _finger.id ] == NULL ){
        BrushRef newBrush (new Brush());
        brushes[ _finger.id ] = newBrush;
    }
    
    //  If is close
    //
    if (radio < 5 && nFingerOnPaper == -1){
        
        //  ... and not already down the brush
        //
        if ( !brushes[ _finger.id ]->bDown ){
            brushes[ _finger.id ]->init(brushNumber);
            brushes[ _finger.id ]->setBrushWidth(brushWidth);
            brushes[ _finger.id ]->setLineWidth(lineWidth);
            brushes[ _finger.id ]->setColor(color,0.1);
            
            brushes[ _finger.id ]->damp = brushDamp;
            brushes[ _finger.id ]->k = brushK;
            brushes[ _finger.id ]->kRandomness = brushKonsistency*0.05;
            brushes[ _finger.id ]->repPct = brushRepPct;
            brushes[ _finger.id ]->repRad = brushRepRad;
            brushes[ _finger.id ]->maskExpansion = waterExpansion;
            
            brushes[ _finger.id ]->begin();
        }
        
        //  Update pos
        //
        brushes[ _finger.id ]->set(pos);
//        brushes[ _finger.id ]->setLineWidth(ofMap(radio, 0, 5, lineWidth, 0.0,true));
        brushes[ _finger.id ]->repPct = ofMap(_finger.z, 0, -30, 0, brushRepPct, true);
    } else {
        
        //  if is far away finish it
        //
        brushes[ _finger.id ]->end();
    }
}

void LeapBrush::onFingerDel( LeapFinger &_finger){
    if (nFingerOnPaper == _finger.id){
        nFingerOnPaper = -1;
        paperHolder.set(ofGetWidth(), ofGetHeight());
        page.setHandAt(ofPoint(ofGetWidth(),ofGetHeight()));
    }
    
    if ( brushes[ _finger.id ] != NULL ){
        brushes[ _finger.id ]->end();
        brushes[ _finger.id ]->bKill = true;
    }
}

void LeapBrush::selfReSizeWindow(ofResizeEventArgs &args){
    colorAdded.allocate(args.width,args.height);
    water.allocate(args.width, args.height,GL_RGB);
    normals.allocate(args.width, args.height);
    noise.allocate(args.width,args.height);
    canvas.allocate(args.width, args.height);
    
    page.set(0,0,args.width,args.height);
    page.forceUpdate();
    paperCoorners[0].set(args.width, args.height);
    paperCoorners[1].set(0, args.height);
    
    paperHolder.set(args.width,args.height);
}

