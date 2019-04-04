#include "MainComponent.h"
#include "Grain.h"

//==============================================================================
MainComponent::MainComponent()  :   grainStream(),
                                    thumbnailCache(5),
                                    thumbnail(512, formatManager, thumbnailCache)
{
    setMacMainMenu(this);
    setupButtonsAndDials();
   /*
    // essentia
    preApplyEssentia.buffer.setSize(1, lengthOfEssentiaBuffer);
    essentia::init();
    essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
    fft = factory.create("FFT", "size", 8192);
    
    essentiaInput.reserve(lengthOfEssentiaBuffer);
    essentiaInput.resize(lengthOfEssentiaBuffer, 0.0f);
    
    fft->input("frame").set(essentiaInput);
    fft->output("fft").set(essentiaFFT);
  */
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (1200, 380);

    PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Preferences";
    appProperties = new ApplicationProperties();
    appProperties->setStorageParameters (options);
    auto userSettings = appProperties->getUserSettings();
    savedAudioState = std::unique_ptr<XmlElement>(userSettings->getXmlValue (XMLKEYAUDIOSETTINGS));
    
    // register the audio file reader
    formatManager.registerBasicFormats();
    
    // waveform visualisation
    grainStream.addChangeListener(this);
    thumbnail.addChangeListener(this);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (0, 2, savedAudioState.get()); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2, savedAudioState.get());
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    setMacMainMenu(nullptr);
    shutdownAudio();
    //essentia::shutdown;
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    std::cout<<"Sample Rate: "<<sampleRate<<std::endl;
    std::cout<<"Buffer Size: "<<samplesPerBlockExpected<<std::endl;
    oversampling = new dsp::Oversampling<float>(1, overSampleFactor, dsp::Oversampling<float>::FilterType::filterHalfBandFIREquiripple, false);
    auto channels = static_cast<uint32>(1); // supports only mono input
    dsp::ProcessSpec spec {sampleRate, static_cast<uint32>(samplesPerBlockExpected), channels};
    highpass.prepare(spec);
//    updateHighpassCoefficient(19, sampleRate);
    oversampling->initProcessing(static_cast<size_t>(samplesPerBlockExpected));
    oversampling->reset();
//    sampleRate = 0;
//    samplesPerBlockExpected = 0;
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (grainStream.grainStreamIsActive)
    {
        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            // get a pointer to the start sample in the buffer for this audio output channel
            auto* buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
            
            // fill the required number of samples with
            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                buffer[sample] = grainStream(channel);
            }
        }
    }
   /*
    dsp::AudioBlock<float> block(*bufferToFill.buffer);
    dsp::ProcessContextReplacing<float> context(block);
    dsp::AudioBlock<float> overSampledBlock;
    overSampledBlock = oversampling->processSamplesUp(context.getInputBlock());
    
    // if the essentia buffer is smaller than the length we're aiming for then add samples
    // so this should execute 6 times as 512 x 6 = 81...
    if (preApplyEssentia.index + overSampledBlock.getNumSamples() < lengthOfEssentiaBuffer)
    {
        preApplyEssentia.buffer.copyFrom(0, preApplyEssentia.index, overSampledBlock.getChannelPointer(0), (int)overSampledBlock.getNumSamples());
        preApplyEssentia.index += overSampledBlock.getNumSamples();
    }
    // if it's filled up
    else
    {
        const int remains = lengthOfEssentiaBuffer - preApplyEssentia.index;
        preApplyEssentia.buffer.copyFrom(0, preApplyEssentia.index, overSampledBlock.getChannelPointer(0), remains);
        const float* preBuffer = preApplyEssentia.buffer.getReadPointer(0);
        for (int i = 0; i < lengthOfEssentiaBuffer; ++i)
        {
            essentiaInput.emplace_back(preBuffer[i]);
        }
        
        calculateFFT();
        essentiaInput.clear();

        preApplyEssentia.index = 0;
        const int n = (int)overSampledBlock.getNumSamples() - remains;
        if (n > 0)
        {
            dsp::AudioBlock<float> subBlock = overSampledBlock.getSubBlock(remains);
            preApplyEssentia.buffer.copyFrom(0, preApplyEssentia.index, subBlock.getChannelPointer(0) , n);
            preApplyEssentia.index += n;
        }
    }
    */
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
    // if (slider == &filePositionDial)
    //   grainStream.setFilePosition(static_cast<int>(filePositionDial.getValue()));
    
    // grain duration
    if (slider == &grainDurationDial)
        grainStream.setDuration(static_cast<int>(grainDurationDial.getValue()));
    
    // starting offset
    else if (slider == &startingOffsetDial)
        grainStream.filePositionOffset = static_cast<int>(startingOffsetDial.getValue());
    
    // grain stream size
    else if (slider == &streamSizeDial)
        grainStream.setStreamSize(static_cast<int>(streamSizeDial.getValue()));
    
    // pitch offset
    else if (slider == &pitchOffsetDial)
        grainStream.pitchOffsetForOneGrain = static_cast<int>(pitchOffsetDial.getValue());
    
    // grain stream gain
    else if (slider == &globalGain)
        grainStream.globalGain = Decibels::decibelsToGain<double>(globalGain.getValue());
    
    // grain gain offset - random grain amplitude
    else if (slider == &grainGainOffsetDial)
        grainStream.gainOffsetForOneGrain = static_cast<int>(grainGainOffsetDial.getValue());
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
    
    filePositionDial.setBounds(xValue, getHeight()/3, dial_width, dial_height);
    grainDurationDial.setBounds ((xValue += 100), getHeight()/3,  dial_width, dial_height);
    startingOffsetDial.setBounds ((xValue += 100), getHeight()/3, dial_width, dial_height);
    streamSizeDial.setBounds ((xValue += 100), getHeight()/3,  dial_width, dial_height);
    pitchOffsetDial.setBounds((xValue += 100), getHeight()/3, dial_width, dial_height);
    globalGain.setBounds((xValue += 100), getHeight()/3, dial_width, dial_height);
    grainGainOffsetDial.setBounds((xValue += 100), getHeight()/3, dial_width, dial_height);
    
    openFileButton.setBounds (10, (yValue += 30), getWidth()/5, 20);
    playButton.setBounds (10, (yValue += 30), getWidth()/5, 20);
    stopButton.setBounds (10, (yValue += 30), getWidth()/5, 20);
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::darkgrey);
    
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
                grainStream.grainStreamIsActive = true;
                changeState(TransportState::playing);
                break;
                
            case TransportState::playing:
                stopButton.setEnabled (true);
                break;
                
            case TransportState::stopping:
                grainStream.silenceAllGrains();
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
            grainStream.setAudioSource(*reader);
            // set new thumbanil
            thumbnail.setSource(new FileInputSource (file));
            
            // update the starting sample dial
            filePositionDial.setRange(1, grainStream.getFileSize());
            filePositionDial.setNumDecimalPlacesToDisplay(0);
            
            // reset values of all sliders
            filePositionDial.setValue(1);
            grainDurationDial.setValue(1);
            startingOffsetDial.setValue(0);
            streamSizeDial.setValue(1);
            pitchOffsetDial.setValue(0);
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
    if (source == &grainStream)  transportSourceChanged();
    if (source == &thumbnail)        thumbnailChanged();
}

void MainComponent::transportSourceChanged()
{
    if (grainStream.grainStreamIsActive)
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
    g.setColour (Colours::black);
    g.fillRect (thumbnailBounds);
    g.setColour (Colours::white);
    g.drawFittedText ("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
}

void MainComponent::paintIfFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    g.setColour (Colours::black);
    g.fillRect (thumbnailBounds);
    
    // pos in samples
    double pos = static_cast<double>(filePositionDial.getValue());
    double pos1 = pos/44100.0;
    auto audioLength (thumbnail.getTotalLength());
    
    //draw channel 0 in red
    g.setColour (Colours::darkred);
    thumbnail.drawChannel(g, thumbnailBounds, 0.0, audioLength, 0, 1.0f);
    
    //draw channel 1 in blue
    g.setColour(Colours::darkgrey);
    thumbnail.drawChannel(g, thumbnailBounds, 0.0, audioLength, 1, 1.0f);
    
    g.setColour (Colours::lightyellow);
    auto drawPosition ((pos1/audioLength) * thumbnailBounds.getWidth() + thumbnailBounds.getX());
    g.drawLine(drawPosition, thumbnailBounds.getY(), drawPosition, thumbnailBounds.getBottom(), 2.0f);
}

void MainComponent::timerCallback()
{
    repaint();
}

StringArray MainComponent::getMenuBarNames()
{
    const char* const names[] = {"Settings", nullptr};
    return StringArray (names);
}

PopupMenu MainComponent::getMenuForIndex(int topLevelMenuIndex, const String&)
{
    PopupMenu menu;
    if (topLevelMenuIndex == 0) menu.addItem(1, "Audio Settings");
    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
    if (menuItemID == 1)
    {
        showAudioSettings();
    }
}

void MainComponent::buttonClicked(Button *button)
{
}

void MainComponent::showAudioSettings()
{
   AudioDeviceSelectorComponent audioSettingsComp (deviceManager,
                                                   0, 0,    // audio input
                                                   1, 1,    // audio output
                                                   false,   // midi in
                                                   false,   // midi out
                                                   false,   // stereo pair
                                                   false);  // advanced
    audioSettingsComp.setSize(500, 450);
    
    DialogWindow::LaunchOptions o;
    o.content.setNonOwned (&audioSettingsComp);
    o.dialogTitle                   = "Audio settings";
    o.componentToCentreAround       = this;
    o.dialogBackgroundColour        = Colours::black;
    o.escapeKeyTriggersCloseButton  = true;
    o.useNativeTitleBar             = true;
    o.resizable                     = true;
    o.runModal();
    
    ScopedPointer<XmlElement> audioState (deviceManager.createStateXml());
    appProperties->getUserSettings()->setValue(XMLKEYAUDIOSETTINGS, audioState);
    appProperties->getUserSettings()->saveIfNeeded();
}

void MainComponent::calculateFFT()
{
    fft->compute();
}

//===========================================================================================
//                     setup buttons and dials down here for cleaner code
void MainComponent::setupButtonsAndDials()
{
    // filePositionDial setup
    addAndMakeVisible(filePositionDial);
    filePositionDial.setRange(1, 2);
    filePositionDial.onValueChange = [this] {(grainStream.setFilePosition(static_cast<int>(filePositionDial.getValue()))); };
    filePositionDial.setSliderStyle(Slider::RotaryVerticalDrag);
    filePositionDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    filePositionDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    filePositionDial.setNumDecimalPlacesToDisplay(0);
    
    addAndMakeVisible(filePositionLabel);
    filePositionLabel.setColour(Label::backgroundColourId, Colours::black);
    filePositionLabel.setText("FilePos", dontSendNotification);
    filePositionLabel.attachToComponent(&filePositionDial, false);
    
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
    globalGain.setSliderStyle(Slider::RotaryVerticalDrag);
    globalGain.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    globalGain.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    globalGain.setRange (-60, 0);
    globalGain.setTextValueSuffix (" dB");
    globalGain.setNumDecimalPlacesToDisplay(0);
    globalGain.addListener(this);
    addAndMakeVisible (globalGain);
    
    // grain gain offset dial
    grainGainOffsetDial.setSliderStyle(Slider::RotaryVerticalDrag);
    grainGainOffsetDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    grainGainOffsetDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    grainGainOffsetDial.setRange (-60, 0);
    grainGainOffsetDial.setTextValueSuffix (" dB");
    grainGainOffsetDial.setNumDecimalPlacesToDisplay(0);
    grainGainOffsetDial.addListener(this);
//    addAndMakeVisible (grainGainOffsetDial);
    
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
