#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

// Grain

class GrainStream   :public ChangeBroadcaster
{
public:
    // constructor
    GrainStream(int startingSample_ = 1, int duration = 0);
    
    // ???
    double operator()(int channel);
    
    // sets the starting sample of a grain
    void setCentroidSample(int startingSample);
    
    // duration of a grain
    void setDuration(int duration);
    
    // audio source
    void setAudioSource(AudioFormatReader& audioReader);
    
    // set the size of grain stream
    void setStreamSize(int size);
    
    // get size of one grain in samples
    int getSize() {return gsWaveSize;};
    
    // resets grain to initial state
    void silence();
    
    // check if grain has a valid WAV file
    bool hasValidWAVFile(){return gsHasValidWAVFile;}
    
    bool gsIsPlaying = false;
    
    int gsStartingOffset = 0;
    
    int gsPitchOffset = 0;
    
    int gsGainOffsetDb = 0;
    
    double gsGlobalGain = 1.0;
    
private:
    //---------------------------------------------------------------------------
    //                                  variables
    
    // AudioSampleBuffer of the grain's audio source WAV file
    unique_ptr<AudioSampleBuffer> gsAudioSourceBuffer;
    
    // this contains data about a specific grain inside the grain stream
    struct grainData
    {
        double gdCurrentSample[2] = {0.0, 0.0};
        
        int gdStartingSample = 0;       // starting sample for a specific grain
        int gdEndSample = 0;            // ending sample of a specific grain
        
        double gdPitchScalar = 1.0f;    // scalar value for randomized pitch offset
        double gdGainScalar = 1.0f;      // scalar value for rand gain offset
        
        bool gdIsFinished = true;       // boolean whether grain needs to be replayed
    };
    
    // vector containing grains in the stream
    vector<grainData> grains;
    
    int gsStreamSize = 0;       // size of the stream (number of grains)
    
    int gsCentroidSample = 1;   // the centroid sample for a grain cloud
    int gsDuration = 0;         // duration of the stream in ms
    int gsSampleDelta = 0;      // difference between starting and ending point
    
    int gsWaveSize = 0;         // size of the audio sampled
    int gsSamplingRate;         // sampling rate of the audio sampled
    
    bool gsHasValidWAVFile;     // boolean for whether grain has a valid WAV file
    
    //----------------------------------------------------------------------------
    //                                  functions
    
    // randomize grain
    void randomizeGrain(grainData& grain);
    
    // add grains to the stream
    void addGrains(int count = 1);
    
    // remove grains from the stream
    void removeGrains(int count = 1);
};
