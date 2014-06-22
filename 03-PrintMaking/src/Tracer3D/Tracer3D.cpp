//
//  Tracer3D.cpp
//  PrintMaking
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//

#include "Tracer3D.h"

using namespace ofxCv;
using namespace cv;

bool comparisonFunction( Particle * a, Particle * b ) {
	return a->x < b->x;
}

void Tracer3D::selfSetup(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    shader.loadFrag(getDataPath()+"shaders/shader.frag");
    fbo.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    particlesArea.loadSettings(getDataPath()+"area.xml");
    pixels.allocate(ofGetScreenWidth(), ofGetScreenHeight(),OF_IMAGE_COLOR);
    
    bViewMovePaths = false;
    bPrint = false;
    bTrace = false;
    bInvert = false;
    
    tspRouteLength = 0;
    tspRouteStep = 0;
    tspRoute = NULL;
    bTsp = false;
}

void Tracer3D::selfSetupGuis(){
    guiAdd(shell);
    guiAdd(shader);
    guiAdd(plotter);
    
    lightAdd("DIR LIGHT ", OF_LIGHT_DIRECTIONAL );
    lightAdd("SPOT LIGHT", OF_LIGHT_SPOT);
}

void Tracer3D::selfSetupSystemGui(){
    
    sysGui->addLabel("Particle System");
    sysGui->addSlider("Repulsion_Force", 0.0, 1.0,&particlesRepulsion);
    sysGui->addSlider("Particles_Speed", 0.0, 1.0, &particlesSpeed);
    sysGui->addSlider("Particles_Min", 1.0, 5.0, &particlesMin);
    sysGui->addSlider("Particles_Max", 1.0, 10.0, &particlesMax);
    sysGui->addToggle("Invert", &bInvert);
    sysGui->addButton("POPULATE", &bPopulate);
    sysGui->addButton("CLEAR",&bClear);
    
    sysGui->addLabel("TSP");
    sysGui->addSlider("Threshold", 0.0, 1.0, &tspThreshold);
    sysGui->addToggle("CALCULATE", &bTsp);
    
    sysGui->addLabel("Tracer");
    sysGui->addSlider("Canny_Threshold1", 0.0, 1.0, &fCannyThreshold1);
    sysGui->addSlider("Canny_Threshold2",0.0,1.0,&fCannyThreshold2);
    sysGui->addSlider("MinGapLenght", 2.0, 12.0, &minGapLength);
    sysGui->addSlider("MinPathLenght", 0.0, 50.0, &minPathLength);
    sysGui->addSlider("pathSpace", 0.0, 10.0, &pathSpace);
    sysGui->addSlider("pathSmooth", 0.0, 10.0, &pathSmooth);
    sysGui->addButton("TRACE", &bTrace);
    
    sysGui->addSpacer();
    sysGui->addButton("PRINT", &bPrint);
}

void Tracer3D::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name == "DEBUG"){
        if (bDebug){
            camera->disableMouseInput();
        } else {
            particlesArea.saveSettings(getDataPath()+"area.xml");
        }
    } else if (name == "CALCULATE" || name == "Threshold"){
        tspRouteStep = 0;
        if (tspRoute != NULL){
            delete [] tspRoute;
            tspRoute = NULL;
        }
    } else if (name == "CLEAR"){
        if (bClear){
            for(int i = particles.size()-1; i >= 0 ; i-- ){
                delete particles[i];
                particles.erase(particles.begin()+i);
            }
            particles.clear();
            bTsp = false;
        }
    } else if (name == "POPULATE"){
        if (bPopulate){
            int step = (particlesMin+particlesMax)*2.0;
            for (int i = step/2; i < particlesArea.getWidth(); i+=step) {
                for(int j = step/2; j < particlesArea.getHeight(); j+=step){
                    
                    ofPoint pos = ofPoint(particlesArea.x+i,
                                          particlesArea.y+j);
                    
                    float brigtness = pixels.getColor(pos.x,pos.y).r;
                    if (bInvert) {
                        brigtness = 255-brigtness;
                    }
                    
                    int amount = ofMap(brigtness, 0, 255, 5, 0,true);
                    
                    for(int k = 0; k < amount; k++){
                        Particle *myParticle = new Particle();
                        myParticle->set(pos+ofPoint(ofRandom(-step,step),ofRandom(-step,step)));
                        myParticle->size = particlesMin+(brigtness/255.0)*particlesMax;
                        particles.push_back(myParticle);
                    }
                }
            }
            bPopulate = false;
        }
    }
}

void Tracer3D::selfSetupRenderGui(){
}

void Tracer3D::guiRenderEvent(ofxUIEventArgs &e){
    
}

//--------------------------------------------------- Tracer
void Tracer3D::removeIslands(ofPixels& img) {
	int w = img.getWidth(), h = img.getHeight();
	int ia1=-w-1,ia2=-w-0,ia3=-w+1,ib1=-0-1,ib3=-0+1,ic1=+w-1,ic2=+w-0,ic3=+w+1;
	unsigned char* p = img.getPixels();
	for(int y = 1; y + 1 < h; y++) {
		for(int x = 1; x + 1 < w; x++) {
			int i = y * w + x;
			if(p[i]) {
				if(!p[i+ia1]&&!p[i+ia2]&&!p[i+ia3]&&!p[i+ib1]&&!p[i+ib3]&&!p[i+ic1]&&!p[i+ic2]&&!p[i+ic3]) {
					p[i] = 0;
				}
			}
		}
	}
}

typedef std::pair<int, int> intPair;
vector<ofPolyline> Tracer3D::getPaths(ofPixels& img, float minGapLength, int minPathLength) {
	float minGapSquared = minGapLength * minGapLength;
	
	list<intPair> remaining;
	int w = img.getWidth(), h = img.getHeight();
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {
			if(img.getColor(x, y).getBrightness() > 128) {
				remaining.push_back(intPair(x, y));
			}
		}
	}
	
	vector<ofPolyline> paths;
	if(!remaining.empty()) {
		int x = remaining.back().first, y = remaining.back().second;
		while(!remaining.empty()) {
			int nearDistance = 0;
			list<intPair>::iterator nearIt, it;
			for(it = remaining.begin(); it != remaining.end(); it++) {
				intPair& cur = *it;
				int xd = x - cur.first, yd = y - cur.second;
				int distance = xd * xd + yd * yd;
				if(it == remaining.begin() || distance < nearDistance) {
					nearIt = it, nearDistance = distance;
					// break for immediate neighbors
					if(nearDistance < 4) {
						break;
					}
				}
			}
			intPair& next = *nearIt;
			x = next.first, y = next.second;
			if(paths.empty()) {
				paths.push_back(ofPolyline());
			} else if(nearDistance >= minGapSquared) {
				if(paths.back().size() < minPathLength) {
					paths.back().clear();
				} else {
					paths.push_back(ofPolyline());
				}
			}
			paths.back().addVertex(ofVec2f(x, y));
			remaining.erase(nearIt);
		}
	}
	
	return paths;
}

//--------------------------------------------------- TSP
void Tracer3D::applyTSP(){
    
    int temp;
    
    // Calculate and show "optimized" plotting path, beneath points.
    //
    ofPoint p1;
    if (tspRouteStep == 0){
        float threshold = particlesMin+tspThreshold*particlesMax;
        
        bTsp = true;
        
        // Begin process of optimizing plotting route, by flagging particles that will be shown.
        //
        tspRouteLength = 0;
        bool tspRouteTemp[particles.size()];
        for (int i = 0; i < particles.size(); ++i) {
            tspRouteTemp[i] = false;
            if (particles[i]->size < threshold){
                tspRouteTemp[i] = true;
                tspRouteLength++;

            }
        }
        
        if (tspRoute != NULL){
            delete [] tspRoute;
        }
        tspRoute = new int[tspRouteLength];
        
        int tempCounter = 0;
        for (int i = 0; i < particles.size(); ++i) {
            if (tspRouteTemp[i]){
                tspRoute[tempCounter]=i;
                tempCounter++;
            }
        }
        // These are the ONLY points to be drawn in the tour.
    }
    
    if (tspRouteStep < (tspRouteLength - 2)){
        
        // Nearest neighbor ("Simple, Greedy") algorithm path optimization:
        
        int StopPoint = tspRouteStep + 1000;      // 1000 steps per frame displayed; you can edit this number!
        
        if (StopPoint > (tspRouteLength - 1))
            StopPoint = tspRouteLength - 1;
        
        for (int i = tspRouteStep; i < StopPoint; ++i) {
            
            p1 = *particles[ tspRoute[tspRouteStep] ];
            int ClosestParticle = 0;
            float distMin = 1000000;
            
            for (int j = tspRouteStep + 1; j < (tspRouteLength - 1); ++j) {
                ofPoint p2 = *particles[tspRoute[j]];
                
                float  dx = p1.x - p2.x;
                float  dy = p1.y - p2.y;
                float  distance = (float) (dx*dx+dy*dy);  // Only looking for closest; do not need sqrt factor!
                
                if ( distance < distMin) {
                    ClosestParticle = j;
                    distMin = distance;
                }
            }
            
            temp = tspRoute[tspRouteStep+1];
            
            tspRoute[tspRouteStep+1] = tspRoute[ClosestParticle];
            tspRoute[ClosestParticle] = temp;
            
            if (tspRouteStep < (tspRouteLength - 1))
                tspRouteStep++;
        }
    } else if (tspRouteLength > 0){
        
        // Initial routing is complete
        //
        
        // 2-opt heuristic optimization:
        // Identify a pair of edges that would become shorter by reversing part of the tour.
        
        for (int i = 0; i < 90000; ++i) {   // 1000 tests per frame; you can edit this number.
            
            int indexA = floor( ofRandom(tspRouteLength - 1) );
            int indexB = floor( ofRandom(tspRouteLength - 1) );
            
            while (indexA+1 >= tspRouteLength || indexB+1 >= tspRouteLength ) {
                indexA = floor( ofRandom(tspRouteLength - 1) );
                indexB = floor( ofRandom(tspRouteLength - 1) );
            }
            
            if (abs(indexA  - indexB) < 2)
                continue;
            
            if (indexB < indexA){
                // swap A, B.
                temp = indexB;
                indexB = indexA;
                indexA = temp;
            }
            
            ofPoint a0 = *particles[tspRoute[indexA]];
            ofPoint a1 = *particles[tspRoute[indexA+1]];
            ofPoint b0 = *particles[tspRoute[indexB]];
            ofPoint b1 = *particles[tspRoute[indexB+1]];
            
            // Original distance:
            float  dx = a0.x - a1.x;
            float  dy = a0.y - a1.y;
            float  distance = (float) (dx*dx+dy*dy);  // Only a comparison; do not need sqrt factor!
            dx = b0.x - b1.x;
            dy = b0.y - b1.y;
            distance += (float) (dx*dx+dy*dy);  //  Only a comparison; do not need sqrt factor!
            
            // Possible shorter distance?
            dx = a0.x - b0.x;
            dy = a0.y - b0.y;
            float distance2 = (float) (dx*dx+dy*dy);  //  Only a comparison; do not need sqrt factor!
            dx = a1.x - b1.x;
            dy = a1.y - b1.y;
            distance2 += (float) (dx*dx+dy*dy);  // Only a comparison; do not need sqrt factor!
            
            if (distance2 < distance){
                
                // Reverse tour between a1 and b0.
                //
                int indexhigh = indexB;
                int indexlow = indexA + 1;
                
                while (indexhigh > indexlow){
                    temp = tspRoute[indexlow];
                    tspRoute[indexlow] = tspRoute[indexhigh];
                    tspRoute[indexhigh] = temp;
                    
                    indexhigh--;
                    indexlow++;
                }
            }
        }
        
        tspPath.clear();
        for (int i = 0; i < tspRouteLength; i++) {
            tspPath.addVertex( *particles[ tspRoute[i]] );
        }
    }
}

//---------------------------------------------------

void Tracer3D::selfBegin(){
	
}


void Tracer3D::selfUpdate(){
    shell.update();
    
    fbo.getTextureReference().readToPixels(pixels);
    
    if (particles.size()>0){
        
        // sort all the particle
        //
        if (bTsp){
            applyTSP();
            
        } else {
            sort( particles.begin(), particles.end(), comparisonFunction );
            float zone = (particlesMin+particlesMax)*4.0;
            
            for (int i = 0; i < particles.size(); i++){
                for (int j = i-1; j >= 0; j--){
                    if ( fabs(particles[j]->x - particles[i]->x) > zone)
                        break;
                    
                    if (particlesRepulsion > 0)
                        particles[i]->addRepulsionForce( particles[j], particlesRepulsion );
                }
            }
            
            for(int i = particles.size()-1; i >= 0 ; i-- ){
                if ( particlesArea.inside( *particles[i] ) ){
                    float brigtness = pixels.getColor(particles[i]->x,particles[i]->y).r;
                    if (bInvert) {
                        brigtness = 255-brigtness;
                    }
                    particles[i]->size = ofLerp(particles[i]->size, particlesMin+(brigtness/255.0)*particlesMax, 0.04);
                    
                    particles[i]->update( particlesSpeed );
                } else {
                    delete particles[i];
                    particles.erase(particles.begin()+i);
                }
            }
        }
    }
    
    if (bTrace){
        ofPixels pixels, gray;
        fbo.getTextureReference().readToPixels(pixels);
        
        convertColor(pixels, gray, CV_RGB2GRAY);
		Canny(gray, pixels, fCannyThreshold1*255., fCannyThreshold2*255., 3);
        
        paths = getPaths(pixels, minGapLength, minPathLength);
        
        for (int i = 0; i < paths.size(); i++) {
            ofPolyline contornoSpaced;
            contornoSpaced = paths[i].getResampledBySpacing(pathSpace);
            
            ofPolyline contornoSmooth;
            contornoSmooth = contornoSpaced.getSmoothed(pathSmooth);
            
            paths[i] = contornoSmooth;
        }
        
        bTrace = false;
    }
    
    plotter.update();
    if(bPrint){
        if (tspPath.size()>0){
            paths.push_back(tspPath);
        }
        plotter.print(paths);
        bPrint = false;
    }
}

void Tracer3D::selfDraw(){
    materials["MATERIAL 1"]->begin();

    
    shell.draw();
    
    materials["MATERIAL 1"]->end();
}

void Tracer3D::selfDrawOverlay(){
    if(bDebug){
        particlesArea.draw();
    }
};

void Tracer3D::selfPostDraw(){

    if(fbo.getWidth() != ofGetWidth() ||
       fbo.getHeight() != ofGetHeight()){
        fbo.allocate(ofGetWidth(), ofGetHeight());
    }
    
    fbo.begin();
    ofClear(0);
    shader.begin();
    shader.getShader().setUniformTexture("tex1", ofxPro::getRenderTarget().getDepthTexture(), 1);
    ofxPro::selfPostDraw();
    shader.end();
    fbo.end();
    
    ofSetColor(255);
    fbo.draw(0, 0);
    
    //-------------------------------
    
    ofPushMatrix();
    ofPushStyle();
    ofDisableLighting();
    ofEnableAlphaBlending();
    
    ofSetColor(255);
    ofFill();
    for (int i = 0; i < particles.size(); i++){
        ofSetColor(0);
		particles[i]->draw();
	}
    
    ofSetColor(255,0,0);
    for(int i = 0; i < paths.size(); i++) {
		ofSetColor(yellowPrint);
		paths[i].draw();
		if(i + 1 < paths.size() && bViewMovePaths) {
			ofVec2f endPoint = paths[i].getVertices()[paths[i].size() - 1];
			ofVec2f startPoint = paths[i + 1].getVertices()[0];
			ofSetColor(magentaPrint, 128);
			ofLine(endPoint, startPoint);
		}
	}
    
    ofSetColor(255,0,0);
    ofNoFill();
    tspPath.draw();
    
    ofSetColor(255,0,0);
    plotter.draw();
    
    ofPopStyle();
    ofPopMatrix();
}

void Tracer3D::selfEnd(){
	
}

void Tracer3D::selfExit(){
	
}

void Tracer3D::selfKeyPressed(ofKeyEventArgs & args){
    if (!bDebug){
        if(args.key == ' '){
            paths.clear();
            tspPath.clear();
        }
    }
}

void Tracer3D::selfKeyReleased(ofKeyEventArgs & args){
	
}

void Tracer3D::selfMouseMoved(ofMouseEventArgs& data){
    
}

void Tracer3D::selfMousePressed(ofMouseEventArgs& data){
	if (bDebug){
        if (particlesArea.mouseDragged(ofPoint(data.x,data.y))){
            camera->disableMouseInput();
        }
    }
}

void Tracer3D::selfMouseDragged(ofMouseEventArgs& data){
    if (bDebug){
        particlesArea.mouseDragged(ofPoint(data.x,data.y));
    }
}

void Tracer3D::selfMouseReleased(ofMouseEventArgs& data){
    if (bDebug){
        camera->enableMouseInput();
    }
}