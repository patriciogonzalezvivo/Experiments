//
//  Brush.h
//
//  Created by Patricio Gonzalez Vivo on 12/17/12.
//
//

#pragma once

#include "ofMain.h"

#include "sParticle.h"
#include "Spring.h"

class Brush : public ofPoint {
public:
    Brush();
    ~Brush();

    void    begin();
    void    end();
    
    void    init(int _numSprings);
    
    void    setBrushWidth(float _width);
    void    setLineWidth(float _width);
    
    void    setColor( ofColor _color, float _randAmount );
    void    pickColorFrom( ofTexture &Text, float _lerpAmount, float _randAmount);
    
    void    set(int _x, int _y);
    void    set(ofPoint _pos);
    void    set(int _x, int _y, int _w, int _h);
    
    ofPoint getVel();
    float   getAngle();

    void    clear();
    
    void    update();
    
    void    draw();
    void    drawMask();
    void    drawDebug();

    float   lineWidth;
    float   maskExpansion;
    float   alphaThreshold;
    float   damp;
    float   k;
    float   kRandomness;
    float   repRad;
    float   repPct;
    
    int     nId;
    bool    bDown;
    bool    bDebug;
    
    bool    bKill;
    bool    bInk;
    
    
private:
    vector<sParticle*>   As;
    vector<sParticle*>   Bs;
    vector<Spring>      springs;
    vector<ofFloatColor> colors;
    
    ofPolyline          tail;
    
    ofPoint             prevPos;
    
    float               brushWidth;
    
};

typedef std::shared_ptr<Brush> BrushRef;

