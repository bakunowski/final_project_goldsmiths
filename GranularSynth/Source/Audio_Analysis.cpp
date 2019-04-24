#include "Audio_Analysis.h"
#include <typeinfo>

// essentia
AudioFeatureExtraction::AudioFeatureExtraction()
{
    preApplyEssentia.buffer.setSize(1, lengthOfEssentiaBuffer);
    essentia::init();
    standard::AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
    
    const char* stats[] = { "mean"};

    agrr = factory.create("PoolAggregator", "defaultStats", arrayToVector<string>(stats));

    output = factory.create("YamlOutput");
    output2 = factory.create("YamlOutput");
    test_output = factory.create("YamlOutput");
    
    mergedMFCCs = factory.create("YamlOutput", "filename", "MFCC.json", "format", "json", "writeVersion", false);
    mergedParameters = factory.create("YamlOutput", "filename", "parameters.json", "format", "json", "writeVersion", false);
    
    agrr->input("input").set(pool);
    agrr->output("output").set(agrrPool);

    output->input("pool").set(mergePool);
    output2->input("pool").set(paramPool);
    test_output->input("pool").set(pool);
    
    mergedMFCCs->input("pool").set(mergePool);
    mergedParameters->input("pool").set(mergePoolParams);
    
    //dcremoval = factory.create("DCRemoval");
    
    frameCutter = factory.create("FrameCutter", "frameSize", frameSize, "hopSize", hopSize);
    frameCutterInput = factory.create("FrameCutter", "frameSize", frameSize, "hopSize", hopSize);
    
    windowing = factory.create("Windowing", "type", "hann");
    windowingInput = factory.create("Windowing", "type", "hann");
    
//    fft = factory.create("FFT", "size", lengthOfEssentiaBuffer);
//    cartesian2polar = factory.create("CartesianToPolar");
    
    spectrum = factory.create("Spectrum");
    spectrumInput = factory.create("Spectrum");
    mfcc = factory.create("MFCC");
    mfccInput = factory.create("MFCC");
    
    flux = factory.create("Flux");

    //dcremoval->input("signal").set(temporaryBuffer);
    //dcremoval->output("signal").set(dcRemovalBuffer);
    
    frameCutter->input("signal").set(temporaryBuffer2);
    frameCutter->output("frame").set(frame);
    
    frameCutterInput->input("signal").set(microphoneBuffer);
    frameCutterInput->output("frame").set(frameInput);
    
    windowing->input("frame").set(frame);
    windowing->output("frame").set(windowedFrame);
    
    windowingInput->input("frame").set(frameInput);
    windowingInput->output("frame").set(windowedFrameInput);
    
    //fft->input("frame").set(windowedFrame);
    //fft->output("fft").set(fftBuffer);
    
    //cartesian2polar->input("complex").set(fftBuffer);
    //cartesian2polar->output("magnitude").set(cartesian2polarMagnitudes);
    //cartesian2polar->output("phase").set(cartesian2polarPhases);
    
    spectrum->input("frame").set(windowedFrame);
    spectrum->output("spectrum").set(spectrumResults);
    
    spectrumInput->input("frame").set(windowedFrameInput);
    spectrumInput->output("spectrum").set(spectrumResultsInput);
    
    mfcc->input("spectrum").set(spectrumResults);
    mfcc->output("bands").set(mfccBands);
    mfcc->output("mfcc").set(mfccCoeffs);
    
    mfccInput->input("spectrum").set(spectrumResultsInput);
    mfccInput->output("bands").set(mfccBandsInput);
    mfccInput->output("mfcc").set(mfccCoeffsInput);

    flux->input("spectrum").set(spectrumResults);
    flux->output("flux").set(fluxOutput);

    //onsetRate->input("signal").set(temporaryBuffer2);
    //onsetRate->output("onsets").set(onsets);
    //onsetRate->output("onsetRate").set(onsetRateValue);
}

void AudioFeatureExtraction::pushNextSampleIntoEssentiaArray(float sample) noexcept
{
        // if stopped write to the pool and give me the output in console
         playbackBuffer.index += 1;
         temporaryBuffer2.emplace_back(sample);
}

void AudioFeatureExtraction::computeEssentia()
{
    while (true)
    {
      frameCutter->compute();

      if (!frame.size())
      {
          break;
      }
      if (isSilent(frame)) continue;
        
      windowing->compute();
      spectrum->compute();
      mfcc->compute();
      
      pool.add("lowlevel.mfcc", mfccCoeffs);
      //cout << "mfcc13 " << mfccCoeffs.size() << endl;
    }
    
    //agrr->compute();
    
    //vector<Real> matrix = pool.value<vector<Real>>("lowlevel.mfcc");
    //cout << matrix.size() << endl;
    
    mergePool.merge(pool, "append");
    //mergedMFCCs->compute();

    //mergePoolParams.merge(paramPool, "append");
    //mergedParameters->compute();
    
    //output->compute();
    //output2->compute();
}

void AudioFeatureExtraction::computeEssentiaInput()
{
    while (true)
    {
        frameCutterInput->compute();

        if (!frameInput.size())
        {
            break;
        }
        if (isSilent(frameInput)) continue;
        
        windowingInput->compute();
        spectrumInput->compute();
        mfccInput->compute();

        poolInput.add("lowlevel.mfcc", mfccCoeffsInput);
        kerasInput.emplace_back(mfccCoeffsInput);
        //cout << kerasInput.size() << endl;
        // maybe add to a vector of vectors instead and use that as input to ML
    }
    //mergePool.merge(pool, "append");
}
void AudioFeatureExtraction::clearBufferAndPool()
{
    playbackBuffer.index = 0;
    temporaryBuffer2.clear();
    
    frameCutter->reset();
    windowing->reset();
    spectrum->reset();
    mfcc->reset();
    
    pool.clear();
    paramPool.clear();
    agrrPool.clear();
}

void AudioFeatureExtraction::printFluxValues()
{
    int spectralSize = static_cast<int>(spectralFlux.size());
    
    for (int i = 0; i < spectralFlux.size(); i++)
    {
        int start = std::max(0, i - THRESHOLD_WINDOW_SIZE);
        int end = std::min(spectralSize - 1, i + THRESHOLD_WINDOW_SIZE);
        float mean = 0;
        for (int j = start; j <= end; j++)
        {
            mean += spectralFlux[j];
        }
        mean /= (end - start);
        threshold.emplace_back(mean * MULTIPLIER);
    }
    
    // if spectral value is bigger than threshold, put however much bigger into new array
    // otherwise put 0 in
    for( int i = 0; i < threshold.size(); i++ )
    {
        if (threshold[i] <= spectralFlux[i])
            prunnedSpectralFlux.emplace_back(spectralFlux[i] - threshold[i]);
        else
            prunnedSpectralFlux.emplace_back((float)0);
    }
    
    // if a value if smaller than next one -> end of peak
    for( int i = 0; i < prunnedSpectralFlux.size() - 1; i++ )
    {
        if(prunnedSpectralFlux[i] < prunnedSpectralFlux[i+1])
        {
            prunnedSpectralFlux[i] = 0;
            continue;
        }
        
        // remove instances to close to each other
        if (prunnedSpectralFlux[i] > 0.0f)
        {
            for (int j = i + 1; j < i + 2.3f; j++)
            {
                if (prunnedSpectralFlux[j] > 0)
                {
                    prunnedSpectralFlux[j] = 0.0f;
                }
            }
        }
    }
    for (int i = 0; i < prunnedSpectralFlux.size() - 1; i++)
    {
        if (prunnedSpectralFlux[i] > 0)
        {
            peaks.emplace_back(prunnedSpectralFlux[i]);
        }
    }

//    cout << "new round: " << endl;
//    cout << threshold.size() << endl;
//    cout << spectralFlux.size() << endl;
//    cout << prunnedSpectralFlux.size() << endl;
//    cout << "peaks: " << peaks.size() << endl;

    // stuff for plotting of the threshold vs flux values
    cout << "flux :" << endl;
    for (int i = 0; i <= spectralFlux.size(); i++)
    {
        cout << i << "   " << flush;
        cout << spectralFlux[i] << endl;
    }
    
    cout << "threshold: " << endl;
    for (int i = 0; i <= threshold.size(); i++)
    {
        cout << i << "   " << flush;
        cout << threshold[i] << endl;
    }
    
    cout << "prunned: " << endl;
    for (int i = 0; i <= prunnedSpectralFlux.size(); i++)
    {
        cout << i << "   " << flush;
        cout << prunnedSpectralFlux[i] << endl;
    }
    
    //onsetRate->compute();
    //cout << "onset rate: " << onsetRateValue << endl;
    //cout << "peaks: " << peaks.size() << endl;
    
    temporaryBuffer2.clear();
    playbackBuffer.index = 0;

    threshold.clear();
    spectralFlux.clear();
    prunnedSpectralFlux.clear();
    peaks.clear();
    //onsets.clear();
    //onsetRateValue = 0;
}

int AudioFeatureExtraction::getLengthOfBuffer()
{
    return lengthOfPlaybackBuffer;
}
