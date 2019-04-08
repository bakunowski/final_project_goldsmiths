#include "Audio_Analysis.h"

// essentia
esss::esss()
{
    preApplyEssentia.buffer.setSize(1, esss::lengthOfEssentiaBuffer);
    init();
    standard::AlgorithmFactory& factory = standard::AlgorithmFactory::instance();


    dcremoval = factory.create("DCRemoval");
    windowing = factory.create("Windowing", "type", "hann", "zeroPadding", 0);
    fft = factory.create("FFT", "size", 1024);
    cartesian2polar = factory.create("CartesianToPolar");
    spectrum = factory.create("Spectrum", "size", 8192);
    mfcc = factory.create("MFCC");
    onsetDetection = factory.create("OnsetDetection");
    onsets = factory.create("Onsets");
    //onsetRate = factory.create("OnsetRate");

    temporaryBuffer.reserve(lengthOfEssentiaBuffer);
    temporaryBuffer.resize(lengthOfEssentiaBuffer, 0.0f);
    
    spectrumResults.reserve(lengthOfEssentiaBuffer);
    spectrumResults.resize(lengthOfEssentiaBuffer, 0.0f);
    
    weights.reserve(2);
    weights.resize(2, 0.0f);
    cout << weights.size() << endl;
    
    dcremoval->input("signal").set(temporaryBuffer);
    dcremoval->output("signal").set(dcRemovalBuffer);
    
    windowing->input("frame").set(dcRemovalBuffer);
    windowing->output("frame").set(windowingBuffer);
    
    fft->input("frame").set(windowingBuffer);
    fft->output("fft").set(fftBuffer);
    
    cartesian2polar->input("complex").set(fftBuffer);
    cartesian2polar->output("magnitude").set(cartesian2polarMagnitudes);
    cartesian2polar->output("phase").set(cartesian2polarPhases);
    
    spectrum->input("frame").set(temporaryBuffer);
    spectrum->output("spectrum").set(spectrumResults);
    
    mfcc->input("spectrum").set(spectrumResults);
    mfcc->output("bands").set(mfccBands);
    mfcc->output("mfcc").set(mfccCoeffs);
    
    onsetDetection->input("spectrum").set(cartesian2polarMagnitudes);
    onsetDetection->input("phase").set(cartesian2polarPhases);
    
    // push this into an array, the size of which will serve as 2nd arg in matrix init
    onsetDetection->output("onsetDetection").set(onsetDetectionResult);
    
    // matrix 2 dimensional
    onsets->input("detections").set(matrix);
    onsets->input("weights").set(weights);
    onsets->output("onsets").set(onsetRateVector);
    
    //    onsetRate->input("signal").set(onsetRateVector);
    //    onsetRate->output("onsetRate").set(result);
    //    onsetRate->output("onsets").set(result2);
}

void esss::pushNextSampleIntoEssentiaArray(float sample, bool playing) noexcept
{
    // this won't wowrk!
    if (playing == 1){
        // if we have enough data set a flag to indicate that next line should now be rendered?
        if (preApplyEssentia.index == lengthOfEssentiaBuffer)
        {
            if (! preApplyEssentia.nextBlockReady)
            {
                computeEssentiaValues();
                temporaryBuffer.clear();
                preApplyEssentia.nextBlockReady = true;
            }
            preApplyEssentia.index = 0;
            preApplyEssentia.nextBlockReady = false;
        }
        // update index if not enough data and make that index equal to the sample
        preApplyEssentia.index += 1;
        temporaryBuffer.emplace_back(sample);
    }
}

void esss::computeEssentiaValues()
{
    dcremoval->compute();
    windowing->compute();
    fft->compute();
    cartesian2polar->compute();
    
    spectrum->compute();
    mfcc->compute();

    // for (int i = 0; i <= mfccCoeffs.size(); ++i)
    // {
    //     cout << mfccCoeffs[i] << endl;
    // }

    onsetDetection->compute();
    hfc.push_back(onsetDetectionResult);
    blablabla.push_back(onsetDetectionResult);

    if (hfc.size() == 100)
    {
        matrix = TNT::Array2D<Real>(2, hfc.size());
        
        for (int i=0; i<int(hfc.size()); ++i)
        {
            matrix[0][i] = hfc[i];
            matrix[1][i] = blablabla[i];
        }
        
        hfc.clear();
        weights[0] = 1.0;
        weights[1] = 1.0;
        
        onsets->compute();
        
        for (int i = 0; i <= onsetRateVector.size(); ++i)
        {
            cout << onsetRateVector[i] << endl;
        }
    }
}
