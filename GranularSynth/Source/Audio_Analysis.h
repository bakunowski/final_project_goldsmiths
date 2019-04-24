#ifndef AUDIO_ANALYSIS
#define AUDIO_ANALYSIS

#pragma once

#include </usr/local/Cellar/essentia/HEAD-561764f/include/essentia/essentia.h>
#include </usr/local/Cellar/essentia/HEAD-561764f/include/essentia/algorithmfactory.h>
#include </usr/local/Cellar/essentia/HEAD-561764f/include/essentia/essentiamath.h>
#include </usr/local/Cellar/essentia/HEAD-561764f/include/essentia/pool.h>

#include "../JuceLibraryCode/JuceHeader.h"

#include "../eigen-eigen-323c052e1731/Eigen/Dense"

using namespace std;
using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;

static int THRESHOLD_WINDOW_SIZE = 10;
static float MULTIPLIER = 1.4f;

class AudioFeatureExtraction
{
public:
    
    AudioFeatureExtraction();
    
    int count = 1;
    int count2 = 0;
    int duration = 1;
    int streamSize = 1;
   
    int frameSize = 2048;
    int hopSize = 1024;
    
    vector<Real> temporaryBuffer;
    vector<Real> temporaryBuffer2;
    vector<Real> microphoneBuffer;
    
    standard::Algorithm* frameCutter;
    standard::Algorithm* frameCutterInput;
    vector<Real> frame;
    vector<Real> frameInput;

    standard::Algorithm* windowing;
    standard::Algorithm* windowingInput;
    vector<Real> windowedFrame;
    vector<Real> windowedFrameInput;

    standard::Algorithm* spectrum;
    standard::Algorithm* spectrumInput;
    vector<Real> spectrumResults;
    vector<Real> spectrumResultsInput;

    standard::Algorithm* mfcc;
    standard::Algorithm* mfccInput;
    vector<Real> mfccBands;
    vector<Real> mfccCoeffs;
    vector<Real> mfccBandsInput;
    vector<Real> mfccCoeffsInput;

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
    
    Pool poolInput;
    vector<vector<float>> kerasInput;

    Pool agrrPool;
    
    Pool paramPool;

    Pool mergePool;
    Pool mergePoolParams;

    standard::Algorithm* agrr;
    standard::Algorithm* mergedMFCCs;
    standard::Algorithm* mergedParameters;
    standard::Algorithm* output;
    standard::Algorithm* output2;
    standard::Algorithm* test_output;
    
    string outputFilename = "output";

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
    void computeEssentia();
    void computeEssentiaInput();
    void clearBufferAndPool();
    
    void pushNextSampleIntoEssentiaArray(float sample) noexcept;
    
    void printFluxValues();
    
    int getLengthOfBuffer();

private:
    
    //static constexpr int lengthOfEssentiaBuffer = 1024; // 2 times the buffer size
    int lengthOfEssentiaBuffer = 1024;
    int lengthOfPlaybackBuffer = 44100;
};

#endif
