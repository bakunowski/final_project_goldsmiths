#include "Audio_Analysis.h"

static int THRESHOLD_WINDOW_SIZE = 10;
static float MULTIPLIER = 1.2f;

// essentia
esss::esss()
{
    preApplyEssentia.buffer.setSize(1, lengthOfEssentiaBuffer);
    essentia::init();
    standard::AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
    
    agrr = factory.create("PoolAggregator");
    agrr2 = factory.create("PoolAggregator");
    agrr3 = factory.create("PoolAggregator");
    output = factory.create("YamlOutput");
    output2 = factory.create("YamlOutput");
    output3 = factory.create("YamlOutput");
    
    agrr->input("input").set(pool);
    agrr->output("output").set(agrrPool);

    agrr2->input("input").set(fluxPool);
    agrr2->output("output").set(agrrFluxPool);
    
    agrr3->input("input").set(onsetPool);
    agrr3->output("output").set(agrrOnsetPool);
    
    output->input("pool").set(agrrPool);
    output2->input("pool").set(agrrFluxPool);
    output3->input("pool").set(agrrOnsetPool);

    //dcremoval = factory.create("DCRemoval");
    
    windowing = factory.create("Windowing", "size", lengthOfEssentiaBuffer, "type", "hann");
    
    fft = factory.create("FFT", "size", lengthOfEssentiaBuffer);
    cartesian2polar = factory.create("CartesianToPolar");
    
    spectrum = factory.create("Spectrum", "size", lengthOfEssentiaBuffer);
    mfcc = factory.create("MFCC", "inputSize", (lengthOfEssentiaBuffer/2)+1);

    flux = factory.create("Flux");

    //dcremoval->input("signal").set(temporaryBuffer);
    //dcremoval->output("signal").set(dcRemovalBuffer);
    
    windowing->input("frame").set(temporaryBuffer);
    windowing->output("frame").set(windowedFrame);
    
    fft->input("frame").set(windowedFrame);
    fft->output("fft").set(fftBuffer);
    
    cartesian2polar->input("complex").set(fftBuffer);
    cartesian2polar->output("magnitude").set(cartesian2polarMagnitudes);
    cartesian2polar->output("phase").set(cartesian2polarPhases);
    
    spectrum->input("frame").set(windowedFrame);
    spectrum->output("spectrum").set(spectrumResults);
    
    mfcc->input("spectrum").set(spectrumResults);
    mfcc->output("bands").set(mfccBands);
    mfcc->output("mfcc").set(mfccCoeffs);

    flux->input("spectrum").set(spectrumResults);
    flux->output("flux").set(fluxOutput);
}

void esss::pushNextSampleIntoEssentiaArray(float sample) noexcept
{
    // if stopped write to the pool and give me the output in console
    if (state == playing)
    {
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

        // update the automated extraction length (checks for peaks every second)
        playbackBuffer.index += 1;

        int spectralSize = static_cast<int>(spectralFlux.size());
        
        if (playbackBuffer.index == lengthOfPlaybackBuffer*10)
        {
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
            
            for( int i = 0; i < threshold.size(); i++ )
            {
                if (threshold[i] <= spectralFlux[i])
                    prunnedSpectralFlux.emplace_back(spectralFlux[i] - threshold[i]);
                else
                    prunnedSpectralFlux.emplace_back((float)0);
            }
            
            for( int i = 0; i < prunnedSpectralFlux.size() - 1; i++ )
            {
                if( prunnedSpectralFlux[i] > prunnedSpectralFlux[i+1] )
                {
                    peaks.emplace_back(prunnedSpectralFlux[i]);
                }
            }
            
//            cout << "new round: " << endl;
//            cout << threshold.size() << endl;
//            cout << spectralFlux.size() << endl;
//            cout << prunnedSpectralFlux.size() << endl;
//            cout << peaks.size() << endl;
//            for (int i = 0; i <= peaks.size(); ++i)
//                cout << peaks[i] << flush;
//            cout << " " << endl;
            
            // stuff for plotting of the threshold vs flux values
            
           cout << "flux :" << endl;
           for (int i = 0; i <= spectralFlux.size(); i++)
           {
               cout << i << " " << flush;
               cout << spectralFlux[i] << endl;
           }
           
           cout << "threshold: " << endl;
           for (int i = 0; i <= spectralFlux.size(); i++)
           {
               cout << i << " " << flush;
               cout << threshold[i] << endl;
           }
            
            playbackBuffer.index = 0;
            threshold.clear();
            spectralFlux.clear();
            prunnedSpectralFlux.clear();
            peaks.clear();
        }
    }
}

void esss::computeEssentiaValues()
{
    //dcremoval->compute();
    windowing->compute();
    fft->compute();
    cartesian2polar->compute();
    spectrum->compute();
    mfcc->compute();
    
    pool.add("lowlevel.mfcc", mfccCoeffs);

    flux->compute();
    spectralFlux.emplace_back(fluxOutput);
    //fluxPool.add("lowlevel.flux", fluxOutput);
}

















//    onsetDetection->compute();
//    hfc.push_back(onsetDetectionResult);
//    blablabla.push_back(onsetDetectionResult);
//
//    if (hfc.size() == 100)
//    {
//        matrix = TNT::Array2D<Real>(2, hfc.size());
//
//        for (int i=0; i <= hfc.size(); ++i)
//        {
//          matrix[0][i] = hfc[i];
//          matrix[1][i] = blablabla[i];
//        }
//
//
//        weights[0] = 1.0;
//        weights[1] = 1.0;
//
//        onsets->compute();
//        onsetPool.add("lowlevel.onsets", onsetRateVector);
//
//        hfc.clear();
//
//        //for (int i = 0; i <= onsetRateVector.size(); ++i)
//        //{
//        //    cout << onsetRateVector[i] << endl;
//        //}
//    }
