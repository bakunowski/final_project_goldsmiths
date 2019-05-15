#include "Grain.h"

#define LEFT_CHANNEL 0
#define RIGHT_CHANNEL 1

GrainStream::GrainStream(int startingSample, int duration)
{
    this->samplingRate = 0;
    setDuration(duration);
    setFilePosition(startingSample);
    addGrainsToStream();
}

double GrainStream::createGrain(int channel)
{
    float sample = 0;

    for (oneGrain& grain : grains)
    {
        // check if getting the current grain is done
        if (!grain.adsr.isActive())
        {
            setupGrain(grain);
            grain.adsr.noteOn();
            grain.inRelease = false;
        }
        else if(!grain.inRelease)
        {
            if (this->AudioSourceBuffer->getNumChannels() >= 2)
            {
                if((grain.grainDataCurrentSample[LEFT_CHANNEL] >= grain.grainDataEndPosition)
                    &&
                    (grain.grainDataCurrentSample[RIGHT_CHANNEL] >= grain.grainDataEndPosition))
                {
                    grain.adsr.noteOff();
                    grain.inRelease = true;
                }
            }
            else if (grain.grainDataCurrentSample[LEFT_CHANNEL] >= grain.grainDataEndPosition)
            {
                grain.adsr.noteOff();
                grain.inRelease = true;
            }
        }

        //get the current sample form the audio buffer
        float currentSample = (this->AudioSourceBuffer->getSample(channel,
					 static_cast<int>(grain.grainDataCurrentSample[channel])));
        
         sample += (currentSample * grain.adsr.getNextSample() *
		    static_cast<float>(grain.grainDataGainScalar));

        grain.grainDataCurrentSample[channel] += grain.grainDataPitchScalar;
        
	// prevent from getting grains out of range
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
}

void GrainStream::setDuration(int duration)
{
    // update duration and compute sampleDelta
    this->grainDuration = duration;
    this->sampleDelta = static_cast<int>(this->samplingRate * (duration/1000.0f));
    
    for (oneGrain& grain : grains)
    {
        grain.grainDataEndPosition = grain.grainDataStartPosition + this->sampleDelta;

        // clamp the end sample
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

void GrainStream::setupGrain(oneGrain& grain)
{
    grain.adsrParams.attack = 0.8f;
    grain.adsrParams.decay = 0.0f;
    grain.adsrParams.sustain = 1.0f;
    grain.adsrParams.release = 0.8f;
    grain.adsr.setParameters({});

    Random rand = Random();
    
    // Ensure the Starting Sample is non-negative
    if (filePositionOffset == 0 || (this->filePosition - filePositionOffset) <= 0)
    {
        grain.grainDataStartPosition = this->filePosition;
    }
    else
    {
        // Randomize the Starting Sample
        grain.grainDataStartPosition = rand.nextInt(Range<int>(this->filePosition -
							       filePositionOffset,
							       this->filePosition +
							       filePositionOffset));

	// Prevent samples from being outside of file range 
        if (grain.grainDataStartPosition < 0)
            grain.grainDataStartPosition = 0;
        else if (grain.grainDataStartPosition >= this->fileSize)
            grain.grainDataStartPosition = (this->fileSize - 1);
    }
    
    // Set the Current Sample to the Starting Sample
    grain.grainDataCurrentSample[LEFT_CHANNEL] = static_cast<double>(grain.grainDataStartPosition);
    grain.grainDataCurrentSample[RIGHT_CHANNEL] = static_cast<double>(grain.grainDataStartPosition);
    
    // Clamp the End Sample to be within filesize 
    grain.grainDataEndPosition = grain.grainDataStartPosition + this->sampleDelta;
    if (grain.grainDataEndPosition >= this->fileSize)
        grain.grainDataEndPosition = (this->fileSize - 1);
    
    // Change the pitch
    if(pitchOffsetForOneGrain > -13)
    {
        grain.grainDataPitchScalar = std::pow(2.0f, pitchOffsetForOneGrain / 12.0f);
    }
}

void GrainStream::addGrainsToStream(int count)
{
    for (int i = 0; i < count; i++)
    {
        // append a grain the stream of grains
        grains.push_back(oneGrain());
    }
    
    this->grainStreamSize += count;
}

void GrainStream::removeGrainsFromStream(int count)
{
    for (int i = 0; i < count; i++)
    {
        grains.pop_back();
    }
    
    this->grainStreamSize -= count;
}

void GrainStream::silenceAllGrains()
{
    for (oneGrain& grain : grains)
    {
        grain.adsr.noteOff();
        grain.inRelease = true;
    }
   
    grainStreamIsActive = false;
    
}
