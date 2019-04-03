#include "Grain.h"
#define LEFT_CHANNEL 0
#define RIGHT_CHANNEL 1

// constructor
GrainStream::GrainStream(int startingSample_, int duration_)
{
    gsSamplingRate = 0;
    setDuration(duration_);
    setCentroidSample(startingSample_);
    // add 1 grain at the beggining
    addGrains();
}

double GrainStream::operator()(int channel)
{
    float sample = 0;
    
    for (grainData& grain : grains)
    {
        // check if getting the current grain is done
        if (gsAudioSourceBuffer->getNumChannels() >= 2)
        {
            if ((grain.gdCurrentSample[LEFT_CHANNEL] >= grain.gdEndSample) &&
                (grain.gdCurrentSample[RIGHT_CHANNEL] >= grain.gdEndSample))
                grain.gdIsFinished = true;
        }
        else if((grain.gdCurrentSample[LEFT_CHANNEL] >= grain.gdEndSample))
            grain.gdIsFinished = true;
        
        // restart the grain if we are using it after finishing
        if (grain.gdIsFinished)
        {
            grain.gdIsFinished = false;
            
            // randomize the grain
            randomizeGrain(grain);
        }
        
        //get the current sample form the audio buffer
        float currentSample = (gsAudioSourceBuffer->getSample(channel, static_cast<int>(grain.gdCurrentSample[channel])));
        sample += (currentSample * static_cast<float>(grain.gdGainScalar));
        
        grain.gdCurrentSample[channel] += grain.gdPitchScalar;
        
        if (grain.gdCurrentSample[channel] >= static_cast<double>(gsWaveSize))
            grain.gdCurrentSample[channel] = (static_cast<double>(gsWaveSize) - 1.0f);
    }
    
    // scale the sample by gain
    sample *= static_cast<float>(gsGlobalGain);
    
    // if polyphonic, scale by 0.3f
    if (grains.size() > 1)
        sample *= 0.3f;
    
    //clip check
    if (sample > 1.0f)
        return 1.0f;
    else if (sample < -1.0f)
        return -1.0f;
    
    return sample;
}

void GrainStream::setCentroidSample(int startingSample)
{
    // update the starting sample and reset current sample
    gsCentroidSample = startingSample - 1;
    
    
    for (grainData& grain : grains)
    {
        randomizeGrain(grain);
    }
    
}

void GrainStream::setDuration(int duration)
{
    float dur = static_cast<float>(gsDuration);
    
    // update duration and compute gsSampleDelta
    gsDuration = duration;
    gsSampleDelta = static_cast<int>(gsSamplingRate * (dur/1000.0f));
    
    for (grainData& grain : grains)
    {
        // clamp the end sample
        grain.gdEndSample = grain.gdStartingSample + gsSampleDelta;
        
        if (grain.gdEndSample >= gsWaveSize)
            grain.gdEndSample = (gsWaveSize - 1);
    }
}

void GrainStream::setAudioSource(AudioFormatReader& newAudioFile)
{
    int length = static_cast<int>(newAudioFile.lengthInSamples);
    
    // update grain parameters
    gsWaveSize = length;
    gsSamplingRate = static_cast<int>(newAudioFile.sampleRate);
    
    // clear the audio source and read new WAV
    gsAudioSourceBuffer.reset(new AudioSampleBuffer(newAudioFile.numChannels, length));
    newAudioFile.read(gsAudioSourceBuffer.get(), 0, length, 0, true, true);
}

void GrainStream::setStreamSize(int size)
{
    if (size > gsStreamSize)
        addGrains(size - gsStreamSize);
    else if (size < gsStreamSize)
        removeGrains(gsStreamSize - size);
}

void GrainStream::randomizeGrain(grainData& grain)
{
    Random rand = Random();
    
    // Ensure the Starting Sample is non-negative
    if (gsStartingOffset == 0 || (gsCentroidSample - gsStartingOffset) <= 0)
        grain.gdStartingSample = gsCentroidSample;
    else
    {
        
        // Randomize the Starting Sample
        grain.gdStartingSample = rand.nextInt(Range<int>(gsCentroidSample - gsStartingOffset,
                                                        gsCentroidSample + gsStartingOffset));
        
        // Clamp the Starting Sample to be Within the WaveTable Range
        if (grain.gdStartingSample < 0)
            grain.gdStartingSample = 0;
        else if (grain.gdStartingSample >= gsWaveSize)
            grain.gdStartingSample = (gsWaveSize - 1);
    }
    
    // Set the Current Sample to the Starting Sample
    grain.gdCurrentSample[LEFT_CHANNEL] = static_cast<double>(grain.gdStartingSample);
    grain.gdCurrentSample[RIGHT_CHANNEL] = static_cast<double>(grain.gdStartingSample);
    
    // Clamp the End Sample to be Within the WaveTable Range
    grain.gdEndSample = grain.gdStartingSample + gsSampleDelta;
    if (grain.gdEndSample >= gsWaveSize)
        grain.gdEndSample = (gsWaveSize - 1);
    
    // Randomize the Pitch
    int randomPitch = 0;
    if(gsPitchOffset > 0)
        randomPitch = rand.nextInt(Range<int>(-gsPitchOffset, gsPitchOffset));
    grain.gdPitchScalar = std::pow(2.0f, static_cast<double>(randomPitch) / 12.0f);
    
    // Randomize the Grain Gain
    double randomGain = 0.0;
    if(gsGainOffsetDb < 0)
        randomGain = static_cast<double>(rand.nextInt(Range<int>(gsGainOffsetDb, 0)));
    grain.gdGainScalar = Decibels::decibelsToGain<double>(randomGain);
    
}

void GrainStream::addGrains(int count)
{
    for (int i = 0; i < count; i++)
    {
        grains.push_back(grainData());
        randomizeGrain(grains.back());
    }
    
    gsStreamSize += count;
}

void GrainStream::removeGrains(int count)
{
    for (int i = 0; i < count; i++)
        grains.pop_back();
    
    gsStreamSize -= count;
}

void GrainStream::silence()
{
    for (grainData& grain : grains)
        grain.gdIsFinished = true;
    
    gsIsPlaying = false;
}
