/* This is an example of how to integrate maximilain into openFrameworks,
 including using audio received for input and audio requested for output.
 
 
 You can copy and paste this and use it as a starting example.
 
 */

#include "ofApp.h"
//#include "maximilian.h"/* include the lib */
#include "time.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    //gui
    gui.setup();
    gui.add(volume_gui.setup("volume", 0.1, 0, 1));
    gui.add(speed_gui.setup("speed", 1, -1, 1));
    gui.add(grain_size_gui.setup("grain_size", 0.1, 0.001, 0.4));
    gui.add(pitch_gui.setup("pitch", 1, 0., 2.));
    gui.add(overlaps_gui.setup("overlaps?", 5, 0, 10.));
    gui.add(position_gui.setup("position", 0, 0, 1));
    gui.add(playmode_gui.setup("playmode", true));
    gui.add(random_offset_gui.setup("random", 10, 1, 10000));

    /* This is stuff you always need.*/
    
    //samples from http://freesound.org
    samp.load(ofToDataPath("346755__sergiosmarinis__keys-on-table.wav"));
    samp2.load(ofToDataPath("249592__staticpony1__dirty-electro-bass.wav"));
    //samp3.load(ofToDataPath("26393__brfindla__Calango1berimbau.wav"));
    samp4.load(ofToDataPath("68373__juskiddink__Cello_open_string_bowed.wav"));
    //samp5.load(ofToDataPath("249592__staticpony1__dirty-electro-bass.wav"));
    
    sampleRate 	= 44100;    //Sampling Rate
    bufferSize	= 512;      //Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method
    
    ts = new maxiTimePitchStretch<grainPlayerWin, maxiSample>(&samp);
    ts2 = new maxiTimePitchStretch<grainPlayerWin, maxiSample>(&samp2);
    //ts3 = new maxiTimePitchStretch<grainPlayerWin, maxiSample>(&samp3);
    ts4 = new maxiTimePitchStretch<grainPlayerWin, maxiSample>(&samp4);
    //ts5 = new maxiTimePitchStretch<grainPlayerWin, maxiSample>(&samp5);
    stretches.push_back(ts);
    stretches.push_back(ts2);
    //stretches.push_back(ts3);
    stretches.push_back(ts4);
    //stretches.push_back(ts5);
    rate = 1;
    grainLength = 0.05;
    current=0;
    pitch = 1.;
    out_channels = 2;
    
    fft.setup(1024, 512, 256);
    oct.setup(44100, 1024, 10);
    
    int current = 0;
    ofxMaxiSettings::setup(sampleRate, out_channels, bufferSize);

    ofEnableSmoothing();
    
    //Anything that you would normally find/put in maximilian's setup() method needs to go here. For example, Sample loading.
    
    ofBackground(211,211,211);
    ofSetFrameRate(60);
    
    ofSoundStreamSettings settings;
    settings.numOutputChannels = out_channels;
    settings.sampleRate = sampleRate;
    settings.bufferSize = bufferSize;
    settings.numBuffers = 4;
    settings.setOutListener(this);
    soundStream.setup(settings);
    
    audioAnalyzer.setup(sampleRate, bufferSize, out_channels);
    
    k = 0;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    /* You can use any of the data from audio received and audiorequested to draw stuff here.
     Importantly, most people just use the input and output arrays defined above.
     Clever people don't do this. This bit of code shows that by default, each signal is going to flip
     between -1 and 1. You need to account for this somehow. Get the absolute value for example.
     */
    
    //ADD SOURCE
    drawWaveform();

    //gui
    gui.draw();
    volume = volume_gui;
    rate = speed_gui;
    grainLength = grain_size_gui;
    pitch = pitch_gui;
    overlaps = overlaps_gui;
    position = position_gui;
    playmode = playmode_gui;
    random_offset = random_offset_gui;
    
    //AudioAnalyzer ADD SOURCE
    ofPushMatrix();
    ofTranslate(700, 0);
    int mw = 250;
    
    int graphH = 75;
    int yoffset = graphH + 50;
    int ypos = -100;
    int xpos = ofGetWidth()-300;
    
    ypos += yoffset;
    ofSetColor(255);
    ofDrawBitmapString("MFCC: ", xpos, ypos);
    ofPushMatrix();
    ofTranslate(xpos, ypos);
    ofSetColor(141,82,82);
    float bin_w = (float) mw / mfcc.size();
    for (int i = 0; i < mfcc.size(); i++){
        float scaledValue = ofMap(mfcc[i], 0, MFCC_MAX_ESTIMATED_VALUE, 0.0, 1.0, true);//clamped value
        float bin_h = -1 * (scaledValue * graphH);
        ofDrawRectangle(i*bin_w, graphH, bin_w, bin_h);
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    
    
    for (size_t i = 0; i < outBuffer.getNumFrames(); i++){
        
        /* Stick your maximilian 'play()' code in here ! Declare your objects in testApp.h.
         
         For information on how maximilian works, take a look at the example code at
         
         http://www.maximilian.strangeloop.co.uk
         
         under 'Tutorials'.
         
         */
        if(playmode == false){
        stretches[current]->setPosition(position);
            speed_gui = 0;
        //stretches[current]->randomOffset = ofRandomf() * random_offset_gui;
        }

        wave = stretches[current]->play(pitch, rate, grainLength, overlaps, 0);
        stretches[current]->randomOffset = random_offset_gui;
        
        if (fft.process(wave)) {
            oct.calculate(fft.magnitudes);
        }
        
        //play result
        mymix.stereo(wave, outputs, 0.5);
        outBuffer.getSample(i, 0) = outputs[0] * volume;
        outBuffer.getSample(i, 1) = outputs[1] * volume;
        
        /* You may end up with lots of outputs. add them here */
        }
    
    //AudioAnalyzer
    audioAnalyzer.analyze(outBuffer);
    mfcc = audioAnalyzer.getValues(MFCC, 0, 0);
    
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    // samples are "interleaved"
    for(int i = 0; i < bufferSize; i++){
    //you can also grab the data out of the arrays
        
    }
     
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    //JSON
    //Json::Value event;
    k++;
        for(int i = 0; i < mfcc.size(); i++){
            result["data"][k]["mfcc"][i] = mfcc[i];
            result["data"][k]["parameters"]["pitch"] = pitch;
            result["data"][k]["parameters"]["speed"] = rate;
            result["data"][k]["parameters"]["grainLength"] = grainLength;
            result["data"][k]["parameters"]["overlaps"] = overlaps;
            result["data"][k]["parameters"]["position"] = position;
            result["data"][k]["parameters"]["playmode"] = playmode;
        }
    
    
    result.save("data.json", true);

    //cout << result << endl;
    cout << k << endl;
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (++current > stretches.size()-1) current = 0;
}


//--------------------------------------------------------------
void ofApp::drawWaveform(){
    //ADD SOURCE
    ofSetColor(255);
    ofFill();
    
    const float waveformWidth  = ofGetWidth() - 40;
    const float waveformHeight = 150;
    
    float top = ofGetHeight() - waveformHeight - 50;
    float left = 20;
    
    float point = (stretches[current]->sample->length)/4000;
    
    // draw the audio waveform
    for(int i= 0; i < stretches[current]->sample->length; i+=point){
        curXpos = ofMap(i, 0, stretches[current]->sample->length, left, waveformWidth+20);
        curYpos = ofMap(stretches[current]->sample->temp[i], -32768, 32768, top, waveformHeight + top);
        ofSetColor(0,0,0);
        ofDrawEllipse(curXpos, curYpos, 2, 2);
        ofDrawLine(curXpos, curYpos, prevXpos, prevYpos);
      if(i < (stretches[current]->sample->length)-point){
            prevXpos = curXpos;
            prevYpos = curYpos;
        } else {
            prevXpos = left;
            prevYpos = waveformHeight+top;
        }
    }
    
    // draw a playhead over the waveform
    ofSetColor(ofColor::white);
    
    ofDrawLine(
     stretches[current]->getNormalisedPosition() * waveformWidth,
     top,
     stretches[current]->getNormalisedPosition() * waveformWidth,
     top + waveformHeight);
    
    
    ofDrawBitmapString("PlayHead", left + stretches[current]->getNormalisedPosition() * waveformWidth-69, top+30);
    
    // draw a frame around the whole thing
    ofSetColor(ofColor::white);
    ofNoFill();
    ofDrawRectangle(left, top, waveformWidth, waveformHeight);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
