#include "Audio_Analysis.h"
#include <typeinfo>

// essentia
AudioFeatureExtraction::AudioFeatureExtraction()
{
    preApplyEssentia.buffer.setSize(1, lengthOfEssentiaBuffer);
    essentia::init();
    standard::AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
    
    const char* stats[] = {"mean"};
    agrr = factory.create("PoolAggregator", "defaultStats", arrayToVector<string>(stats));

    output = factory.create("YamlOutput");
    output2 = factory.create("YamlOutput");

    mergedMFCCs = factory.create("YamlOutput",
				 "filename",
				 "MFCC.json",
				 "format",
				 "json",
				 "writeVersion",
				 false);

    mergedParameters = factory.create("YamlOutput",
				      "filename",
				      "parameters.json",
				      "format",
				      "json",
				      "writeVersion",
				      false);
    
    agrr->input("input").set(pool);
    agrr->output("output").set(agrrPool);

    output->input("pool").set(mergePool);
    output2->input("pool").set(paramPool);

    mergedMFCCs->input("pool").set(mergePool);
    mergedParameters->input("pool").set(mergePoolParams);

    frameCutter = factory.create("FrameCutter", "frameSize", frameSize, "hopSize", hopSize);
    frameCutterInput = factory.create("FrameCutter", "frameSize", frameSize, "hopSize", hopSize);
    
    windowing = factory.create("Windowing", "type", "hann");
    windowingInput = factory.create("Windowing", "type", "hann");

    spectrum = factory.create("Spectrum");
    spectrumInput = factory.create("Spectrum");
    mfcc = factory.create("MFCC");
    mfccInput = factory.create("MFCC");
    
    flux = factory.create("Flux");

    frameCutter->input("signal").set(temporaryBuffer2);
    frameCutter->output("frame").set(frame);
    
    frameCutterInput->input("signal").set(microphoneBuffer);
    frameCutterInput->output("frame").set(frameInput);
    
    windowing->input("frame").set(frame);
    windowing->output("frame").set(windowedFrame);
    
    windowingInput->input("frame").set(frameInput);
    windowingInput->output("frame").set(windowedFrameInput);

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
}

void AudioFeatureExtraction::pushNextSampleIntoEssentiaArray(float sample) noexcept
{
    playbackBuffer.index += 1;
    temporaryBuffer2.emplace_back(sample);
}

//// look into this! -> is the mergePool args correct?
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
    }
    
    //normalize mfccs
    for (int i = 0; i < kerasInput.size(); ++i)
    {
        min = *min_element(kerasInput[i].begin(), kerasInput[i].end());
        max = *max_element(kerasInput[i].begin(), kerasInput[i].end());

        for (int j = 0; j < kerasInput[i].size(); ++j)
        {
            float norm = kerasInput[i][j] - min;
            kerasInput[i][j] = norm / (max-min);
        }
    }
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

int AudioFeatureExtraction::getLengthOfBuffer()
{
    return lengthOfPlaybackBuffer;
}
