#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxMaxim.h"
#include "maxiGrains.h"
#include "ofxOsc.h"
#include <sys/time.h>
#include "ofxGui.h"

#define HOST "localhost"
#define RECEIVEPORT 12000
#define SENDPORT 6448

#define LENGTH 294000

typedef hannWinFunctor grainPlayerWin;


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void drawWaveform(float waveformWidth, float waveformHeight, float top, float left);
    
    void audioOut(float * output, int bufferSize, int nChannels);
    void audioIn(float * input, int bufferSize, int nChannels);
    
    int		bufferSize;
    
    /* stick you maximilian declarations below
     
     For information on how maximilian works, take a look at the example code at
     
     http://www.maximilian.strangeloop.co.uk
     
     under 'Tutorials'.
     
     
     */
    
    
    int		initialBufferSize; /* buffer size */
    int		sampleRate;
    
    
    /* stick your maximilian stuff below */
    
    double wave,sample,outputs[2];
    maxiSample samp, samp2, samp3, samp4, samp5;
    vector<maxiTimePitchStretch<grainPlayerWin, maxiSample>*> stretches;
    maxiMix mymix;
    maxiTimePitchStretch<grainPlayerWin, maxiSample> *ts, *ts2, *ts3, *ts4, *ts5;
    double rate, grainLength, pitch, overlaps, position, loop_start, loop_end, volume, random_offset;
    bool playmode, loop_mode;
    
    ofxMaxiFFT fft;
    ofxMaxiFFTOctaveAnalyzer oct;
    int current;
    double pos;
    
    //osc
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    ofxMaxiFilter myFilter, myFilter2;
    
    bool isTraining;
    
    //gui
    ofxPanel gui;
    ofxFloatSlider volume_gui;
    ofxFloatSlider speed_gui;
    ofxFloatSlider grain_size_gui;
    ofxFloatSlider pitch_gui;
    ofxFloatSlider overlaps_gui;
    ofxFloatSlider position_gui;
    ofxToggle playmode_gui;
    ofxToggle loop_mode_gui;
    ofxFloatSlider loop_start_gui;
    ofxFloatSlider loop_end_gui;
    ofxIntSlider random_offset_gui;
    
    //Drawing
    int curXpos, curYpos;
    int prevXpos, prevYpos;
    
};


