#ifndef AUDIO_ANALYSIS
#define AUDIO_ANALYSIS

#pragma once

#include <essentia/essentia.h>
#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h>
#include <essentia/streaming/algorithms/poolstorage.h>

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
    vector<Real> dcRemovalBuffer;
    vector<Real> windowingBuffer;
    vector<complex<Real>> fftBuffer;
    vector<Real> cartesian2polarMagnitudes;
    vector<Real> cartesian2polarPhases;
    Real onsetDetectionResult;
    Real result;
    vector<Real> result2;
    // rename the name "matrix" soon !!!!!!!
    //    vector<vector<essentia::Real>> matrix;
    TNT::Array2D<Real> matrix;
    vector<Real> hfc;
    vector<Real> weights;
    double onsetRatee;
    vector<Real> blablabla;
    vector<Real> onsetRateVector;
    
    vector<Real> spectrumResults;
    vector<Real> mfccBands;
    vector<Real> mfccCoeffs;
    
    standard::Algorithm* dcremoval;
    standard::Algorithm* windowing;
    standard::Algorithm* fft;
    standard::Algorithm* cartesian2polar;
    standard::Algorithm* spectrum;
    standard::Algorithm* mfcc;
    standard::Algorithm* onsetDetection;
    standard::Algorithm* onsets;
    standard::Algorithm* onsetRate;
    

    Pool pool;
    
    static constexpr int lengthOfEssentiaBuffer = 8192; // 6 times the buffer size
    
    // buffer to hold the last 8192 samples for analysis
    struct bufferAndIndex {
        AudioSampleBuffer buffer;
        int index = 0;
        bool nextBlockReady = false;
    };
    
    bufferAndIndex preApplyEssentia;
    
    void essentiaSetup();
    
    void computeEssentiaValues();
    
    void pushNextSampleIntoEssentiaArray(float sample, bool playing) noexcept;
};

#endif
