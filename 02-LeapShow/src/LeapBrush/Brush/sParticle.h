#pragma once

#include "ofMain.h"
#include "Particle.h"

struct pPoint{
    ofPoint pos;
    ofFloatColor color;
};

class sParticle : public Particle {
public:
    
    sParticle();

    void    update();
    
    float   maskExpansion;
    float   alphaThreshold;
    float   damping;
    float   *size;
    
    bool    bFixed;
    
    ofFloatColor    color;
    vector<pPoint>  colorTail;
    ofMesh          colorTailMesh;
    ofMesh          maskTailMesh;
};
