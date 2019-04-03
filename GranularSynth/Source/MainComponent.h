#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Grain.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent                                   : public AudioAppComponent,
                                                        public Slider::Listener,
                                                        public ChangeListener,
                                                        public Timer
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
    
    //==============================================================================
    void sliderValueChanged(Slider* slider) override;

private:
    //==============================================================================
    //                                  variables
    
    TextButton openFileButton;
    TextButton playButton;
    TextButton stopButton;
    
    Slider  centroidSampleDial;
    Slider  grainDurationDial;
    Slider  startingOffsetDial;
    Slider  streamSizeDial;
    Slider  pitchOffsetDial;
    Slider  grainStreamGainDial;
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
    
    vector<GrainStream> gStream;        // vector of grains (stream)
    GrainStream activeGrain;            // reference to current active grain
    
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
    
    //==============================================================================
    //                    functions only used in waveform drawing
    
    void changeListenerCallback(ChangeBroadcaster* source) override;
    
    void transportSourceChanged();
    
    void thumbnailChanged();
    
    void paintIfNoFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);
    
    void paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);
    
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
