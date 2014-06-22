//
//  Navigator.h
//
//  Created by Patricio Gonzalez Vivo on 9/15/13.
//
//

#pragma once

#include "Particle.h"
#include "UIClass.h"
#include "DraggableRectangle.h"

#include "BillBoard.h"

#include "UIBackground.h"

class Navigator : public UIClass, public DraggableRectangle {
public:
    Navigator();
    
    void    setupUI();
    void    linkApps( vector <BillBoard*> *_apps );
    void    linkCamera( ofCamera *_cam );
    void    unlinkCamera();
    
    void        setFocus( int _index );
    
    BillBoard*   getFocus();
    BillBoard*   getApp(int _index);
    string      getClassName(){ return name; };
    
    bool    searchForAnyLeft();
    bool    searchForAnyRight();
    bool    searchForAnyFront();
    bool    searchForAnyBack();
    
    bool    searchForSameLevelLeft();
    bool    searchForSameLevelRight();
    bool    searchForCloseFront();
    bool    searchForCloseBack();
    
    void    update();
    void    draw();
    void    debugDraw();
    
    void    mouseMove(ofPoint _mouse);
    void    mousePressed(ofPoint _mouse);
    void    mouseDragged(ofPoint _mouse);
    void    mouseDraggedDebug(ofPoint _mouse);
    void    mouseReleased(ofPoint _mouse);
    
    ofEvent<int>    appGetFocus;
    
protected:
    void    guiEvent(ofxUIEventArgs &e);
    
    bool    sortByDistance( BillBoard *a, BillBoard *b);
    
    vector <BillBoard*>  *apps;
    ofCamera            *cam;
    
    //  Diagram
    //
    ofPoint sunPos;
    float   sunRad, sunAlpha;
    float   planetRad, planetAlpha, orbitAlpha;
    float   bordersRad;
    
    //  Interaction (click and drag)
    //
    int     nSelected;
    string  sFocus;
    float   clickTimer;

    //  Camera
    //
    Particle    camPos;
    ofPoint     camTarget;
    float       camSpeed, camDistance;
};
