#include "MainComponent.h"
#include <algorithm>

//==============================================================================
MainComponent::MainComponent()  :       grainStream(),
                                        audioFeatureExtraction(),
                                        thumbnailCache(5),
                                        thumbnail(512, formatManager, thumbnailCache)

{
    setMacMainMenu(this);
    setupButtonsAndDials();

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
    // change this to essentia desoncstructor
    essentia::shutdown();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    std::cout<< "Sample Rate: "<< sampleRate << std::endl;
    std::cout<< "Buffer Size: "<< samplesPerBlockExpected << std::endl;
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
   
        // computing the essentia algorithms
        if (bufferToFill.buffer->getNumChannels() > 0)
        {
            if (audioFeatureExtraction.playbackBuffer.index < audioFeatureExtraction.getLengthOfBuffer())
            {
                const auto* channelData = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
                for (auto i = 0; i < bufferToFill.numSamples; ++i)
                {
                    audioFeatureExtraction.pushNextSampleIntoEssentiaArray(channelData[i]);
                }
            }
            else
            {

	      //parameterWalkthrough()
	      randomParameterWalkthrough();
            }
        }
    }
}

void MainComponent::releaseResources()
{
}

void MainComponent::randomParameterWalkthrough()
{
  int numOfSamples = 10000;
  //cout << "pool45 " << audioFeatureExtraction.temporaryBuffer2.size() << endl;
  Random rand = Random();
  
  audioFeatureExtraction.computeEssentia();
  
  audioFeatureExtraction.paramPool.add("parameters.position", audioFeatureExtraction.count);
  audioFeatureExtraction.paramPool.add("parameters.duration", audioFeatureExtraction.duration);
  audioFeatureExtraction.paramPool.add("parameters.spread", grainStream.filePositionOffset);
  audioFeatureExtraction.paramPool.add("parameters.numberOfGrains", audioFeatureExtraction.streamSize);
  audioFeatureExtraction.paramPool.add("parameters.pitch", grainStream.pitchOffsetForOneGrain);
  audioFeatureExtraction.mergePoolParams.merge(audioFeatureExtraction.paramPool, "append");
  
  audioFeatureExtraction.clearBufferAndPool();
  
  audioFeatureExtraction.count = rand.nextInt(Range<int>(1, grainStream.getFileSize()));
  grainStream.setFilePosition(audioFeatureExtraction.count);
  audioFeatureExtraction.duration = rand.nextInt(Range<int>(10, 1000));
  grainStream.setDuration(audioFeatureExtraction.duration);
  grainStream.filePositionOffset = rand.nextInt(Range<int>(0, 50000));
  audioFeatureExtraction.streamSize = rand.nextInt(Range<int>(1, 10));
  grainStream.setStreamSize(audioFeatureExtraction.streamSize);
  grainStream.pitchOffsetForOneGrain = rand.nextInt(Range<int>(-12, 12));
  
  audioFeatureExtraction.count2 += 1;
  cout << audioFeatureExtraction.count2 << endl;
  
  if (audioFeatureExtraction.count2 == numOfSamples)
    {
      audioFeatureExtraction.mergedMFCCs->compute();
      audioFeatureExtraction.mergedParameters->compute();
      changeState(TransportState::stopping);
    }
}

void MainComponent::parameterWalkthrough()
{
    audioFeatureExtraction.computeEssentia();
    audioFeatureExtraction.clearBufferAndPool();
    audioFeatureExtraction.count += audioFeatureExtraction.getLengthOfBuffer();

    if (audioFeatureExtraction.count > grainStream.getFileSize())
    {
        audioFeatureExtraction.clearBufferAndPool();
        audioFeatureExtraction.count = 1.0;
        audioFeatureExtraction.duration += 100;
        
        if (audioFeatureExtraction.duration >= 901) // 901
        {
            audioFeatureExtraction.duration = 0;
            grainStream.filePositionOffset += 10000;
            
            if (grainStream.filePositionOffset >= 50000) //50000
            {
                grainStream.filePositionOffset = 0;
                audioFeatureExtraction.streamSize += 4;
                
                if (audioFeatureExtraction.streamSize >= 9) //10
                {
                    audioFeatureExtraction.streamSize = 0;
                    grainStream.pitchOffsetForOneGrain +=12;
                    
                    if (grainStream.pitchOffsetForOneGrain >= 12)//12
                    {
                        audioFeatureExtraction.mergedMFCCs->compute();
                        audioFeatureExtraction.mergedParameters->compute();
                        changeState(stopping);
                    }
                }
            }
        }
    }
    
    grainStream.setFilePosition(audioFeatureExtraction.count);
    grainStream.setDuration(audioFeatureExtraction.duration);
    grainStream.setStreamSize(audioFeatureExtraction.streamSize);

    audioFeatureExtraction.paramPool.add("parameters.position", audioFeatureExtraction.count);
    audioFeatureExtraction.paramPool.add("parameters.duration", audioFeatureExtraction.duration);
    audioFeatureExtraction.paramPool.add("parameters.spread", grainStream.filePositionOffset);
    audioFeatureExtraction.paramPool.add("parameters.numberOfGrains", audioFeatureExtraction.streamSize);
    audioFeatureExtraction.paramPool.add("parameters.pitch", grainStream.pitchOffsetForOneGrain);

    //audioFeatureExtraction.printFluxValues();
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
    globalGainDial.setBounds((xValue += 100), getHeight()/3, dial_width, dial_height);
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
            pitchOffsetDial.setValue(-12);
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
    g.setColour(Colours::olivedrab);
    thumbnail.drawChannel(g, thumbnailBounds, 0.0, audioLength, 1, 1.0f);
    
    g.setColour (Colours::yellow);
    auto drawPosition ((pos1/audioLength) * thumbnailBounds.getWidth() + thumbnailBounds.getX());
    g.drawLine(drawPosition, thumbnailBounds.getY(), drawPosition, thumbnailBounds.getBottom(), 2.0f);

    /*
    for (int i = 0; i < (static_cast<int>(streamSizeDial.getValue())); ++i)
    {
        double grainsPos = (grainStream.getCurrentGrainPosition(0)[i])/44100.0;
        g.setColour(Colours::white);
        auto drawGrainPosition ((grainsPos/audioLength) * thumbnailBounds.getWidth() + thumbnailBounds.getX());
        
        g.drawLine(drawGrainPosition, thumbnailBounds.getY()+10, drawGrainPosition, thumbnailBounds.getBottom()-10, 1.0f);
    }*/

    //flux drawing to see if changes
//    if (state == playing){
//    g.setColour(Colours::white);
//    double y_finish = audioFeatureExtraction.fluxOutput * 1000.0;
//        cout << "value: " << y_finish << endl;
//    g.drawLine(getWidth()/2, getHeight()/2, getWidth()/2, y_finish, 5.0f);
//    }
}

void MainComponent::timerCallback()
{
    repaint();
    
    // update values of dials when doing parameter sweep
    filePositionDial.setValue(audioFeatureExtraction.count);
    grainDurationDial.setValue(audioFeatureExtraction.duration);
    startingOffsetDial.setValue(grainStream.filePositionOffset);
    streamSizeDial.setValue(audioFeatureExtraction.streamSize);
    pitchOffsetDial.setValue(grainStream.pitchOffsetForOneGrain);
}

//====================================================================================================
//                                          menubar stuff
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
    grainDurationDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    grainDurationDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    grainDurationDial.setRange (1, 1000);
    grainDurationDial.onValueChange = [this] {(grainStream.setDuration(static_cast<int>(grainDurationDial.getValue()))); };
    grainDurationDial.setTextValueSuffix (" ms");
    grainDurationDial.setNumDecimalPlacesToDisplay(0);
    //grainDurationDial.addListener(this);
    addAndMakeVisible (grainDurationDial);
                                               
    // strem size dial setup
    streamSizeDial.setSliderStyle(Slider::RotaryVerticalDrag);
    streamSizeDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    streamSizeDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    streamSizeDial.setRange (1, 10);
    streamSizeDial.onValueChange = [this] {(grainStream.setStreamSize(static_cast<int>(streamSizeDial.getValue()))); };
    streamSizeDial.setTextValueSuffix (" grains");
    streamSizeDial.setNumDecimalPlacesToDisplay(0);
    //streamSizeDial.addListener(this);
    addAndMakeVisible (streamSizeDial);
    
    // starting offset dial setup
    startingOffsetDial.setSliderStyle(Slider::RotaryVerticalDrag);
    startingOffsetDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    startingOffsetDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    startingOffsetDial.setRange (0, 50000);
    startingOffsetDial.onValueChange = [this] {(grainStream.filePositionOffset = static_cast<int>(startingOffsetDial.getValue()));};
    startingOffsetDial.setTextValueSuffix (" samples offset");
    startingOffsetDial.setNumDecimalPlacesToDisplay(0);
    //startingOffsetDial.addListener(this);
    addAndMakeVisible (startingOffsetDial);
    
    // pitch offset dial
    pitchOffsetDial.setSliderStyle(Slider::RotaryVerticalDrag);
    pitchOffsetDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    pitchOffsetDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    pitchOffsetDial.setRange (-12, 12);
    pitchOffsetDial.onValueChange = [this] {(grainStream.pitchOffsetForOneGrain = static_cast<int>(pitchOffsetDial.getValue()));};
    pitchOffsetDial.setTextValueSuffix (" semitones offset");
    pitchOffsetDial.setNumDecimalPlacesToDisplay(0);
    //pitchOffsetDial.addListener(this);
    addAndMakeVisible (pitchOffsetDial);
    
    // grain stream gain dial
    globalGainDial.setSliderStyle(Slider::RotaryVerticalDrag);
    globalGainDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    globalGainDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    globalGainDial.setRange (-60, 0);
    globalGainDial.onValueChange = [this] {grainStream.globalGain = Decibels::decibelsToGain<double>(globalGainDial.getValue());};
    globalGainDial.setTextValueSuffix (" dB");
    globalGainDial.setNumDecimalPlacesToDisplay(0);
    //globalGainDial.addListener(this);
    addAndMakeVisible (globalGainDial);
    
    // grain gain offset dial
    grainGainOffsetDial.setSliderStyle(Slider::RotaryVerticalDrag);
    grainGainOffsetDial.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    grainGainOffsetDial.setTextBoxStyle(Slider::TextBoxAbove, true, 100, 20);
    grainGainOffsetDial.setRange (-60, 0);
    grainGainOffsetDial.onValueChange = [this] {(grainStream.gainOffsetForOneGrain = static_cast<int>(grainGainOffsetDial.getValue()));};
    grainGainOffsetDial.setTextValueSuffix (" dB");
    grainGainOffsetDial.setNumDecimalPlacesToDisplay(0);
    //grainGainOffsetDial.addListener(this);
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
