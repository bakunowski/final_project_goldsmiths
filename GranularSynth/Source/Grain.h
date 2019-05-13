#ifndef GRAIN
#define GRAIN

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class GrainStream                :public ChangeBroadcaster
{
public:
    
    GrainStream(int startingSample = 1, int duration = 0);
    
    // ???
    //double operator()(int channel);
    
    double createGrain(int channel);
    
    void setFilePosition(int startingSample);
    
    // duration of a grain
    void setDuration(int duration);
    
    void setAudioSource(AudioFormatReader& audioReader);
    
    void setStreamSize(int size);
    
    int getFileSize() {return this->fileSize;};
    
    // resets grain to initial state
    void silenceAllGrains();
    
    // check if grain has a valid WAV file
    bool hasValidWAVFile(){return this->grainHasValidWAVFile;}
    
    bool grainStreamIsActive = false;
    
    int filePositionOffset = 0;
    
    int pitchOffsetForOneGrain = -12;
    
    int gainOffsetForOneGrain = 0;
    
    double globalGain = 1.0;
    
    vector<double> getCurrentGrainPosition(int channel);
    
    void changePitch();
    

    
private:

    // AudioSampleBuffer of the grain's audio source WAV file
    unique_ptr<AudioSampleBuffer> AudioSourceBuffer;
    
    // this contains data about a specific grain inside the grain stream
    struct oneGrain
    {
        double grainDataCurrentSample[2] = {0.0, 0.0};
        
        int grainDataStartPosition = 0;       // starting sample for a specific grain
        int grainDataEndPosition = 0;            // ending sample of a specific grain
        
        double grainDataPitchScalar = 1.0f;    // scalar value for randomized pitch offset
        double grainDataGainScalar = 1.0f;      // scalar value for rand gain offset
        
        bool grainDataIsFinished = true;       // boolean whether grain needs to be replayed
        
        juce::ADSR adsr;
        juce::ADSR::Parameters adsrParams;
        
        bool inRelease = true;
    };
    
    // vector containing grains in the stream
    vector<oneGrain> grains;
    
    int grainStreamSize = 0;        // size of the stream (number of grains)
    
    int filePosition = 1;           // the centroid sample for a grain cloud
    int durationOfStream = 0;       // duration of the stream in ms
    int sampleDelta = 0;            // difference between starting and ending point
    
    int fileSize = 0;               // size of the audio sampled
    int samplingRate;               // sampling rate of the audio sampled
    
    bool grainHasValidWAVFile;      // boolean for whether grain has a valid WAV file

    // randomize grain data
    void randomizeGrain(oneGrain& grain);
    
    void addGrainsToStream(int count = 1);
    
    void removeGrainsFromStream(int count = 1);
    
    void getCurrentPosition(oneGrain& grain);
};

#endif
