//
//  SlitCam.cpp
//
//  Created by Patricio Gonzalez Vivo on 11/3/13.
//
//

#include "SlitCam.h"

#define STRINGIFY(A) #A

void SlitCam::selfSetup(){
    video.initGrabber(640, 480);
    pingPong.allocate(640*2, 480);
    
    string fragShader = STRINGIFY(uniform sampler2DRect tex0;
                                  uniform sampler2DRect backBuffer;
                                  uniform vec2  srcHeader;
                                  uniform float dstHeader;
                                  uniform float grainSize;
                                  uniform float time;
                                  
                                  float rand(vec2 co){
                                      return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
                                  }
                                  
                                  float noise2f( in vec2 p ){
                                      vec2 ip = vec2(floor(p));
                                      vec2 u = fract(p);
                                      u = u*u*(3.0-2.0*u);
                                      
                                      float res = mix(mix(rand(ip),
                                                          rand(ip+vec2(1.0,0.0)),u.x),
                                                      mix(rand(ip+vec2(0.0,1.0)),
                                                          rand(ip+vec2(1.0,1.0)),u.x),
                                                      u.y);
                                      return res*res;
                                  }
                                  
                                  void main(){
                                      vec2 st = gl_TexCoord[0].st;
                                      vec4 color = texture2DRect(backBuffer, st);
                                      
                                      if ( abs(st.x - dstHeader) < srcHeader.y ){
                                          vec2 srcSt = vec2(srcHeader.x,st.y);
                                          float noise = noise2f( vec2(st.x*abs(sin(time*0.061))*3.1,st.y) * grainSize );
                                          srcSt.x += srcHeader.y * noise*2.0;
                                          vec4 srcColor = texture2DRect(tex0, srcSt);
                                          color = mix(color,srcColor, noise );
                                      }
                                      
                                      gl_FragColor = color;
                                  });
    
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
    shader.linkProgram();
    
    dstHeader = 0;
    grainSize = 0.005;
}

void SlitCam::selfSetupGui(){
}

void SlitCam::selfSetupSystemGui(){
    sysGui->add2DPad("srcHeader", ofPoint(0,1), ofPoint(0,1), &srcHeader);
    sysGui->addSlider("GrainSize", 0.0, 1.0, &grainSize);
}

void SlitCam::guiSystemEvent(ofxUIEventArgs &e){
    
}

void SlitCam::selfBegin(){
    
}

void SlitCam::selfUpdate(){
    video.update();
    
    dstHeader += 0.5;
    while ( dstHeader > pingPong.getWidth() ){
        dstHeader -= pingPong.getWidth();
    }
    
    pingPong.swap();
    pingPong.src->begin();
    shader.begin();
    shader.setUniform1f("time", ofGetElapsedTimef());
    shader.setUniform1f("grainSize", grainSize);
    shader.setUniform1f("dstHeader", dstHeader);
    shader.setUniform2f("srcHeader", srcHeader.x*video.getWidth(),srcHeader.y*video.getWidth()*0.5);
    shader.setUniformTexture("tex0", video.getTextureReference(), 1);
    shader.setUniformTexture("backBuffer", pingPong.getTextureReference(), 0);
    pingPong.getTextureReference().draw(0, 0);
    shader.end();
    pingPong.src->end();
}

void SlitCam::selfDraw(){
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5-pingPong.getWidth()*0.5,
                ofGetHeight()*0.5-pingPong.getHeight()*0.5);
    
    ofSetColor(255);
    pingPong.getTextureReference().draw(0,0);
    
//    ofSetColor(255, 0, 0);
//    ofLine(srcHeader.x*pingPong.getWidth(), 0,
//           srcHeader.x*pingPong.getWidth(), pingPong.getHeight());
//    
//    ofSetColor(255,50);
//    ofRect(srcHeader.x*pingPong.getWidth()-srcHeader.y*pingPong.getWidth()*0.5, 0,
//           srcHeader.y*pingPong.getWidth()*0.5*2.0, pingPong.getHeight());
    
    ofSetColor(255,50);
    ofRect(dstHeader-srcHeader.y*pingPong.getWidth()*0.25, 0,
           srcHeader.y*pingPong.getWidth()*0.25*2.0, pingPong.getHeight());
    
    ofPopMatrix();
}

void SlitCam::selfEnd(){
    
}

void SlitCam::selfExit(){
    
}

void SlitCam::selfKeyPressed(ofKeyEventArgs & args){
    
}

void SlitCam::selfKeyReleased(ofKeyEventArgs & args){
    
}

void SlitCam::selfMouseDragged(ofMouseEventArgs& data){
    
}

void SlitCam::selfMouseMoved(ofMouseEventArgs& data){
    
}

void SlitCam::selfMousePressed(ofMouseEventArgs& data){
    
}

void SlitCam::selfMouseReleased(ofMouseEventArgs& data){
    
}