#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Grain.h"

#include <essentia/essentia.h>
#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h>

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
    
    //==============================================================================
    vector<essentia::Real> temporaryBuffer;
    vector<essentia::Real> dcRemovalBuffer;
    vector<essentia::Real> windowingBuffer;
    vector<complex<essentia::Real>> fftBuffer;
    vector<essentia::Real> cartesian2polarMagnitudes;
    vector<essentia::Real> cartesian2polarPhases;
    essentia::Real onsetDetectionResult;
    essentia::Real result;
    vector<essentia::Real> result2;
    // rename the name "matrix" soon !!!!!!!
//    vector<vector<essentia::Real>> matrix;
    TNT::Array2D<essentia::Real> matrix;
    vector<essentia::Real> hfc;
    vector<essentia::Real> weights;
    double onsetRatee;
    vector<essentia::Real> blablabla;
    vector<essentia::Real> onsetRateVector;
    
    vector<essentia::Real> spectrumResults;
    vector<essentia::Real> mfccBands;
    vector<essentia::Real> mfccCoeffs;

    essentia::standard::Algorithm* dcremoval;
    essentia::standard::Algorithm* windowing;
    essentia::standard::Algorithm* fft;
    essentia::standard::Algorithm* cartesian2polar;
    essentia::standard::Algorithm* spectrum;
    essentia::standard::Algorithm* mfcc;
    essentia::standard::Algorithm* onsetDetection;
    essentia::standard::Algorithm* onsets;
    essentia::standard::Algorithm* onsetRate;


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
        bool nextBlockReady = false;
    };
    
    bufferAndIndex preApplyEssentia;

    void estimateMelody();

    void pushNextSampleIntoEssentiaArray(float sample) noexcept;
    
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
