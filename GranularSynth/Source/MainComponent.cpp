#include "MainComponent.h"
#include "Grain.h"

//==============================================================================
MainComponent::MainComponent()  :   activeGrain(),
                                    thumbnailCache(5),
                                    thumbnail(512, formatManager, thumbnailCache)
{
    setupButtonsAndDials();
    
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (740, 380);
    
    // register the audio file reader
    formatManager.registerBasicFormats();
    
    // waveform visualisation
    activeGrain.addChangeListener(this);
    thumbnail.addChangeListener(this);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    sampleRate = 0;
    samplesPerBlockExpected = 0;
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()
    // get the next audio block if the grain is being used
    if (activeGrain.gsIsPlaying)
    {
        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            // get a pointer to the start sample in the buffer for this audio output channel
            auto* buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
            
            // fill the required number of samples with
            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                buffer[sample] = activeGrain(channel);
            }
        }
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

void MainComponent::sliderValueChanged(Slider * slider)
{
    //centroid sample
    if (slider == &centroidSampleDial)
        activeGrain.setCentroidSample(static_cast<int>(centroidSampleDial.getValue()));
    
    // grain duration
    else if (slider == &grainDurationDial)
        activeGrain.setDuration(static_cast<int>(grainDurationDial.getValue()));
    
    // starting offset
    else if (slider == &startingOffsetDial)
        activeGrain.gsStartingOffset = static_cast<int>(startingOffsetDial.getValue());
    
    // grain stream size
    else if (slider == &streamSizeDial)
        activeGrain.setStreamSize(static_cast<int>(streamSizeDial.getValue()));
    
    // pitch offset
    else if (slider == &pitchOffsetDial)
        activeGrain.gsPitchOffset = static_cast<int>(pitchOffsetDial.getValue());
    
    // grain stream gain
    else if (slider == &grainStreamGainDial)
        activeGrain.gsGlobalGain = Decibels::decibelsToGain<double>(grainStreamGainDial.getValue());
    
    // grain gain offset - random grain amplitude
    else if (slider == &grainGainOffsetDial)
        activeGrain.gsGainOffsetDb = static_cast<int>(grainGainOffsetDial.getValue());
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    int yValue = getHeight()/3+120;
    int xValue = 10;
    int dial_width = 80;
    int dial_height = 80;
    
    centroidSampleDial.setBounds(xValue, getHeight()/3+10, dial_width, dial_height);
    grainDurationDial.setBounds ((xValue += 100), getHeight()/3+10,  dial_width, dial_height);
    startingOffsetDial.setBounds ((xValue += 100), getHeight()/3+10, dial_width, dial_height);
    streamSizeDial.setBounds ((xValue += 100), getHeight()/3+10,  dial_width, dial_height);
    pitchOffsetDial.setBounds((xValue += 100), getHeight()/3+10, dial_width, dial_height);
    grainStreamGainDial.setBounds((xValue += 100), getHeight()/3+10, dial_width, dial_height);
    grainGainOffsetDial.setBounds((xValue += 100), getHeight()/3+10, dial_width, dial_height);
    
    openFileButton.setBounds (10, (yValue += 30), getWidth()/5, 20);
    playButton.setBounds (10, (yValue += 30), getWidth()/5, 20);
    stopButton.setBounds (10, (yValue += 30), getWidth()/5, 20);
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::lightgrey);
    
    Rectangle<int> thumbnailBounds (0, 0, getWidth(), getHeight()/3);
    
    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded (g, thumbnailBounds);
    else
        paintIfFileLoaded (g, thumbnailBounds);
}

void MainComponent::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;
        
        switch (state)
        {
            case TransportState::stopped:
                stopButton.setEnabled (false);
                playButton.setEnabled (true);
                break;
                
            case TransportState::starting:
                playButton.setEnabled (false);
                activeGrain.gsIsPlaying = true;
                changeState(TransportState::playing);
                break;
                
            case TransportState::playing:
                stopButton.setEnabled (true);
                break;
                
            case TransportState::stopping:
                activeGrain.silence();
                changeState(TransportState::stopped);
                break;
        }
    }
}

void MainComponent::openFile()
{
    // close the audio thread while opening a file
    shutdownAudio();
    
    FileChooser chooser("Select a WAV file to play...", {}, "*.wav;*.flac");

    // open file browser
    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        auto* reader = formatManager.createReaderFor(file);

        // load file into the grain
        if (reader != nullptr)
        {
           // enable the playbutton
            playButton.setEnabled(true);
            
            // set the active grain's source audio file to new source
            activeGrain.setAudioSource(*reader);
            // set new thumbanil
            thumbnail.setSource(new FileInputSource (file));
            
            // update the starting sample dial
            centroidSampleDial.setRange(1, activeGrain.getSize());
            centroidSampleDial.setTextValueSuffix("FilePos");
            centroidSampleDial.setNumDecimalPlacesToDisplay(0);
            
            // reset values of all sliders
            centroidSampleDial.setValue(1);
            grainDurationDial.setValue(1);
            startingOffsetDial.setValue(0);
            streamSizeDial.setValue(1);
            pitchOffsetDial.setValue(0);
            grainStreamGainDial.setValue(0);
            grainGainOffsetDial.setValue(0);
            
            // turn the audio thread back on
            setAudioChannels(0, reader->numChannels);
            
            // once the file is loaded, start timer for drawing current position in the sample
            startTimer(40);
            // clear the file reader from memory
            delete reader;
        }
    }
}

void MainComponent::playFile()
{
    changeState(TransportState::starting);
}

void MainComponent::stopFile()
{
    changeState(TransportState::stopping);
}

//==============================================================================
//                    functions used in waveform drawing only

void MainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &activeGrain)  transportSourceChanged();
    if (source == &thumbnail)        thumbnailChanged();
}

void MainComponent::transportSourceChanged()
{
    if (activeGrain.gsIsPlaying)
        changeState (playing);
    else
        changeState (stopped);
}

void MainComponent::thumbnailChanged()
{
    repaint();
}

void MainComponent::paintIfNoFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    g.setColour (Colours::darkgrey);
    g.fillRect (thumbnailBounds);
    g.setColour (Colours::white);
    g.drawFittedText ("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
}

void MainComponent::paintIfFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    g.setColour (Colours::darkgrey);
    g.fillRect (thumbnailBounds);
    
    // pos in samples
    double pos = static_cast<double>(centroidSampleDial.getValue());
    double pos1 = pos/44100.0;
    auto audioLength (thumbnail.getTotalLength());
    
    //draw channel 0 in blue
    g.setColour (Colours::darkred);
    thumbnail.drawChannel(g, thumbnailBounds, 0.0, audioLength, 0, 1.0f);
    
    //draw channel 1 in red
    g.setColour(Colours::royalblue);
    thumbnail.drawChannel(g, thumbnailBounds, 0.0, audioLength, 1, 1.0f);
    
    g.setColour (Colours::green);
    auto drawPosition ((pos1/audioLength) * thumbnailBounds.getWidth() + thumbnailBounds.getX());
    g.drawLine(drawPosition, thumbnailBounds.getY(), drawPosition, thumbnailBounds.getBottom(), 2.0f);
}

void MainComponent::timerCallback()
{
    repaint();
}
//===========================================================================================
//                     setup buttons and dials down here for cleaner code
void MainComponent::setupButtonsAndDials()
{
    // centroid sample dial setup
    centroidSampleDial.setSliderStyle(Slider::RotaryVerticalDrag);
    centroidSampleDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    centroidSampleDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    centroidSampleDial.setRange(1, 2);
    centroidSampleDial.setTextValueSuffix("FilePos");
    centroidSampleDial.addListener(this);
    centroidSampleDial.setNumDecimalPlacesToDisplay(0);
    addAndMakeVisible(centroidSampleDial);
    
    // grain duration dial setup
    grainDurationDial.setSliderStyle(Slider::RotaryVerticalDrag);
    //   mGrainDurationSlider.setRotaryParameters (MathConstants<float>::pi * 1.2f, MathConstants<float>::pi * 2.8f, false);
    grainDurationDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    grainDurationDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    grainDurationDial.setRange (1, 1000);
    grainDurationDial.setTextValueSuffix (" ms");
    grainDurationDial.setNumDecimalPlacesToDisplay(0);
    grainDurationDial.addListener(this);
    addAndMakeVisible (grainDurationDial);
    
    // strem size dial setup
    streamSizeDial.setSliderStyle(Slider::RotaryVerticalDrag);
    streamSizeDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    streamSizeDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    streamSizeDial.setRange (1, 10);
    streamSizeDial.setTextValueSuffix (" grains");
    streamSizeDial.setNumDecimalPlacesToDisplay(0);
    streamSizeDial.addListener(this);
    addAndMakeVisible (streamSizeDial);
    
    // starting offset dial setup
    startingOffsetDial.setSliderStyle(Slider::RotaryVerticalDrag);
    startingOffsetDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    startingOffsetDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    startingOffsetDial.setRange (0, 10000);
    startingOffsetDial.setTextValueSuffix (" samples offset");
    startingOffsetDial.setNumDecimalPlacesToDisplay(0);
    startingOffsetDial.addListener(this);
    addAndMakeVisible (startingOffsetDial);
    
    // pitch offset dial
    pitchOffsetDial.setSliderStyle(Slider::RotaryVerticalDrag);
    pitchOffsetDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    pitchOffsetDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    pitchOffsetDial.setRange (0, 24);
    pitchOffsetDial.setTextValueSuffix (" semitones offset");
    pitchOffsetDial.setNumDecimalPlacesToDisplay(0);
    pitchOffsetDial.addListener(this);
    addAndMakeVisible (pitchOffsetDial);
    
    // grain stream gain dial
    grainStreamGainDial.setSliderStyle(Slider::RotaryVerticalDrag);
    startingOffsetDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    grainStreamGainDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    grainStreamGainDial.setRange (-60, 0);
    grainStreamGainDial.setTextValueSuffix (" dB");
    grainStreamGainDial.setNumDecimalPlacesToDisplay(0);
    grainStreamGainDial.addListener(this);
    addAndMakeVisible (grainStreamGainDial);
    
    // grain gain offset dial
    grainGainOffsetDial.setSliderStyle(Slider::RotaryVerticalDrag);
    grainGainOffsetDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    grainGainOffsetDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    grainGainOffsetDial.setRange (-60, 0);
    grainGainOffsetDial.setTextValueSuffix (" dB");
    grainGainOffsetDial.setNumDecimalPlacesToDisplay(0);
    grainGainOffsetDial.addListener(this);
    addAndMakeVisible (grainGainOffsetDial);
    
    // open file button
    openFileButton.setButtonText("Open File...");
    openFileButton.onClick = [this] { openFile(); };
    addAndMakeVisible(&openFileButton);
    
    // play button
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playFile(); };
    playButton.setColour(TextButton::buttonColourId, Colours::lightgreen);
    playButton.setEnabled(false);
    addAndMakeVisible(&playButton);
    
    // stop button
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopFile(); };
    stopButton.setColour(TextButton::buttonColourId, Colours::lightpink);
    stopButton.setEnabled(false);
    addAndMakeVisible(&stopButton);
}
