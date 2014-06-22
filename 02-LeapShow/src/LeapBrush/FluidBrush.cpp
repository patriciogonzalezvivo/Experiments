//
//  FluidBrush.cpp
//
//  Created by Patricio Gonzalez Vivo on 10/4/13.
//
//

#include "FluidBrush.h"


void FluidBrush::selfSetup(){
    ofSetCircleResolution(36);
    ofEnableAlphaBlending();
    ofDisableSmoothing();

    ofSetFullscreen(true);
    
    ofDisableArbTex();
    ofLoadImage(brushTexture, getDataPath()+"brush.png");
    ofEnableArbTex();
    
    int width = ofGetScreenWidth();
    int height = ofGetScreenHeight();
    colorAdded.allocate(width,height);
    water.allocate(width, height,GL_RGB);
    flow.allocate(width, height);
    fluid.allocate(width, height,0.5,false);
    fluid.setGravity(ofVec2f(0.0,0.0));
    fluid.setUseObstacles(false);
    
    color.set(255);
}

void FluidBrush::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
}

void FluidBrush::selfSetupSystemGui(){
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
    
    sysGui->addLabel("FLUID IN VALUES");
    sysGui->addSlider("Vel_Pct", 0.0, 1.0, &fluidVelPct);
    sysGui->addSlider("Color_Pct", 0.0, 1.0, &fluidColorPct);
    
    sysGui->addLabel("FLUID SIMULATION");
    sysGui->addSlider("Fluid_dissipation", 0.95, 1.0, &fluid.dissipation);
    sysGui->addSlider("Fluid_vel_dissipation", 0.5, 1.0, &fluid.velocityDissipation);
    sysGui->addSlider("Fluid_temp_dissipation", 0.5, 1.0, &fluid.temperatureDissipation);
    sysGui->addSlider("Fluid_press_dissipation", 0.5, 1.0, &fluid.pressureDissipation);
    
}

void FluidBrush::selfBegin(){
    colorAdded.begin();
    ofClear(0,0);
    colorAdded.end();

    bClean = true;
}

void FluidBrush::selfUpdate(){
    ofPushStyle();
    ofSetColor(255);
    
    if (bClean){
        colorAdded.begin();
        ofClear(0,0);
        colorAdded.end();
        brush.clear();
        fluid.clear(0);
        bClean = false;
    }
    
    //  UPDATE
    //
    if(brush.bDown){
        brush.update();
        
        //  Add Ink
        //
        if(brush.bInk){
            colorAdded.begin();
            ofClear(0,0);
            brushTexture.bind();
            brush.draw();
            brushTexture.unbind();
            
            colorAdded.end();
            fluid.addColor(colorAdded,fluidColorPct);
        }
        
        water.swap();
        water.dst->begin();
        ofClear(0,0);
        ofSetColor(waterAmount);
        brush.drawMask();
        water.dst->end();
        
        flow << *(water.dst);
    } else {
        water.swap();
        water.dst->begin();
        ofClear(0,0);
        water.dst->end();
        
        flow << *(water.dst);
    }
    
    fluid.addVelocity(flow,fluidVelPct);
    
    fluid.update();
    ofEnableAlphaBlending();
    
    ofPopStyle();
}

void FluidBrush::selfDraw(){
    ofEnableAlphaBlending();
    fluid.draw(0,0);
    
    ofPushMatrix();
    ofTranslate(0, 0,1);
    
    if( bDebug ){
        ofSetColor(255);
        ofFill();
        brush.drawDebug();
        ofPushMatrix();
        ofTranslate(ofGetWidth(), 0);
        ofScale(0.25, 0.25);
        colorAdded.draw(-colorAdded.getWidth(), 0);
        water.dst->draw(-water.getWidth(),colorAdded.getHeight());
        flow.draw(-flow.getWidth(),colorAdded.getHeight()*2.0);
        fluid.drawVelocity(-fluid.getWidth(),colorAdded.getHeight()*3.0);
        ofPopMatrix();
    }
    ofPopMatrix();
}

void FluidBrush::selfKeyPressed(ofKeyEventArgs & args){
    if (args.key == ' '){
        bClean = true;
    }
}

void FluidBrush::selfKeyReleased(ofKeyEventArgs & args){
}

void FluidBrush::selfMouseMoved(ofMouseEventArgs& data){
}

void FluidBrush::selfMousePressed(ofMouseEventArgs& data){
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

void FluidBrush::selfMouseDragged(ofMouseEventArgs& data){
    brush.set(ofPoint(data.x,data.y));
}

void FluidBrush::selfMouseReleased(ofMouseEventArgs& data){
    brush.end();
    brush.clear();
    colorAdded.begin();
    ofClear(0,0);
    colorAdded.end();
}

void FluidBrush::selfReSizeWindow(ofResizeEventArgs &args){
    colorAdded.allocate(args.width,args.height);
    water.allocate(args.width, args.height,GL_RGB);
    flow.allocate(args.width, args.height);
    fluid.allocate(args.width, args.height,0.5,false);
}

