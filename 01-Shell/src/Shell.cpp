//
//  Shell.cpp
//
//  Created by Patricio Gonzalez Vivo on 11/6/13.
//
//

#include "Shell.h"


void Shell::selfSetup(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();

    bufferSize = 512;
	
    magnitude   = new float[bufferSize];
    phase       = new float[bufferSize];
    power       = new float[bufferSize];
    freq        = new float[bufferSize/2];
    
    audioIn.setup(44100, bufferSize);
	FFTanalyzer.setup(44100, bufferSize/2, 2);
	FFTanalyzer.peakHoldTime        = 15;       // hold longer
	FFTanalyzer.peakDecayRate       = 0.95f;    // decay slower
	FFTanalyzer.linearEQIntercept   = 0.9f;     // reduced gain at lowest frequency
	FFTanalyzer.linearEQSlope       = 0.01f;    // increasing gain at higher frequencies
}

void Shell::selfSetupGuis(){
    lightAdd("DIR LIGHT", OF_LIGHT_DIRECTIONAL );
    lightAdd("SPOT LIGHT", OF_LIGHT_SPOT);
    backgroundSet(new UIMapBackground() );
    
    guiAdd(grid);
    guiAdd(shell);
    guiAdd(audioIn);
}

void Shell::selfUpdate(){
    
    //  Do the FFT
    //
    float avg_power = 0.0f;
	myFft.powerSpectrum(0,(int)bufferSize/2, audioIn.left, bufferSize, magnitude, phase, power, &avg_power);
	for (int i = 0; i < (int)(bufferSize/2); i++){
		freq[i] = magnitude[i];
	}
    FFTanalyzer.calculate(freq);
    
    vector<float> newAverage;
    newAverage.assign(FFTanalyzer.averages,FFTanalyzer.averages+18);
    for(int i = 0; i < newAverage.size(); i++){
        newAverage[i] /= ofNoise(ofGetElapsedTimef()*0.1,(float)i/((float)newAverage.size()))*100;
    }
    newAverage.push_back(ofNoise(ofGetElapsedTimef()*0.1));
    newAverage.push_back(ofNoise(ofGetElapsedTimef()*0.1));
    
    shell.feed(newAverage);
    shell.update();
}

void Shell::selfDraw(){
    materials["MATERIAL 1"]->begin();
    
    ofPushStyle();
    grid.draw();
    ofPopStyle();
    
    ofSetColor(255);
    shell.draw();
    
    materials["MATERIAL 1"]->end();
}

void Shell::selfKeyPressed(ofKeyEventArgs & args){

}

void Shell::selfKeyReleased(ofKeyEventArgs & args){
	
}

void Shell::selfMouseDragged(ofMouseEventArgs& data){

}

void Shell::selfMouseMoved(ofMouseEventArgs& data){
	
}

void Shell::selfMousePressed(ofMouseEventArgs& data){

}

void Shell::selfMouseReleased(ofMouseEventArgs& data){
	
}