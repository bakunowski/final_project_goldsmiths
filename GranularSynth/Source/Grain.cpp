#include "Grain.h"

#define LEFT_CHANNEL 0
#define RIGHT_CHANNEL 1

GrainStream::GrainStream(int startingSample, int duration)
{
    this->samplingRate = 0;
    setDuration(duration);
    setFilePosition(startingSample);
    // add 1 grain at the beggining
    addGrainsToStream();
}

double GrainStream::operator()(int channel)
{
    float sample = 0;
    
    for (oneGrain& grain : grains)
    {
        // check if getting the current grain is done
        if (this->AudioSourceBuffer->getNumChannels() >= 2)
        {
            if ((grain.grainDataCurrentSample[LEFT_CHANNEL] >= grain.grainDataEndPosition) &&
                (grain.grainDataCurrentSample[RIGHT_CHANNEL] >= grain.grainDataEndPosition))
                grain.grainDataIsFinished = true;
        }
        else if((grain.grainDataCurrentSample[LEFT_CHANNEL] >= grain.grainDataEndPosition))
            grain.grainDataIsFinished = true;
        
        // restart the grain if we are using it after finishing
        if (grain.grainDataIsFinished)
        {
            grain.grainDataIsFinished = false;
            
            // randomize the grain
            // file position and grain size do not work when this is off
            randomizeGrain(grain);
        }
        
        //get the current sample form the audio buffer
        float currentSample = (this->AudioSourceBuffer->getSample(channel, static_cast<int>(grain.grainDataCurrentSample[channel])));
        sample += (currentSample * static_cast<float>(grain.grainDataGainScalar));
        
        grain.grainDataCurrentSample[channel] += grain.grainDataPitchScalar;
        
        if (grain.grainDataCurrentSample[channel] >= static_cast<double>(this->fileSize))
            grain.grainDataCurrentSample[channel] = (static_cast<double>(this->fileSize) - 1.0f);
    }
    
    // scale the sample by gain
    sample *= static_cast<float>(globalGain);
    
    // when more than one grain is playing, scale gain appropriately
    if (grains.size() > 1)
        sample *= 1.0f/(static_cast<float>(grains.size()));
    
    //clip check
    if (sample > 1.0f)
        return 1.0f;
    else if (sample < -1.0f)
        return -1.0f;
    
    return sample;
}

void GrainStream::setFilePosition(int startingSample)
{
    // update the starting sample and reset current sample
    this->filePosition = startingSample - 1;
    
// this is essentially the spread functionality
//    for (grainData& grain : grains)
//    {
//        randomizeGrain(grain);
//    }
    
}



void GrainStream::setDuration(int duration)
{
    float dur = static_cast<float>(this->durationOfStream);
    
    // update duration and compute gsSampleDelta
    this->durationOfStream = duration;
    this->sampleDelta = static_cast<int>(this->samplingRate * (dur/1000.0f));
    
    for (oneGrain& grain : grains)
    {
        // clamp the end sample
        grain.grainDataEndPosition = grain.grainDataStartPosition + this->sampleDelta;
        
        if (grain.grainDataEndPosition >= this->fileSize)
            grain.grainDataEndPosition = (this->fileSize - 1);
    }
}

void GrainStream::setAudioSource(AudioFormatReader& newAudioFile)
{
    int length = static_cast<int>(newAudioFile.lengthInSamples);
    
    // update grain parameters
    this->fileSize = length;
    this->samplingRate = static_cast<int>(newAudioFile.sampleRate);
    
    // clear the audio source and read new WAV
    this->AudioSourceBuffer.reset(new AudioSampleBuffer(newAudioFile.numChannels, length));
    newAudioFile.read(this->AudioSourceBuffer.get(), 0, length, 0, true, true);
}

void GrainStream::setStreamSize(int size)
{
    if (size > this->grainStreamSize)
        addGrainsToStream(size - this->grainStreamSize);
    else if (size < this->grainStreamSize)
        removeGrainsFromStream(this->grainStreamSize - size);
}

vector<double> GrainStream::getCurrentGrainPosition(int channel)
{
    vector<double> positions;
    
        for (int i = 0; i <= grains.size(); i++)
        {
            positions.emplace_back(grains[i].grainDataCurrentSample[channel]);
        }
        return positions;
}

void GrainStream::randomizeGrain(oneGrain& grain)
{
    Random rand = Random();
    
    // Ensure the Starting Sample is non-negative
    if (filePositionOffset == 0 || (this->filePosition - filePositionOffset) <= 0)
        grain.grainDataStartPosition = this->filePosition;
    else
    {
        // Randomize the Starting Sample
        grain.grainDataStartPosition = rand.nextInt(Range<int>(this->filePosition - filePositionOffset, this->filePosition + filePositionOffset));
        
        // Clamp the Starting Sample to be Within the WaveTable Range
        if (grain.grainDataStartPosition < 0)
            grain.grainDataStartPosition = 0;
        else if (grain.grainDataStartPosition >= this->fileSize)
            grain.grainDataStartPosition = (this->fileSize - 1);
    }
    
    // Set the Current Sample to the Starting Sample
    grain.grainDataCurrentSample[LEFT_CHANNEL] = static_cast<double>(grain.grainDataStartPosition);
    grain.grainDataCurrentSample[RIGHT_CHANNEL] = static_cast<double>(grain.grainDataStartPosition);
//    cout << grain.grainDataCurrentSample[LEFT_CHANNEL] << endl;
    // Clamp the End Sample to be Within the WaveTable Range
    grain.grainDataEndPosition = grain.grainDataStartPosition + this->sampleDelta;
    if (grain.grainDataEndPosition >= this->fileSize)
        grain.grainDataEndPosition = (this->fileSize - 1);
    
    // Randomize the Pitch
    int randomPitch = 0;
    if(pitchOffsetForOneGrain > 0)
        randomPitch = rand.nextInt(Range<int>(-pitchOffsetForOneGrain, pitchOffsetForOneGrain));
    grain.grainDataPitchScalar = std::pow(2.0f, static_cast<double>(randomPitch) / 12.0f);
    
    // Randomize the Grain Gain
    double randomGain = 0.0;
    if(gainOffsetForOneGrain < 0)
        randomGain = static_cast<double>(rand.nextInt(Range<int>(gainOffsetForOneGrain, 0)));
    grain.grainDataGainScalar = Decibels::decibelsToGain<double>(randomGain);
    
}

void GrainStream::addGrainsToStream(int count)
{
    for (int i = 0; i < count; i++)
    {
        // append a grain the stream of grains
        grains.push_back(oneGrain());
        // reference the last element(newly added grain) and randomize it
        // randomizeGrain(grains.back());
    }
    
    this->grainStreamSize += count;
}

void GrainStream::removeGrainsFromStream(int count)
{
    for (int i = 0; i < count; i++)
        grains.pop_back();
    
    this->grainStreamSize -= count;
}

void GrainStream::silenceAllGrains()
{
    for (oneGrain& grain : grains)
        grain.grainDataIsFinished = true;
    
    grainStreamIsActive = false;
}

void GrainStream::getCurrentPosition(oneGrain& grain)
{
        cout << grain.grainDataCurrentSample[0] << endl;
}
