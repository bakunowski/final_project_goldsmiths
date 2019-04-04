#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Grain.h"

#include <essentia/essentia.h>
#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h>

#define XMLKEYAUDIOSETTINGS "audioDeviceState"

//==============================================================================
class MainComponent                                   : public AudioAppComponent,
                                                        public Slider::Listener,
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
    void sliderValueChanged(Slider* slider) override;
    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex (int topLevelMenuIndex, const String&) override;
    void buttonClicked (Button* button) override;
    void menuItemSelected (int menuItemID, int topLevelMenuIndex) override;
    void showAudioSettings();
    
    //==============================================================================
    std::vector<essentia::Real> essentiaInput;
    std::vector<essentia::Real> essentiaFFT;
    essentia::standard::Algorithm*  fft;


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
    Slider  globalGain;
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
    //                              essentia stuff
    
    static constexpr int lengthOfEssentiaBuffer = 8192; // 6 times the buffer size
    
    // buffer to hold the last 8192 samples for analysis
    struct bufferAndIndex {
        AudioSampleBuffer buffer;
        int index = 0;
    };
    
    bufferAndIndex preApplyEssentia;
    
    ScopedPointer<dsp::Oversampling<float>> oversampling;
    static const int overSampleFactor = 1;
    using iir = dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>;
    dsp::ProcessorChain<iir, iir> highpass;
    
    void calculateFFT();
    
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
