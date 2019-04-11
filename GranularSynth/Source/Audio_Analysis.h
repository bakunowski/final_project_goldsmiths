#ifndef AUDIO_ANALYSIS
#define AUDIO_ANALYSIS

#pragma once

#include </usr/local/Cellar/essentia/HEAD-561764f/include/essentia/essentia.h>
#include </usr/local/Cellar/essentia/HEAD-561764f/include/essentia/algorithmfactory.h>
#include </usr/local/Cellar/essentia/HEAD-561764f/include/essentia/essentiamath.h>
#include </usr/local/Cellar/essentia/HEAD-561764f/include/essentia/pool.h>

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;
using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;

class esss
{
public:
    
    esss();
    
    vector<Real> temporaryBuffer;
    vector<Real> temporaryBuffer2;
    
    standard::Algorithm* windowing;
    vector<Real> windowedFrame;
    standard::Algorithm* spectrum;
    vector<Real> spectrumResults;
    standard::Algorithm* mfcc;
    vector<Real> mfccBands;
    vector<Real> mfccCoeffs;
    standard::Algorithm* flux;
    Real fluxOutput = 0.1;
    vector<Real> spectralFlux;
    vector<Real> threshold;
    vector<Real> prunnedSpectralFlux;
    vector<Real> peaks;

    standard::Algorithm* onsetRate;
    vector<Real> onsets;
    Real onsetRateValue;
    
    //TNT::Array2D<Real> matrix;
    //vector<Real> weights;
    //vector<Real> onsetRateVector;
    //double onsetRatee;
    //vector<Real> blablabla;
    //vector<Real> hfc;

    //standard::Algorithm* dcremoval;
    //vector<Real> dcRemovalBuffer;
    standard::Algorithm* fft;
    vector<complex<Real>> fftBuffer;
    standard::Algorithm* cartesian2polar;
    vector<Real> cartesian2polarMagnitudes;
    vector<Real> cartesian2polarPhases;

    Pool pool;
    Pool agrrPool;
    
    Pool fluxPool;
    Pool agrrFluxPool;
    
    Pool onsetPool;
    Pool agrrOnsetPool;
    
    standard::Algorithm* agrr;
    standard::Algorithm* agrr2;
    standard::Algorithm* agrr3;
    standard::Algorithm* output;
    standard::Algorithm* output2;
    standard::Algorithm* output3;
    string outputFilename = "output";
    
    
    enum TransportState
    {
        stopped,
        starting,
        playing,
        stopping
    };
    
    TransportState state;


    //static constexpr int lengthOfEssentiaBuffer = 1024; // 2 times the buffer size
    int lengthOfEssentiaBuffer = 1024;
    int lengthOfPlaybackBuffer = 44100;
    
    // buffer to hold the last 1024 samples for analysis
    struct bufferAndIndex {
        AudioSampleBuffer buffer;
        int index = 0;
        bool nextBlockReady = false;
    };
    
    bufferAndIndex preApplyEssentia;
    
    bufferAndIndex playbackBuffer;
    
    void essentiaSetup();
    
    void computeEssentiaValues();
    
    void pushNextSampleIntoEssentiaArray(float sample) noexcept;
    
    void printFluxValues();
};

#endif
