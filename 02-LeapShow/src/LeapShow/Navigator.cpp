//
//  Navigator.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/15/13.
//
//

#include "Navigator.h"

float phi = (1.0+sqrtf(5.0))/2.0;

bool sortByAngle( BillBoard *a, BillBoard *b ){
	return a->x < b->x;
}

bool sortByRadio( BillBoard *a, BillBoard *b ){
	return a->y < b->y;
}

Navigator::Navigator(){
    apps = NULL;
    cam  = NULL;
    
    sFocus = "";
    nSelected = -1;
    clickTimer = 0;
}

void Navigator::linkApps( vector <BillBoard*> *_apps ){
    apps = _apps;
    
    if ( (*apps).size() > 0 ){
        sort( (*apps).begin(), (*apps).end(), sortByAngle);
        sFocus = (*apps)[0]->appName;
    }
}

void Navigator::linkCamera( ofCamera *_cam ){
    cam = _cam;
}

void Navigator::unlinkCamera(){
    cam = NULL;
}

void Navigator::setupUI(){
    
    gui->addLabel("Camera");
    gui->addSlider("Speed", 0.0, 1.0, &camSpeed);
    gui->addSlider("Distance", 100, 1000, &camDistance);
    
    gui->addLabel("Render");
    gui->addSlider("sun_radio", 1.0, 20, &sunRad);
    gui->addSlider("sun_alpha", 0.0, 1.0, &sunAlpha);
    
    gui->addSlider("planet_radio", 1.0, 10, &planetRad);
    gui->addSlider("planet_alpha", 0.0, 1.0, &planetAlpha);
    gui->addSlider("orbit_alpha", 0.0, 1.0, &orbitAlpha);
    
    gui->addSlider("Borders", 0.0, 90, &bordersRad);
    
    sunPos.set(x+width*0.5, y+height*0.5);
}

void Navigator::guiEvent(ofxUIEventArgs &e){
    
}

void Navigator::setFocus( int _index ){
    
    sFocus = (*apps)[_index]->appName;
    ofNotifyEvent(appGetFocus, _index);
    
    ofPoint pos = (*apps)[ _index ]->getPos();
    pos += pos.getNormalized() * camDistance;// (*apps)[_index]->getHeight();
    camTarget.set(pos);
}


bool Navigator::searchForAnyLeft(){
    sort( (*apps).begin(), (*apps).end(), sortByAngle);

    float angle = getFocus()->x;
    
    for(int i = apps->size()-1; i >= 0 ; i--){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
            if ( (*apps)[i]->x < angle ){
                setFocus(i);
                return true;
            }
        }
    }
    
    for(int i = apps->size()-1; i >= 0 ; i--){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
            if ( (*apps)[i]->x < PI ){
                setFocus(i);
                return true;
            }
        }
    }
    
    return false;
}

bool Navigator::searchForSameLevelLeft(){
    sort( (*apps).begin(), (*apps).end(), sortByAngle);
    
    float angle = getFocus()->x;
    float range = PI/2.0;
    float radio = getFocus()->y;
    float threshold = (planetRad*2)/(width*0.5);
    
    for(int i = apps->size()-1; i >= 0 ; i--){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
            if ((*apps)[i]->x < angle &&
                (*apps)[i]->x > angle-range &&
                abs((*apps)[i]->y - radio) < threshold){
                    
                setFocus(i);
                return true;
            }
        }
    }
    
    if (angle-range < -PI){
        float rest = angle-PI;
        for(int i = apps->size()-1; i >= 0 ; i--){
            if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
                if ((*apps)[i]->x < PI &&
                    (*apps)[i]->x > PI-(range-rest) &&
                    abs( (*apps)[i]->y - radio) < threshold){
                    setFocus(i);
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool Navigator::searchForAnyRight(){
    sort( (*apps).begin(), (*apps).end(), sortByAngle);
    
    float angle = getFocus()->x;
    
    for(int i = 0; i < apps->size(); i++){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
            if ( (*apps)[i]->x > angle ){
                setFocus(i);
                return true;
            }
        }
    }
    
    for(int i = 0; i < apps->size(); i++){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
            if ( (*apps)[i]->x > -PI ){
                setFocus(i);
                return true;
            }
        }
    }
    
    return false;
}

bool Navigator::searchForSameLevelRight(){
    sort( (*apps).begin(), (*apps).end(), sortByAngle);
    
    float angle = getFocus()->x;
    float range = PI/2.0;
    float radio = getFocus()->y;
    float threshold = (planetRad*2)/(width*0.5);
    
    for(int i = 0; i < apps->size(); i++){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
            if ((*apps)[i]->x > angle &&
                (*apps)[i]->x < angle+range&&
                abs((*apps)[i]->y - radio) < threshold){
                setFocus(i);
                return true;
            }
        }
    }
    
    if (angle+range > PI){
        float rest = angle-PI;
        
        for(int i = 0; i < apps->size(); i++){
            if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
                if ((*apps)[i]->x > -PI &&
                    (*apps)[i]->x < -PI+(range-rest)&&
                    abs((*apps)[i]->y - radio) < threshold){
                    setFocus(i);
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool Navigator::searchForAnyFront(){
    sort( (*apps).begin(), (*apps).end(), sortByRadio);
    
    float radio = getFocus()->y;
    
    for(int i = apps->size()-1; i >= 0 ; i--){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
            if ( (*apps)[i]->y < radio ){
                setFocus(i);
                return true;
            }
        }
    }
    
    return false;
}

bool Navigator::searchForCloseFront(){
    sort( (*apps).begin(), (*apps).end(), sortByRadio);
    
    float radio = getFocus()->y;
    float minDist = (planetRad)/(width*0.5);
    float dist = 100000.0;
    int index = -1;
    
    for(int i = apps->size()-1; i >= 0 ; i--){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
            
            
            
            if ((*apps)[i]->y < radio &&
                abs((*apps)[i]->y - radio) > minDist ){
                
                float diff = (*apps)[i]->getPos().distance(getFocus()->getPos());
                
                if ( diff < dist){
                    dist = diff;
                    index = i;
                }
                
            }
        }
    }
    
    if (index != -1){
        setFocus(index);
        return true;
    } else {
        return false;
    }
}

bool Navigator::searchForAnyBack(){
    sort( (*apps).begin(), (*apps).end(), sortByRadio);
    
    float radio = getFocus()->y;
    
    for(int i = 0; i < apps->size(); i++){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
            if ( (*apps)[i]->y > radio ){
                setFocus(i);
                return true;
            }
        }
    }
    
    return false;
}

bool Navigator::searchForCloseBack(){
    sort( (*apps).begin(), (*apps).end(), sortByRadio);
    
    float radio = getFocus()->y;
    float minDist = (planetRad)/(width*0.5);
    float dist = 1000000.0;
    int index = -1;
    
    for(int i = 0; i < apps->size(); i++){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName != sFocus){
            
            float diff = abs((*apps)[i]->y - radio);
            
            if ((*apps)[i]->y > radio &&
                diff > minDist ){
                
                float diff = (*apps)[i]->getPos().distance(getFocus()->getPos());
                if ( diff < dist){
                    dist = diff;
                    index = i;
                }
            }
        }
    }
    
    if (index != -1){
        setFocus(index);
        return true;
    } else {
        return false;
    }
}

BillBoard* Navigator::getFocus(){
    BillBoard *rta = NULL;
    
    for(int i = 0; i < apps->size(); i++){
        if ( (*apps)[i]->bEnable && (*apps)[i]->appName == sFocus){
            rta = (*apps)[i];
        }
    }
    
    return rta;
}

void Navigator::update(){
    if ( camPos.distance(camTarget) > 1.0 && cam != NULL){
        camPos.addSlowdonForceTo(camTarget, 1.0, false);
        camPos.update(camSpeed*0.25);
        
        cam->setPosition(ofPoint( camPos.x,0, -camPos.y));
        cam->lookAt( ofPoint(0,0,0) );

    }
    
    //  Update post-click time
    //
    if (clickTimer < 3.0){
        clickTimer += 1.0/60.0f;
    }
}

void Navigator::draw(){
    if (bEnable){
        ofPushMatrix();
        ofPushStyle();
        ofTranslate(sunPos);
        
        {
            
            float radio = MAX(width,height)*0.5;
            ofPolyline orbit;
            orbit.arc(ofPoint(0,0), radio, radio, -bordersRad, bordersRad, true, 60);
            orbit.arc(ofPoint(0,0), -radio, -radio, -bordersRad, bordersRad, true, 60);
            
            ofPushMatrix();
            ofFill();
            ofSetColor(0,20);
            ofBeginShape();
            for(int i = 0; i < orbit.size(); i++){
                ofVertex(orbit[i]);
            }
            ofEndShape();
            
            orbit.clear();
            orbit.arc(ofPoint(0,0), radio-10, radio-10, -bordersRad*0.9, bordersRad*0.9, true, 60);
            
            ofSetColor(255,50);
            ofLine(ofPoint(-sunRad,0), ofPoint(-width*0.5+10,0));
            ofLine(ofPoint(sunRad,0), ofPoint(width*0.5-10,0));
            orbit.draw();
            ofRotate(180);
            orbit.draw();
            
            ofPopMatrix();
        }
        
        //  SUN
        //
        
        ofNoFill();
        ofSetColor(255, sunAlpha*255.0);
        ofCircle(0,0, sunRad);
        
        //  Draw enable apps
        //
        for(int i = 0; i < apps->size(); i++){
            if ( (*apps)[i]->bEnable ){
                float angle = (*apps)[i]->x;
                float radio = (*apps)[i]->y*( MIN(width,height) *0.5);
                
                ofPolyline orbit;
                ofPolyline orbit1;
                ofPolyline orbit2;
                
                int total = (2.0*PI*radio)/(planetRad);
                float jump = ofRadToDeg((2.0*PI)/(float)total);
                
                orbit.arc(ofPoint(0,0), radio, radio, jump, -jump, true, 60);
                orbit1.arc(ofPoint(0,0), radio, radio, -jump*phi, -jump, true, 20);
                orbit2.arc(ofPoint(0,0), radio, radio, jump, jump*phi, true, 20);
                
                ofPushMatrix();
                ofRotate( -ofRadToDeg(angle) );
                
                ofPushStyle();
                ofSetColor(255, orbitAlpha*255.0);
                ofSetLineWidth(0);
                orbit.draw();
                ofSetColor(255,255.0);
                
                if(sFocus == (*apps)[i]->appName){
                    ofSetColor(255);
                }
                
                orbit1.draw();
                orbit2.draw();
                ofPopStyle();
                
                ofTranslate(radio,0);
                ofNoFill();
                ofSetColor(255, planetAlpha*255.0);
                if(sFocus == (*apps)[i]->appName){
                    ofSetColor(255);
                }
                ofCircle(0, 0, planetRad);
                
                if(sFocus == (*apps)[i]->appName){
                    ofFill();
                    ofSetColor(255);
                    ofCircle(0, 0, planetRad*0.5);
                    ofLine(-planetRad*phi, 0, -planetRad, 0);
                    ofLine(planetRad, 0, planetRad*phi, 0);
                }
                
                ofPopMatrix();

                ofPoint appPos = ofPoint(radio*cos( (*apps)[i]->x ), radio*sin( -(*apps)[i]->x ) );
                if (nSelected == i){
                    ofSetColor(255);
                    ofRectangle planet;
                    planet.setFromCenter(appPos, planetRad, planetRad);
                    drawBrakets(planet, 3, 7);
                }
            }
        }
        
        //  Draw observer
        //
//        {
//            ofPoint observer = camPos ;
//            float radio = (observer.length()/3000.0) *(width*0.5);
//            float angle = atan2(observer.y, observer.x);
//            
//            observer.x = sunPos.x + radio*cos(angle);
//            observer.y = sunPos.y + radio*sin(-angle);
//            
//            ofFill();
//            ofSetColor(255, 0, 0);
//            ofCircle(observer, 2);
//        }
        
        ofPopStyle();
        ofPopMatrix();
    }
}

void Navigator::debugDraw(){
    DraggableRectangle::draw();
}

void Navigator::mouseMove(ofPoint _mouse){
    if (bEnable){
        nSelected = -1;
        
        for(int i = 0; i < apps->size(); i++){
            if ( (*apps)[i]->bEnable ){
                float radio = (*apps)[i]->y*( MIN(width,height)*0.5);
                ofPoint appPos = ofPoint(sunPos.x + radio*cos( (*apps)[i]->x ),
                                         sunPos.y + radio*sin( -(*apps)[i]->x ) );
                
                if(appPos.distance( _mouse) < planetRad*2.0 ){
                    nSelected = i;
                }
            }
        }
    }
}

void Navigator::mousePressed(ofPoint _mouse){
    
    if ( clickTimer < 1.0){
        for(int i = 0; i < apps->size(); i++){
            if ( (*apps)[i]->bEnable ){
                float radio = (*apps)[i]->y*( MIN(width,height)*0.5);
                ofPoint appPos = ofPoint(sunPos.x + radio*cos( (*apps)[i]->x ),
                                         sunPos.y + radio*sin( -(*apps)[i]->x ) );
                
                if(appPos.distance( _mouse) < planetRad*2.0 ){
                    setFocus(i);
                }
            }
        }
    }
    
}

void Navigator::mouseDragged(ofPoint _mouse){
    if (bEnable){
        mouseMove( _mouse );
        
        if ( nSelected != -1){
            ofPoint diff = _mouse - sunPos;
            
            (*apps)[nSelected]->x = atan2(-diff.y, diff.x);
            (*apps)[nSelected]->y = diff.length()/(MIN(width,height)*0.5);
        }
    }
}

void Navigator::mouseDraggedDebug(ofPoint _mouse){
    if (bEnable){
        ofPoint A = ofPoint(x,y);
        ofPoint B = ofPoint(x+width,y+height);
        
        if ( A.distance( _mouse) < 20 ){
            x += _mouse.x - x;
            y += _mouse.y - y;
            this->set(x,y, width, height);
            sunPos.set(x+width*0.5, y+height*0.5);
            
        } else if ( B.distance( _mouse) < 20 ){
            width += _mouse.x - x - width;
            height += _mouse.y - y - height;
            this->set(x,y, width, height);
            sunPos.set(x+width*0.5, y+height*0.5);
            
        }
    }
}

void Navigator::mouseReleased(ofPoint _mouse){
    clickTimer = 0;
}