//
//  Tracer2D.cpp
//  PrintMaking
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//
#include "Tracer2D.h"

using namespace ofxCv;
using namespace cv;

bool compFunc( Particle * a, Particle * b ) {
	return a->x < b->x;
}

void Tracer2D::selfSetup(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    ofEnableArbTex();
    
    shader.loadFrag(getDataPath()+"shaders/ADJUSTMENTS.frag");
//    image.loadImage(getDataPath()+"images/empathy.jpg");

    pixels.allocate(ofGetScreenWidth(), ofGetScreenHeight(),OF_IMAGE_COLOR);
    
    patternSelected = "default";
    director.loadSettings(getDataPath()+"patterns/"+patternSelected+".xml",patternSelected);
    
    bWalkers = false;
    bContour = false;
    
//    bViewMovePaths = false;
    
    tspRouteLength = 0;
    tspRouteStep = 0;
    tspRoute = NULL;
    bTsp = false;
    bTraceTSP = false;
    
    bPrint = false;
}

void Tracer2D::selfSetupGuis(){
    //  Adding Plotter
    //
    guiAdd(plotter);
    
    //  Adjustments with Shader
    //
    guiAdd(shader);
    
    //  UI Patterns and Pattern Editor
    //
    director.setFont(guiTemplate->getFontMedium() );
    UIReference guiPatterns( new ofxUISuperCanvas("PATTERNS" , guiTemplate) );
    guiPatterns->copyCanvasStyle( guiTemplate );
    guiPatterns->copyCanvasProperties( guiTemplate );
    guiPatterns->setName( "PATTERNS" );
    guiPatterns->setWidgetFontSize(OFX_UI_FONT_SMALL);
    ofxUIToggle *toggle = guiPatterns->addToggle("ENABLE", &bPatterns);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    guiPatterns->resetPlacer();
    guiPatterns->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    guiPatterns->addWidgetToHeader(toggle);
    guiPatterns->addSpacer();
    
    guiPatterns->addSlider("Scale",1.0,20.0,&walkersSpaceBetween);
    guiPatterns->addSlider("Step",0.0,1.0,&walkersSpeed);
    guiPatterns->addButton("FILL",&bWalkers);
    
    guiPatterns->addSpacer();
    guiPatterns->addSlider("LINE SPACE",2.0,30.0,&lineSpace);
    guiPatterns->addSlider("LINE ANGLE",-1.0,1.0,&lineWalkerAngle);
    guiPatterns->addButton("FILL LINES",&bLineWalkers);
    
    guiPatterns->addSpacer();
    guiPatterns->addButton("ADD", &patternAdd);
    guiPatterns->addButton("CLEAN", &patternClean);
    
    vector<string> patternsList;
    string path = getDataPath()+"patterns/";
    ofDirectory patternsDir(path);
    if(patternsDir.exists()){
		patternsDir.listDir();
        int total = patternsDir.getFiles().size();
        for (int i = 0; i < total; i++) {
            patternsList.push_back( patternsDir.getName(i) );
        }
    }
    patterns = guiPatterns->addDropDownList("LOAD", patternsList);
    ofAddListener(guiPatterns->newGUIEvent, this, &Tracer2D::selfGuiEvent);
    guis.push_back(guiPatterns);
    
    //  UI CONTOUR
    //
    UIReference guiContour( new ofxUISuperCanvas("CONTOUR" , guiTemplate) );
    guiContour->copyCanvasStyle( guiTemplate );
    guiContour->copyCanvasProperties( guiTemplate );
    guiContour->setName( "CONTOUR" );
    guiContour->setWidgetFontSize(OFX_UI_FONT_SMALL);
    toggle = guiContour->addToggle("ENABLE", &bContour);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    guiContour->resetPlacer();
    guiContour->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    guiContour->addWidgetToHeader(toggle);
    guiContour->addSpacer();
    
    guiContour->addSlider("Canny_Threshold1", 0.0, 1.0, &fCannyThreshold1);
    guiContour->addSlider("Canny_Threshold2",0.0,1.0,&fCannyThreshold2);
    guiContour->addSlider("MinGapLenght", 2.0, 12.0, &minGapLength);
    guiContour->addSlider("MinPathLenght", 0.0, 50.0, &minPathLength);
    guiContour->addSlider("pathSpace", 0.0, 10.0, &contourSpace);
    guiContour->addSlider("pathSmooth", 0.0, 10.0, &contourSmooth);
    guiContour->addSpacer();
    guiContour->addButton("TRACE", &bTrace);
    ofAddListener(guiContour->newGUIEvent, this, &Tracer2D::selfGuiEvent);
    guis.push_back(guiContour);
    
    //  UI TSP
    //
    UIReference guiTSP( new ofxUISuperCanvas("TSP" , guiTemplate) );
    guiTSP->copyCanvasStyle( guiTemplate );
    guiTSP->copyCanvasProperties( guiTemplate );
    guiTSP->setName( "TSP" );
    guiTSP->setWidgetFontSize(OFX_UI_FONT_SMALL);
    toggle = guiTSP->addToggle("ENABLE", &bTsp);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    guiTSP->resetPlacer();
    guiTSP->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    guiTSP->addWidgetToHeader(toggle);
    guiTSP->addSpacer();
    guiTSP->addLabel("Particle System");
    guiTSP->addSlider("Repulsion_Force", 0.0, 1.0,&particlesRepulsion);
    guiTSP->addSlider("Particles_Speed", 0.0, 1.0, &particlesSpeed);
    guiTSP->addSlider("Particles_Min", 1.0, 5.0, &particlesMin);
    guiTSP->addSlider("Particles_Max", 1.0, 10.0, &particlesMax);
    guiTSP->addToggle("Invert", &bInvert);
    guiTSP->addButton("POPULATE", &bPopulate);
    guiTSP->addButton("CLEAR",&bClear);
    guiTSP->addLabel("TSP");
    guiTSP->addSlider("Threshold", 0.0, 1.0, &tspThreshold);
//    guiTSP->addSlider("Density",0.0,1.0,&tspDensity);
    guiTSP->addToggle("DO", &bTraceTSP);
    ofAddListener(guiTSP->newGUIEvent, this, &Tracer2D::selfGuiEvent);
    guis.push_back(guiTSP);
}

void Tracer2D::selfGuiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if(name == "Canny_Threshold1" || name == "Canny_Threshold2" ||
       name == "MinGapLenght" || name == "MinPathLenght" ||
       name == "pathSpace" || name == "pathSmooth"){
        bTrace = true;
    } else if(name == "DO" || name == "Density" || name == "Threshold"){
        if (bTraceTSP){
            tspRouteStep = 0;
            if (tspRoute != NULL){
                delete [] tspRoute;
                tspRoute = NULL;
            }
        }
    } else if (name == "CLEAR"){
        
    } else if (name == "POPULATE"){
        if (bPopulate){
            particlesArea.setFromCenter(ofGetWidth()*0.5, ofGetHeight()*0.5, image.getWidth(), image.getHeight());
            
            int step = (particlesMin+particlesMax)*5.0;
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
                        myParticle->color.set(background->getUIBrightness());
                        particles.push_back(myParticle);
                    }
                }
            }
            bPopulate = false;
        }
    } else if(name == "FILL"){
        if (bWalkers){
            ofRectangle area;
            area.set(0, 0, ofGetWidth(), ofGetHeight());
            
            float minArea = ofGetWidth()*0.5-image.getWidth()*0.5;
            int counter = 0;
            for (int i = walkersSpaceBetween*0.5; i < area.getHeight(); i+= (int)walkersSpaceBetween ) {
                Walker newWalker;
                newWalker.bOverBlob = false;
                
                /// its possible to have a little random from x start position,
                //  so each draw not begin at same ???
                //
                //  NICE!!!
                //
//                newWalker.set(minArea+2 + ofRandom(-7, 7), area.y+i);
                
                float ratio = director.width/director.height;
                float offset = (counter%2==0)?0:(walkersSpaceBetween*ratio)*0.5;
                newWalker.set(minArea+2+offset,area.y+i);
                walkers.push_back(newWalker);
                counter++;
            }
            
            walkersStep = 0;
        }
    } else if (name == "ADD"){
        if(patternAdd){
            string lineName = ofSystemTextBoxDialog("Save Line as");
            if(lineName.length()){
                patterns->addToggle(lineName+".xml");
                patternSelected=lineName;
                if(director.saveSettings(getDataPath()+"patterns/"+patternSelected+".xml",patternSelected)){
                    cout << "SAVED" << endl;
                }else{
                    cout << "ERROR SAVING " << lineName << endl;
                }
                
                
            }
        }
    } else if(name=="CLEAN"){
        director.clear();
    }else if (name == "FILL LINES"){
        
        if(bLineWalkers){
            ofRectangle area;
            area.set(ofGetWidth()*.5 - image.getWidth()/2, -200, image.getWidth(), ofGetHeight());
            
            for (int i = lineSpace; i < ofGetHeight(); i+= (int)lineSpace ) {
                lineWalker newWalker;
                newWalker.bOverBlob = false;
                newWalker.set(area.x, area.y+i);
                lineWalkers.push_back(newWalker);
            }
        }
    }
    
    string extention = ".xml";
    if (name.find(extention) != string::npos){
        name.replace(name.find(extention), extention.length(), "");
        patternSelected = name;
        director.loadSettings(getDataPath()+"patterns/"+patternSelected+".xml",patternSelected);
    }
}

void Tracer2D::selfSetupSystemGui(){
    sysGui->addButton("PRINT", &bPrint);
    sysGui->addButton("CLEAR", &bClearAll);
    sysGui->addSpacer();
    sysGui->addToggle("SHOW_IMAGE", &bImage);
    
    vector<string> imagesList;
    string path = getDataPath()+"images/";
    ofDirectory imagesDir(path);
    if(imagesDir.exists()){
		imagesDir.listDir();
        int total = imagesDir.getFiles().size();
        for (int i = 0; i < total; i++) {
            imagesList.push_back( imagesDir.getName(i) );
        }
    }
    images = sysGui->addDropDownList("LOAD_IMAGE", imagesList);
}

//---------------------------------------------------

void Tracer2D::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name.find('.') != string::npos){
        cout << name << endl;
        image.loadImage(getDataPath()+"images/"+name);
    }
}

void Tracer2D::loadImage(string _path){
    ofFile file(_path);
    string extension = file.getExtension();
    
    if(extension == "jpg" ||
       extension == "JPG" ||
       extension == "jpeg" ||
       extension == "JPEG" ||
       extension == "png" ||
       extension == "PNG" ){
        file.copyTo(getDataPath()+"images/");
        
        image.loadImage(file);
        
        vector<string> imagesList;
        string path = getDataPath()+"images/";
        ofDirectory imagesDir(path);
        if(imagesDir.exists()){
            imagesDir.listDir();
            int total = imagesDir.getFiles().size();
            for (int i = 0; i < total; i++) {
                imagesList.push_back( imagesDir.getName(i) );
            }
        }
        images->clearToggles();
        images->addToggles(imagesList);
    }
}

//--------------------------------------------------- Tracer
void Tracer2D::removeIslands(ofPixels& img) {
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
vector<ofPolyline> Tracer2D::getPaths(ofPixels& img, float minGapLength, int minPathLength) {
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

void Tracer2D::selfUpdate(){
    
    if(bClearAll){
        contourLines.clear();
        walkersLines.clear();
        lineWalkers.clear();
        walkers.clear();
        tspPath.clear();
        tspRouteStep = 0;
        bTraceTSP = false;
        bClearAll = false;
    }
    
    if (bClear){
        for(int i = particles.size()-1; i >= 0 ; i-- ){
            delete particles[i];
            particles.erase(particles.begin()+i);
        }
        particles.clear();
        bTraceTSP = false;
        bClear = false;
    }
    
    //  CONTOUR
    //
    if(bTrace){
        getRenderTarget().getTextureReference().readToPixels(pixels);
        ofPixels gray;
        convertColor(pixels, gray, CV_RGB2GRAY);
        
		Canny(gray, pixels, fCannyThreshold1*255., fCannyThreshold2*255., 3);
        
        contourLines.clear();
        vector<ofPolyline> tmp = getPaths(pixels, minGapLength, minPathLength);
        for (int i = 0; i < tmp.size(); i++) {
            ofPolyline contornoSpaced;
            ofPolyline contornoSmooth;
            
            if (contourSpace>0.0){
                contornoSpaced = tmp[i].getResampledBySpacing(contourSpace);
            } else {
                contornoSpaced = tmp[i];
            }
            
            if (contourSmooth>0.0){
                contornoSmooth = contornoSpaced.getSmoothed(contourSmooth);
            } else {
                contornoSmooth = contornoSpaced;
            }
            
            contourLines.push_back(contornoSmooth);
        }
        
        bTrace = false;
    }
    
    //  TSP
    //
    if (particles.size()>0){
        if(bTraceTSP&&bTsp){
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
        } else {
            getRenderTarget().getTextureReference().readToPixels(pixels);
            sort(particles.begin(), particles.end(), compFunc);
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
    
    //  PATTERNS
    //
    if (bGui){
        director.brigtness = background->getUIBrightness();
    }
    if (walkers.size()>0){
        pixels.allocate(getRenderTarget().getWidth(), getRenderTarget().getHeight(),OF_IMAGE_COLOR);
        getRenderTarget().getTextureReference().readToPixels(pixels);
        
       // ofRectangle area;
       // area.set(0, 0, getRenderTarget().getWidth(), getRenderTarget().getHeight());
        
        ofRectangle area;
        area.set(ofGetWidth()*.5 - image.getWidth()/2, ofGetHeight()*.5 - image.getHeight()/2, image.getWidth(), image.getHeight());
        
        area.x += 1;
        area.y += 1;
        area.width -=2;
        area.height -=2;
        float maxArea = ofGetWidth()*0.5+image.getWidth()*0.5;
        for(int i = walkers.size()-1; i >= 0 ; i-- ){
            if ( area.inside( walkers[i] ) && walkers[i].x <  maxArea){
                walkers[i].checkOver(pixels.getColor(walkers[i].x, walkers[i].y).getBrightness() < 255*0.5);
                walkers[i] += director.getDirForStep(walkersStep, walkersSpaceBetween*0.5)*walkersSpeed;
            } else {
                for(int j=0;j<walkers[i].lines.size();j++){
                    walkersLines.push_back(walkers[i].lines[j]);
                }
                walkers.erase(walkers.begin()+i);
            }
        }
        
        walkersStep += 1.0*walkersSpeed;
    }
    
    ///// LINE WALKER
    ///// como haces para que siempre haya walkers cuando van en angulo???? 
    
    
    if (lineWalkers.size()>0){
        pixels.allocate(getRenderTarget().getWidth(), getRenderTarget().getHeight(),OF_IMAGE_COLOR);
        getRenderTarget().getTextureReference().readToPixels(pixels);
        
        ofRectangle area;
        area.set(0, 0, getRenderTarget().getWidth(), getRenderTarget().getHeight());
      
         float maxArea = ofGetWidth()*0.5+image.getWidth()*0.5;
        
        for(int i = lineWalkers.size()-1; i >= 0 ; i-- ){
            if ( area.inside( lineWalkers[i] ) && lineWalkers[i].x < maxArea){
                lineWalkers[i].checkOver(pixels.getColor(lineWalkers[i].x, lineWalkers[i].y).getBrightness() < 255*0.5);
                lineWalkers[i].addForce(ofPoint(cos(lineWalkerAngle),sin(lineWalkerAngle)).normalize());
                //lineWalkers[i].addForce(ofPoint(1.0,0.0));
                lineWalkers[i].update(.3);
            } else {
                for(int j=0;j<lineWalkers[i].lines.size();j++){
                    walkersLines.push_back(lineWalkers[i].lines[j]);
                }
                lineWalkers.erase(lineWalkers.begin()+i);
            }
        }
    }
    
    //  PLOTTER
    //
    if(bPrint){
        
        /// no podemos vaciar los vectores sin haber acabado la impresion
        /// cada vez que se manda una instruccion el code pasa por aqui
        /// y si vaciamos los vectores.... solo ejecuta la primera orden,
        /// porque al volver a mandar el vector este esta vacio
        
        vector<ofPolyline> paths;
        if(bContour){
            for(int i=0;i<contourLines.size();i++){
                paths.push_back(contourLines[i]);
            }
           // contourLines.clear();
        }
        
        if(bPatterns){
            for(int i=0;i<walkersLines.size();i++){
                paths.push_back(walkersLines[i]);
            }
           // walkersLines.clear();
        }
        
        if(bTsp){
            paths.push_back(tspPath);
           // tspPath.clear();
        }
        
        plotter.print(paths);
        bPrint = false;
    }
    
    plotter.update();
}

void Tracer2D::selfDraw(){
    if (bImage){
        ofPushMatrix();
        ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
        shader.begin();
        shader.getShader().setUniformTexture("tex0", image, 1);
        image.draw(-image.getWidth()*0.5, -image.getHeight()*0.5);
        shader.end();
        ofPopMatrix();
    }
}

void Tracer2D::selfPostDraw(){
    ofxPro::selfPostDraw();
    
    ofFill();
    ofSetColor(255);
    for (int i = 0; i < particles.size(); i++){
        particles[i]->draw();
    }
    
    if(bContour) {
        ofNoFill();
        ofSetColor(255,0,0);
        for(int i=0;i<contourLines.size();i++){
            contourLines[i].draw();
        }
    }
    
    if(bTsp){
    
        ofNoFill();
        ofSetColor(255,0,0);
        tspPath.draw();
    }
    
    if(bPatterns){
        ofNoFill();
        ofSetColor(255,0,0);
        for(int i=0;i<walkersLines.size();i++){
            walkersLines[i].draw();
        }
        
        
        if(bGui){
            director.draw();
            ofFill();
            for(int i = 0; i < walkers.size(); i++) {
                walkers[i].draw();
            }
            
            for(int i = 0; i < lineWalkers.size(); i++){
                lineWalkers[i].draw();
            }
            
        }
    }
    
    plotter.draw();
}

void Tracer2D::selfEnd(){
	director.saveSettings(getDataPath()+"patterns/"+patternSelected+".xml",patternSelected);
}

void Tracer2D::selfExit(){
	director.saveSettings(getDataPath()+"patterns/"+patternSelected+".xml",patternSelected);
}

void Tracer2D::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key == ' '){
        bClearAll = true;
    } else if(args.key == 'p'){
        bPrint = true;
    }
}

void Tracer2D::selfKeyReleased(ofKeyEventArgs & args){
	
}

void Tracer2D::selfMouseMoved(ofMouseEventArgs& data){
	
}

void Tracer2D::selfMousePressed(ofMouseEventArgs& data){
    director.mousePressed(ofPoint(data.x,data.y));
}

void Tracer2D::selfMouseDragged(ofMouseEventArgs& data){
    if(bGui&&bPatterns){
        if(!director.mouseDragged(ofPoint(data.x,data.y))){
        }
    }
}

void Tracer2D::selfMouseReleased(ofMouseEventArgs& data){
    if(bGui&&bPatterns){
        if ( director.mouseDragged(ofPoint(data.x,data.y)) ){
            director.saveSettings(getDataPath()+"patterns/"+patternSelected+".xml",patternSelected);
        }
    }
}