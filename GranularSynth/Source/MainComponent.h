#pragma once

#include "Grain.h"
#include "Audio_Analysis.h"
#include <fdeep/fdeep.hpp>

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
    TextButton MachineLearningButton;
    TextButton MLBUTTON;
    
    bool record = false;
    bool ready = false;
    
    // ml results vector
    std::vector<float> result_vec;

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
    
    GrainStream grainStream;            // reference to current active grain
    AudioFeatureExtraction audioFeatureExtraction; // the essentia setup reference
    
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
    
    void recordButton();
    
    void predict();
    
    void parameterWalkthrough();

    void randomParameterWalkthrough();

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
