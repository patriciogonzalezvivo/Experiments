//
//  lineWalker.h
//  TraceMaker
//
//  Created by natxo pedreira on 04/11/13.
//
//

#pragma once
#include "Particle.h"

class lineWalker : public Particle {
public:
    
    lineWalker(){
        color.set(1,0,0);
        size = 2;
    }
    
    void checkOver( bool _isOverNow ){
        if ( !bOverBlob && _isOverNow ){
            bOverBlob = true;
            start.set(*this);
        } else if (bOverBlob && !_isOverNow ){
            ofPolyline line;
            line.addVertex(start);
            line.addVertex(*this);
            lines.push_back(line);
            bOverBlob = false;
        }
    }
    
    void draw(){
        ofSetColor(color);
        ofCircle(*this, size);
        for(int i=0;i<lines.size();i++){
            lines[i].draw();
        }
    }
    
    vector<ofPolyline> lines;
    ofPoint start;
    bool bOverBlob;
    
};
