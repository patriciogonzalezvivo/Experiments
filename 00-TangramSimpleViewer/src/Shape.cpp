//
//  Shape.cpp
//  glyphsOnSpline
//
//  Created by Patricio Gonzalez Vivo on 11/1/12.
//
//

#include "Shape.h"

Shape::Shape(){
    size = 20;
    damping = 0.05f;
    
    actual.rot.set(0.0,0.0,0.0);
    target.rot.set(0.0,0.0,0.0);
    
    bDebug = NULL;
    bTrail = NULL;
}

void Shape::update(float _posLerp, float _rotLerp){
    
    actual.rot.x = ofLerp(actual.rot.x, target.rot.x, _rotLerp);
    actual.rot.y = ofLerp(actual.rot.y, target.rot.y, _rotLerp);
    actual.rot.z = ofLerp(actual.rot.z, target.rot.z, _rotLerp);
    
    if ( getDistanceToTarget() > 10) {
        addAttractionForce(target.pos, 1000, 0.3);
    } else {
        actual.pos.x = ofLerp(actual.pos.x,target.pos.x,_posLerp);
        actual.pos.y = ofLerp(actual.pos.y,target.pos.y,_posLerp);
        actual.pos.z = ofLerp(actual.pos.z,target.pos.z,_posLerp);
    }
    
    vel += acc;
    vel *= 1.0f - damping;
    actual.pos += vel;
    acc *= 0;
    
    if (bTrail != NULL){
        if (*bTrail){
            trail.push_back(actual);
            while(trail.size() > 10){
                trail.erase(trail.begin());
            }
        }
    }
}

void Shape::draw(){
    //  Make the shape
    //
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    for (int i = 0; i < getVertices().size(); i++) {
        mesh.addVertex(getVertices()[i]);
    }
    
    //  Draw Trail
    //
    if (bTrail != NULL){
        if (*bTrail){
            for (int j = 0; j < trail.size(); j++) {
                ofPushMatrix();
                ofTranslate(trail[j].pos);
                ofRotateY(trail[j].rot.y);
                ofRotateX(trail[j].rot.x);
                ofRotateZ(trail[j].rot.z);
                
                ofFill();
                ofColor trailColor = color;
                float pct = trailColor.getSaturation()*((float)j/(float)trail.size());
                //            trailColor.setSaturation( pct );
                trailColor.a = pct;
                ofSetColor(trailColor);
                mesh.draw();
                ofPopMatrix();
            }
        }
    }
    
    
    //  Draw Shape
    //
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(actual.pos);
    ofRotateY(actual.rot.y);
    ofRotateX(actual.rot.x);
    ofRotateZ(actual.rot.z);
    
    ofFill();
    ofSetColor(color);
    mesh.draw();
    ofPopMatrix();

    
    if(bDebug){
        ofPushMatrix();
        ofTranslate(target.pos);
        ofSetColor(255);
        ofLine(-5,0,5,0);
        ofLine(0, -5, 0, 5);
        ofLine(0, 0, -5, 0, 0, 5);
        
        ofRotateY(target.rot.y);
        ofRotateX(target.rot.x);
        ofRotateZ(target.rot.z);
        
        ofEnableSmoothing();
//        mesh.drawWireframe();
        ofNoFill();
        ofBeginShape();
        for (int i = 0; i < getVertices().size(); i++) {
            ofVertex(getVertices()[i]);
        }
        ofEndShape(true);
        
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(actual.pos);
        
        ofFill();
        ofSetColor(255,100);
        ofSphere(0,0,0, 7);
        
        ofDrawBitmapString("x: "+ofToString(target.rot.x), 15,-15);
        ofDrawBitmapString("y: "+ofToString(target.rot.y), 15,0);
        ofDrawBitmapString("z: "+ofToString(target.rot.z), 15,15);
        
        ofPopMatrix();
        ofDisableSmoothing();
    }
    
    ofPopStyle();
}

//------------------------------------------------- SETUP
void Shape::makeNumber( int _num, float _scale ){
    clear();
    
    actual.pos.set(0,0);
    target.pos.set(0,0);
    
    //  Choose of Shapes
    //
    switch (_num) {
        case 0:
            addVertex(ofVec3f(0, 0));
            addVertex(ofVec3f(2, 2));
            addVertex(ofVec3f(0, 4));
            break;
        case 1:
            addVertex(ofVec3f(0, 0));
            addVertex(ofVec3f(4, 0));
            addVertex(ofVec3f(2, 2));
            break;
        case 2:
            addVertex(ofVec3f(4, 0));
            addVertex(ofVec3f(4, 2));
            addVertex(ofVec3f(3, 1));
            break;
        case 3:
            addVertex(ofVec3f(2, 2));
            addVertex(ofVec3f(3, 1));
            addVertex(ofVec3f(4, 2));
            addVertex(ofVec3f(4, 2));
            addVertex(ofVec3f(3, 3));
            addVertex(ofVec3f(2, 2));
            break;
        case 4:
            addVertex(ofVec3f(1, 3));
            addVertex(ofVec3f(2, 2));
            addVertex(ofVec3f(3, 3));
            break;
        case 5:
            addVertex(ofVec3f(2, 4));
            addVertex(ofVec3f(0, 4));
            addVertex(ofVec3f(1, 3));
            addVertex(ofVec3f(1, 3));
            addVertex(ofVec3f(3, 3));
            addVertex(ofVec3f(2, 4));
            break;
        case 6:
            addVertex(ofVec3f(4, 2));
            addVertex(ofVec3f(4, 4));
            addVertex(ofVec3f(2, 4));
            break;
    }
    close();
    
    //  Centered
    //
    for (int i = 0; i < getVertices().size(); i++){
        getVertices()[i] -= ofPoint(2,2);
        getVertices()[i] *= _scale;
    }
    target.pos = actual.pos = this->getCentroid2D();
    for (int i = 0; i < getVertices().size(); i++){
        getVertices()[i] -= target.pos;
    }
    ofRectangle rect = getBoundingBox();
    size = MAX(rect.width,rect.height);
    
    //  Colors Palette
    //
    vector<ofColor> colors;
    colors.push_back(ofColor(245, 180, 69));
    colors.push_back(ofColor(221, 35, 66));
    colors.push_back(ofColor(93, 191, 185));
    colors.push_back(ofColor(206, 116, 172));
    colors.push_back(ofColor(86, 172, 220));
    colors.push_back(ofColor(0, 55, 115));
    color = colors[ _num%colors.size() ];
}

void Shape::makeFromLimb(int _limbNum, Glyph &_glyph ){
    
    Limb limb = _glyph.limbs()[_limbNum];
    for (int i = 0; i < limb.coords().size(); i++){
        ofPoint coord = limb.coords()[i];
        coord *= _glyph.scale();
        this->addVertex(coord);
    }
    close();
    
    target.pos = actual.pos = this->getCentroid2D();
    for (int i = 0; i < getVertices().size(); i++){
        getVertices()[i] -= target.pos;
    }
    
    ofRectangle rect = limb.getBoundingBox();
    size = MAX(this->getBoundingBox().width,this->getBoundingBox().height);
    
    color = _glyph.limbs()[_limbNum].color();
}

void Shape::setTargetPos( ofPoint _pos ){
    target.pos = _pos;
};

void Shape::setTargetRot( ofPoint _rot ){
    target.rot.x = _rot.x;
    target.rot.y = _rot.y;
    target.rot.z = _rot.z;
};

//----------------------------------------------------- Questions
bool Shape::mouseHover(){
    return inside(ofGetMouseX()-ofGetWidth()*0.5-actual.pos.x,
                  ofGetMouseY()-ofGetHeight()*0.5-actual.pos.y);
}

ofPoint Shape::getTargetRot(){
    return ofPoint(target.rot.x,target.rot.y,target.rot.z);
};

ofPoint Shape::getTargetPos(){
    return target.pos;
}

float Shape::getDistanceToTarget(){
    return target.pos.distance(actual.pos);
}

//----------------------------------------------------- ACTIONS
void Shape::flipH(){
    target.rot.x += 180;
    while (target.rot.x >= 360) {
        target.rot.x -= 360;
    }
}

void Shape::flipV(){
    target.rot.y += 180;
    while (target.rot.y >= 360) {
        target.rot.y -= 360;
    }
}

void Shape::turnLeft(){
    if ( target.rot.z < 5){
        target.rot.z = 360;
    }
    
    target.rot.z -= 5;
}

void Shape::turnRight(){
    target.rot.z += 5;
    
    while (target.rot.z >= 360) {
        target.rot.z -= 360;
    }
}


void Shape::addForce(ofPoint &_force){
    acc += _force;
}
//------------------------------------------------------------ PHYSICS
void Shape::addRepulsionForce(ofPoint posOfForce, float radius, float scale){
    
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= actual.pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
        diff.normalize();
        ofPoint dir = ofPoint(diff * scale * pct);
        addForce(dir);
    }
}

void Shape::addAttractionForce(ofPoint posOfForce, float radius, float scale){
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= actual.pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
        ofPoint dir = ofPoint(diff * scale * pct) * -1;
        addForce(dir);
    }
}

void Shape::addRepulsionForce(Shape &s, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(s.actual.pos);
	
	// ----------- (2) calculate the difference & length
	
	ofVec2f diff	= actual.pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
        ofPoint dir = ofPoint(diff * scale * pct);
        addForce(dir);
		dir *= -1.0;
        s.addForce(dir);
    }
}

void Shape::addRepulsionForce(Shape &s, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(s.actual.pos);
	
	// ----------- (2) calculate the difference & length
	
	ofVec2f diff	= actual.pos - posOfForce;
	float length	= diff.length();
    float radius    = size*2.f + s.size*2.f;
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
        ofPoint dir = ofPoint(diff * scale * pct);
        addForce(dir);
		dir *= -1.0;
        s.addForce(dir);
    }
}

void Shape::addAttractionForce(Shape &s, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(s.actual.pos);
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= actual.pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
        ofPoint dir = ofPoint(diff * scale * pct);
        s.addForce(dir);
		dir *= -1.0;
        addForce(dir);
    }
	
}

void Shape::addClockwiseForce(Shape &s, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(s.actual.pos);
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= actual.pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
        
		acc.x -= diff.y * scale * pct;
        acc.y += diff.x * scale * pct;
        
        ofPoint dir = ofPoint(diff * scale * pct);
        s.addForce(dir);
		s.acc.x += diff.y * scale * pct;
        s.acc.y -= diff.x * scale * pct;
    }
}

void Shape::addCounterClockwiseForce(Shape &s, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(s.actual.pos);
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= actual.pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		acc.x += diff.y * scale * pct;
        acc.y -= diff.x * scale * pct;
		s.acc.x -= diff.y * scale * pct;
        s.acc.y += diff.x * scale * pct;
		
    }
}

