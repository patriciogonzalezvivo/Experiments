//
//  Walker.h
//  09 - TraceMaker
//
//  Created by Patricio Gonzalez Vivo on 10/9/13.
//
//

#pragma once
#include "ofMain.h"

class Walker : public ofPoint {
public:
    
    void checkOver(bool _isOverNow ){
        if ( !bOverBlob && _isOverNow ){
            bOverBlob = true;
            currentLine.addVertex(*this);
        } else if (bOverBlob && _isOverNow ){
            currentLine.addVertex(*this);
        } else if (bOverBlob && !_isOverNow ){
            currentLine.addVertex(*this);
            lines.push_back(currentLine);
            currentLine.clear();
            bOverBlob = false;
        }
    }
    
    void draw(){
        ofSetColor(255,0,0);
        ofCircle(*this, 2);
        for(int i=0;i<lines.size();i++){
            lines[i].draw();
        }
    }
    
    vector<ofPolyline>  lines;
    ofPolyline currentLine;
    
    bool    bOverBlob;
};
