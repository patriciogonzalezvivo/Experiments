//
//  Shell.h
//
//  Created by Patricio Gonzalez Vivo on 9/6/13.
//
//

#pragma once

#include "UI3DGrid.h"
#include "ofxPro.h"

#include "UIShell.h"
#include "UIBufferIn.h"

#include "fft.h"
#include "FFTOctaveAnalyzer.h"

class Shell : public ofx3DPro {
public:

    string getSystemName(){return "Shell";}

    void selfSetupGuis();

    void selfSetup();
    void selfUpdate();
    void selfDraw();

    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);

    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);

    void audioReceived 	(float * input, int bufferSize, int nChannels);
    
protected:
    UI3DGrid grid;
    UIShell shell;
    UIBufferIn audioIn;
    
    fft                 myFft;
    FFTOctaveAnalyzer   FFTanalyzer;
    int                 bufferSize;
    int                 large;
    
    float               *magnitude;
    float               *phase;
    float               *power;
    float               *freq;
};
