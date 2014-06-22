//
//  LinesMaker.cpp
//  PrintMaking
//
//  Created by Patricio Gonzalez Vivo on 10/16/13.
//
//

#include "LinesMaker.h"

void LinesMaker::selfSetup(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    bCursor = true;
    ofHideCursor();
    
    nBookmark = -1;
    nSelected = -1;
    lineStroke = 1;
    
    shader.loadFrag(getDataPath()+"shaders/shader.frag");
    ofLoadImage(shaderTexture, getDataPath()+"texture.jpg");
    
    grid.setFromCenter(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
    grid.setResolutionToInches();
}

void LinesMaker::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(grid);
    guiAdd(shader);
}

void LinesMaker::selfSetupSystemGui(){
    
}

void LinesMaker::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name == "DEBUG"){
    }
}

void LinesMaker::selfSetupRenderGui(){
    
}

void LinesMaker::guiRenderEvent(ofxUIEventArgs &e){
    
}

void LinesMaker::selfBegin(){
    player.play();
    actualLine.setTimeLine( &player );
    actualLine.stroke = lineStroke;
}

void LinesMaker::selfUpdate(){
    for (int i = 0; i < storedLines.size(); i++){
        storedLines[i].updateBox();
    }
    
    player.update();
}

void LinesMaker::selfDraw(){
    
    float time = player.getPosition()*player.getLength();
    float brigness = background->getUIBrightness();

    //  Current lineStroke
    //
    ofPushMatrix();
    ofSetColor(255,0,0,100);
    ofTranslate(0, ofGetHeight() - player.height);
    float strokeY = ofMap(lineStroke, 0.0,MAX_LINE_WIDTH, player.height*0.9-REFERENCEBOX_HEIGHT*0.5,player.height*0.1+REFERENCEBOX_HEIGHT*0.5);
    ofBeginShape();
    ofVertex(0, strokeY-MARK_SIZE);ofVertex(0, strokeY+MARK_SIZE);ofVertex(MARK_SIZE, strokeY);
    ofEndShape(false);
    ofLine(MARK_SIZE, strokeY, ofGetWidth(), strokeY);
    ofPopMatrix();
    
    //  LINES
    //
    ofEnableSmoothing();
    
    ofSetColor( brigness*255 );
    actualLine.draw(time);
    
    ofSetColor(180);
    for (int i = 0; i < storedLines.size(); i++){
        
        storedLines[i].bSelected = false;
        
        if ( i == nSelected){
            ofSetColor(255,0,0);
            storedLines[i].bSelected = true;
        } else {
            ofSetColor( brigness*255 );
        }
        
        storedLines[i].draw( time );
    }
    
    ofDisableSmoothing();
}

void LinesMaker::selfDrawOverlay(){
    if (bCursor){
        ofPushMatrix();
        ofPushStyle();
        
        ofDisableSmoothing();
        ofSetColor(255);
        ofSetLineWidth(0.1);
        ofTranslate(ofGetMouseX(), ofGetMouseY());
        ofCircle(0, 0, 1);
        int size = 5;
        int margin = 3;
        ofLine(-margin, 0, -margin-size, 0);
        ofLine(margin, 0, margin+size, 0);
        ofLine(0, -margin, 0, -margin-size);
        ofLine(0, margin, 0, margin+size);
        
        ofEnableSmoothing();
        ofPopStyle();
        ofPopMatrix();
    }
    
    player.draw();
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
    grid.draw();
    ofPopMatrix();
}

void LinesMaker::selfPostDraw(){
    shader.begin();
    shader.getShader().setUniformTexture("tex1", shaderTexture, 1);
    shader.getShader().setUniform2f("textureResolution", shaderTexture.getWidth() , shaderTexture.getHeight());
    ofxPro::selfPostDraw();
    shader.end();
}

void LinesMaker::selfEnd(){
    
}

void LinesMaker::selfExit(){
    
}

void LinesMaker::selfKeyPressed(ofKeyEventArgs & args){
    float strokeJump = 0.1;
    
    if (args.key == ' '){
        if (player.getIsPlaying())
            player.pause();
        else
            player.play();
        
    } else if (args.key == OF_KEY_BACKSPACE){
        if (nSelected != -1){
            storedLines.erase(storedLines.begin()+nSelected);
            nSelected = -1;
        } else if (nBookmark != -1){
            player.bookmarks.erase(player.bookmarks.begin()+nBookmark);
            nBookmark = -1;
        }
    } else if (args.key == 's'){
        saveSecuence(getDataPath()+"test.xml");
    } else if (args.key == 'c'){
        if ((storedLines.size() == 0) && (actualLine.size() <= 2))
            player.bookmarks.clear();
        
        storedLines.clear();
        actualLine.clear();
    } else if (args.key == 'v'){
        player.bookmarks.clear();
    } else if (args.key == 'b'){
        if (player.getIsPlaying())
            player.addBookmark();
    }
}

void LinesMaker::selfKeyReleased(ofKeyEventArgs & args){

}

void LinesMaker::selfMouseMoved(ofMouseEventArgs& data){

	if (player.inside(data.x,data.y)){
        if (bCursor)
            ofShowCursor();
        
        bCursor = false;
    } else {
        if (!bCursor)
            ofHideCursor();
        
        bCursor = true;
    }
}

void LinesMaker::selfMousePressed(ofMouseEventArgs& data){
    //  Any new click could be de start of new gesture
    //
    nSelected = -1;
    nBookmark = -1;
    
    if (player.inside(data.x,data.y)){
        
        //  If is inside the canvas, check if it's clicking a referenceBox
        //
        for (int i = 0; i < storedLines.size(); i++){
            if ( storedLines[i].checkOver(data.x,data.y) ){
                nSelected = i;
                selectOffSet.x = data.x - storedLines[i].getReferenceBox().x;
                selectOffSet.y = data.y - storedLines[i].getReferenceBox().y;
                break;
            }
        }
        
        //  If no reference box is selected try with a bookmark
        //
        if (nSelected == -1){
            for (int i = 0; i < player.bookmarks.size(); i++){
                int mark= player.bookmarks[i] * ofGetWidth();
                if ( data.x > mark-5 && data.x < mark+5 ){
                    nBookmark = i;
                    break;
                }
            }
        }
        
    } else {
        
        //  On canvas, draw something
        //
        actualLine.addPoint(ofPoint(data.x,data.y));
    }
}

void LinesMaker::selfMouseDragged(ofMouseEventArgs& data){
    if (player.inside(data.x,data.y)){
        
        //  If it was draing, finish the line
        //
        if (actualLine.size() > 1){
            actualLine.setFinish();
            storedLines.push_back(actualLine);
            actualLine.clear();
        }
        
        //  If there is a selected refBox, move it
        //
        if (nSelected != -1){
            storedLines[nSelected].moveRefTo(data.x-selectOffSet.x,data.y-selectOffSet.y);
        }
        //  If a bookmark is selected
        //
        else if (nBookmark != -1){
            player.bookmarks[nBookmark] = (float)data.x/(float)ofGetWidth();
        }
        //  What about LINE STROKE
        //
        else if ( data.x < MARK_SIZE*2.0 ){
            float strokeY = ofMap(lineStroke,
                                  0.0,MAX_LINE_WIDTH,
                                  player.height*0.9-REFERENCEBOX_HEIGHT*0.5,
                                  player.height*0.1+REFERENCEBOX_HEIGHT*0.5);
            float mY = data.y - (ofGetHeight()-player.height);
            if ( abs(strokeY - mY) < MARK_SIZE*2.0 ){
                lineStroke = ofMap(mY,
                                   player.height*0.9-REFERENCEBOX_HEIGHT*0.5,
                                   player.height*0.1+REFERENCEBOX_HEIGHT*0.5,
                                   0.0,
                                   MAX_LINE_WIDTH,
                                   true);
                actualLine.stroke = lineStroke;
            }
        }
        //  If it's nothing selected, just move the header of the player
        //
        else {
            player.setPosition((float)data.x/ofGetWidth());
            nSelected = -1;
            nBookmark = -1;
        }
        
    } else {
        
        //  If it's on canvas, draw!
        //
        actualLine.addPoint(ofPoint(data.x,data.y));
    }
}

void LinesMaker::selfMouseReleased(ofMouseEventArgs& data){
    nSelected = -1;
    nBookmark = -1;
    
    if (player.inside(data.x, data.y)){
        
    } else {
        actualLine.setFinish();
        storedLines.push_back(actualLine);
        actualLine.clear();
    }
}

bool LinesMaker::loadSecuence(string _file){
    vector<string> file = ofSplitString(_file, ".");
    
    if ( file[1] == "xml" ){
        ofxXmlSettings XML;
        
        if (XML.loadFile(_file)){
            player.setLength(XML.getValue("length", 1.0));
            
            actualLine.clear();
            storedLines.clear();
            int totalLines = XML.getNumTags("line");
            
            for (int i = 0; i < totalLines; i++) {
                XML.pushTag("line",i);
                
                float lStroke = XML.getValue("stroke", 0.0f);
                float lSTime = XML.getValue("startTime", 0.0f);
                
                int totalPnts = XML.getNumTags("pnt");
                vector<TimePoint> lPnts;
                for (int j = 0; j < totalPnts; j++){
                    TimePoint newPoint;
                    newPoint.x = XML.getValue("pnt:x", 0.0, j);
                    newPoint.y = XML.getValue("pnt:y", 0.0, j);
                    newPoint.z = XML.getValue("pnt:z", 0.0, j);
                    lPnts.push_back(newPoint);
                }
                
                Line newLine = Line(lPnts, lSTime, lStroke);
                newLine.setTimeLine(&player);
                storedLines.push_back( newLine );
                
                XML.popTag();
            }
            
            int totalBookmark = XML.getNumTags("bookmark");
            for (int i = 0; i < totalBookmark; i++) {
                player.bookmarks.push_back(XML.getValue("bookmark", 0.0, i));
            }
        }
        
        player.play();
        actualLine.setTimeLine( &player );
        actualLine.stroke = lineStroke;
    }
}

bool LinesMaker::saveSecuence(string _file){
    ofxXmlSettings XML;
    
    XML.addValue("length", player.getLength());
    
    int totalLines = storedLines.size();
    for(int i = 0; i < totalLines; i++){
        XML.addTag("line");
    }
    
    for(int i = 0; i < totalLines; i++){
        XML.pushTag("line",i);
        
        XML.addValue("stroke", storedLines[i].stroke );
        XML.addValue("startTime", storedLines[i].getStartTime() );
        
        int totalPnts = storedLines[i].size();
        for(int j = 0; j < totalPnts; j++){
            XML.addTag("pnt");
        }
        
        for(int j = 0; j < totalPnts; j++){
            XML.setValue("pnt:x", storedLines[i][j].x , j);
            XML.setValue("pnt:y", storedLines[i][j].y , j);
            XML.setValue("pnt:z", storedLines[i][j].z , j);
        }
        
        XML.popTag();
    }
    
    int totalBookmarks = player.bookmarks.size();
    for(int i = 0; i < totalBookmarks; i++){
        XML.addTag("bookmark");
    }
    
    for(int i = 0; i < totalBookmarks; i++){
        XML.setValue("bookmark", player.bookmarks[i] , i);
    }
    
    XML.saveFile(_file);
}