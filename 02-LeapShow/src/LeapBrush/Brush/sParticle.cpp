#include "sParticle.h"

//------------------------------------------------------------
sParticle::sParticle(){
	this->set(0.0,0.0,0.0);
    vel.set(ofRandom(-10,10), ofRandom(-10,10));
    acc.set(0,0,0);
    
    size = NULL;
	damping = 0.07f;
    
    colorTailMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
}

//------------------------------------------------------------
void sParticle::update(){
	
    if (bFixed == false){
        vel += acc;
        vel *= 1.0f - damping;
        *this += vel;
        
        pPoint newPoint;
        newPoint.pos = *this;
        newPoint.color = color;
        newPoint.color.a = ofMap( vel.length(), 0.0, alphaThreshold , 1.0 ,0.0, true );
        
        colorTail.push_back(newPoint);
        
        if (colorTail.size() > 10){
            colorTail.erase(colorTail.begin());
        }
        
        maskTailMesh.clear();
        colorTailMesh.clear();
        for(int i = 0; i < colorTail.size()-1; i++){
            
            ofFloatColor aColor = colorTail[i].color;
            ofFloatColor bColor = colorTail[i+1].color;
            
            ofVec2f E = (colorTail[i+1].pos-colorTail[i].pos).getNormalized() * (aColor.a*(*size));
            ofVec2f N = ofVec2f(-E.y, E.x);
            ofVec2f S = -N;
            ofVec2f NE = N + E;
            ofVec2f NW = N - E;
            ofVec2f SW = -NE;
            ofVec2f SE = -NW;
        
//            aColor.a = 1.0;
//            bColor.a = 1.0;
            
            colorTailMesh.addColor(aColor);
            colorTailMesh.addTexCoord(ofVec2f(0, 1));
            colorTailMesh.addVertex(colorTail[i].pos + SW);
            maskTailMesh.addTexCoord(ofVec2f(0, 1));
            maskTailMesh.addVertex(colorTail[i].pos + SW*maskExpansion);
            
            colorTailMesh.addColor(aColor);
            colorTailMesh.addTexCoord(ofVec2f(0, 0));
            colorTailMesh.addVertex(colorTail[i].pos + NW);
            maskTailMesh.addTexCoord(ofVec2f(0, 0));
            maskTailMesh.addVertex(colorTail[i].pos + NW*maskExpansion);
            
            colorTailMesh.addColor(aColor);
            colorTailMesh.addTexCoord(ofVec2f(0.5, 1));
            colorTailMesh.addVertex(colorTail[i].pos + S);
            maskTailMesh.addTexCoord(ofVec2f(0.5, 1));
            maskTailMesh.addVertex(colorTail[i].pos + S*maskExpansion);
            
            colorTailMesh.addColor(aColor);
            colorTailMesh.addTexCoord(ofVec2f(0.5, 0));
            colorTailMesh.addVertex(colorTail[i].pos + N);
            maskTailMesh.addTexCoord(ofVec2f(0.5, 0));
            maskTailMesh.addVertex(colorTail[i].pos + N*maskExpansion);
            
            colorTailMesh.addColor(aColor);
            colorTailMesh.addTexCoord(ofVec2f(0.5, 1));
            colorTailMesh.addVertex(colorTail[i].pos + S);
            maskTailMesh.addTexCoord(ofVec2f(0.5, 1));
            maskTailMesh.addVertex(colorTail[i].pos + S*maskExpansion);
            
            colorTailMesh.addColor(bColor);
            colorTailMesh.addTexCoord(ofVec2f(0.5, 0));
            colorTailMesh.addVertex(colorTail[i].pos + N);
            maskTailMesh.addTexCoord(ofVec2f(0.5, 0));
            maskTailMesh.addVertex(colorTail[i].pos + N*maskExpansion);
            
            colorTailMesh.addColor(bColor);
            colorTailMesh.addTexCoord(ofVec2f(1, 1));
            colorTailMesh.addVertex(colorTail[i].pos + SE);
            maskTailMesh.addTexCoord(ofVec2f(1, 1));
            maskTailMesh.addVertex(colorTail[i].pos + SE*maskExpansion);
            
            colorTailMesh.addColor(bColor);
            colorTailMesh.addTexCoord(ofVec2f(1, 0));
            colorTailMesh.addVertex(colorTail[i].pos + NE);
            maskTailMesh.addTexCoord(ofVec2f(1, 0));
            maskTailMesh.addVertex(colorTail[i].pos + NE*maskExpansion);
        }
    }
    
    acc *= 0;
}