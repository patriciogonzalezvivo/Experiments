//
//  Director.cpp
//  TraceMaker
//
//  Created by Patricio Gonzalez Vivo on 11/1/13.
//
//

#include "Director.h"

Director::Director(){
    font = NULL;
    clear();
}

void Director::setFont(ofxUIFont *_font){
    font = _font;
}

void Director::loadCustomValues(ofxXmlSettings &_XML){
    if (_XML.pushTag("line")){
        int totalSavedPoints = _XML.getNumTags("point");
        points.clear();
        
        for(int i = 0; i < totalSavedPoints; i++){
            _XML.pushTag("point",i);
            points.push_back(ofPoint(_XML.getValue("x", 0.0),
                                     _XML.getValue("y", 0.0)) );
            _XML.popTag();
        }
    }
}

void Director::saveCustomValues(ofxXmlSettings &_XML){
    
    if(!_XML.tagExists("line")){
        _XML.addTag("line");
    }
    
    {
        _XML.pushTag("line");
        int totalSavedPoints = _XML.getNumTags("point");
        
        for(int i = 0; i < points.size(); i++){
            int tagNum = i;
            
            if (i >= totalSavedPoints)
                tagNum = _XML.addTag("point");
            
            _XML.setValue("point:x",points[i].x, tagNum);
            _XML.setValue("point:y",points[i].y, tagNum);
        }
        
        int totalCorners = points.size();
        totalSavedPoints = _XML.getNumTags("point");
        
        if ( totalCorners < totalSavedPoints){
            for(int i = totalSavedPoints; i > totalCorners; i--){
                _XML.removeTag("point",i-1);
            }
        }
        _XML.popTag();
    }
}

void Director::clear(){
    points.clear();
    points.push_back(ofPoint(0,0));
    points.push_back(ofPoint(0.5,0));
    points.push_back(ofPoint(1.0,0));
}

ofPoint Director::getDirForStep(int _index, float _scale){
    _index = _index%(points.size()-1);
    
    float ratio = width/height;
    
    ofPoint dir = (points[_index+1]-points[_index])*_scale;
    dir.x *= ratio;
    
    return dir*2.0;
}

void Director::draw(){
    
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(x, y);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
    
    ofFill();
    ofSetColor(brigtness*255,OFX_UI_COLOR_BACK_ALPHA*4.0);
    ofRect(0,0,width,height);
    
    ofSetColor(OFX_UI_COLOR_FILL ,OFX_UI_COLOR_FILL_ALPHA);
    if(font == NULL){
        ofDrawBitmapString(name,15,15);
    } else {
        font->drawString(name,15,20);
    }
    
    ofNoFill();
    ofSetColor(brigtness*255,OFX_UI_COLOR_BACK_ALPHA);
    drawBrakets(ofRectangle(-7,-7,14,14), 5, 2 );
    drawBrakets(ofRectangle(width-7,height-7,14,14), 5, 2 );
    ofPopMatrix();
    
    ofSetColor(OFX_UI_COLOR_FILL,OFX_UI_COLOR_FILL_ALPHA);
    ofBeginShape();
    for(int i = 0; i< points.size();i++){
        ofPoint pos = toScreen(points[i]);
        ofVertex(pos);
    }
    ofEndShape(false);
    
    ofSetColor(OFX_UI_COLOR_FILL_HIGHLIGHT,OFX_UI_COLOR_FILL_HIGHLIGHT_ALPHA);
    for(int i = 1; i< points.size()-1;i++){
        ofCircle(toScreen(points[i]),5);
    }
    
    ofPopStyle();
}

ofPoint Director::fromScreen(ofPoint _pos){
    ofPoint pos;
    pos.x = (_pos.x-x)/width;
    pos.y = (_pos.y-y)/height;
    pos.y -= 0.5;
    return pos;
}

ofPoint Director::toScreen(ofPoint _pos){
    return ofPoint(_pos.x*width+x,_pos.y*height+y+height*0.5);
}

bool Director::mousePressed(ofPoint _mouse){
    if (inside(_mouse)){
        
        //  Is over one dot?
        //
        bool overDot = false;
        for(int i=1;i<points.size()-1;i++){
            ofPoint pos = toScreen(points[i]);
            if(pos.distance(_mouse)<20){
                overDot = true;
                break;
            }
        }
        
        if (!overDot){
            lastCLick = _mouse;
        }
        
        if(!ofGetKeyPressed() && !overDot){
            
            float lenghtToClosest = 1000;
            int closest = -1;
            
            for (int i=0;i<points.size()-1;i++){
                int next=i+1;
                
                ofVec2f AtoM = _mouse - toScreen(points[i]);
                ofVec2f AtoB = toScreen(points[next]) - toScreen(points[i]);
                
                float a = atan2f(AtoM.x, AtoM.y);
                float b = atan2f(AtoB.x, AtoB.y);
                
                if (abs(a - b) <= 0.05 &&               //  1. Is over the line
                    AtoM.length() < lenghtToClosest &&  //  2. Is the closest one
                    AtoM.length() < AtoB.length() ){    //  3. Is between those two points
                    lenghtToClosest = AtoM.length();
                    closest = next;
                }
            }
            
            if(closest!=-1){
                points.insert(points.begin()+closest, fromScreen(_mouse));
            }
            
        } else if (ofGetKeyPressed('a') && !overDot){
            points.insert(points.end()-1, fromScreen(_mouse));
        } else if( ( ofGetKeyPressed('d') || ofGetKeyPressed(OF_KEY_BACKSPACE) || ofGetKeyPressed(OF_KEY_DEL) )&& overDot){
            for(int i=1;i<points.size()-1;i++){
                ofPoint pos = toScreen(points[i]);
                if(pos.distance(_mouse)<10){
                    points.erase(points.begin()+i);
                    break;
                }
            }
        }
    }
}

bool Director::mouseDragged(ofPoint _mouse){
    
    bool corners = false;
    ofPoint A = ofPoint(x,y);
    ofPoint B = ofPoint(x+width,y+height);
    
    if ( A.distance( _mouse) < 20 ){
        
        ofPoint disp = ofPoint(_mouse.x - x,
                               _mouse.y - y);
        
        x += disp.x;
        y += disp.y;
        width -= disp.x;
        height -= disp.y;
        this->set(x,y, width, height);
        corners = true;
    } else if ( B.distance( _mouse) < 20 ){
        width += _mouse.x - x - width;
        height += _mouse.y - y - height;
        this->set(x,y, width, height);
        corners = true;
    }
    
    if(!corners){
        if(inside(_mouse)){
            //  Is over one dot?
            //
            bool overDot = false;
            for(int i=1;i<points.size()-1;i++){
                ofPoint pos = toScreen(points[i]);
                if(pos.distance(_mouse)<20){
                    overDot = true;
                    break;
                }
            }
            
            if(!ofGetKeyPressed()&&overDot){
                for(int i=0;i<points.size();i++){
                    ofPoint pos = toScreen(points[i]);
                    if(pos.distance(_mouse)<10){
                        points[i] = fromScreen(_mouse);
                        return true;
                    }
                }
            } else if ( ofGetKeyPressed('a') && !overDot){
                points.insert(points.end()-1, fromScreen(_mouse));
            } else if ( ofGetKeyPressed('d') || ofGetKeyPressed(OF_KEY_BACKSPACE) || ofGetKeyPressed(OF_KEY_DEL) ){
                for(int i=1;i<points.size()-1;i++){
                    ofPoint pos = toScreen(points[i]);
                    if(pos.distance(_mouse)<10){
                        points.erase(points.begin()+i);
                        return true;
                    }
                }
            } else if(!overDot){
                ofPoint diff = _mouse - lastCLick;
                x += diff.x;
                y += diff.y;
                
                lastCLick = _mouse;
            }
        } else {
            return false;
        }
    } else {
        return true;
    }
}