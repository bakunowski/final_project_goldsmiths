#pragma once

#include "Grain.h"
#include "Audio_Analysis.h"

#define XMLKEYAUDIOSETTINGS "audioDeviceState"

//==============================================================================
class MainComponent                                   : public AudioAppComponent,
                                                        public ChangeListener,
                                                        public Timer,
                                                        public MenuBarModel,
                                                        public Button::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex (int topLevelMenuIndex, const String&) override;
    void buttonClicked (Button* button) override;
    void menuItemSelected (int menuItemID, int topLevelMenuIndex) override;
    void showAudioSettings();

private:
    //==============================================================================
    //                                  variables
    
    TextButton openFileButton;
    TextButton playButton;
    TextButton stopButton;
    
    Slider  filePositionDial;
    Label   filePositionLabel;
    Slider  grainDurationDial;
    Slider  startingOffsetDial;
    Slider  streamSizeDial;
    Slider  pitchOffsetDial;
    Slider  globalGainDial;
    Slider  grainGainOffsetDial;
    
    enum TransportState
    {
        stopped,
        starting,
        playing,
        stopping
    };
    
    AudioFormatManager formatManager;        // valid audio formats checker
    TransportState state;               // current audio playback state
    
    // a-ha, useless!
    //vector<GrainStream> gStream;        // vector of grains (stream)
    GrainStream grainStream;            // reference to current active grain
    AudioFeatureExtraction audioFeatureExtraction;
    
    // waveform drawing
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;

    //==============================================================================
    //                                functions
    
    void setupButtonsAndDials();
    
    void changeState(TransportState newState);
    
    void openFile();
    
    void playFile();
    
    void stopFile();
    
    void parameterWalkthrough();

    //==============================================================================
    //                    functions only used in waveform drawing
    
    void changeListenerCallback(ChangeBroadcaster* source) override;
    
    void transportSourceChanged();
    
    void thumbnailChanged();
    
    void paintIfNoFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);
    
    void paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);
    
    void timerCallback() override;
    
    //==============================================================================
    //                              menu bar
    ScopedPointer<ApplicationProperties> appProperties;
    std::unique_ptr<XmlElement> savedAudioState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
