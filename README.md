# JUCE Framework 開發環境設定

## 安裝 [JUCE](https://shop.juce.com/get-juce)
![](https://i.imgur.com/reJENf3.png)
如上圖所示，可以選擇 ==Personal== 或 ==Education== 方案


## 設定指定資料夾
1. 將下載完後的 JUCE 資料夾放到你指定的地方
2. 執行 Projucer，設定 Global Paths
    - Path to JUCE
        - ex: /Users/username/Desktop/JUCE
    - JUCE Modules
        - ex: /Users/username/Desktop/JUCE/modules
    - 如下圖所示:
    ![](https://i.imgur.com/YQxKr2f.png)

執行完以上兩個步驟後我們就可以開始建立專案

## 1. 建立 JUCE project
1. 選擇欲建立 project 類型，這邊選擇 Plug-in
    - Application
        - Blank: 建立一個空白的應用程式
        - GUI: 建立一個帶有 GUI 介面的應用程式
        - Audio：跟 GUI application 相似，但能更有效地處理音訊，可以利用此項目開                 發遊戲，多媒體應用程式
        - Console: 建立一個不帶有 GUI 介面的應用程式
        - Animated: 建立一個可以繪製動畫圖形顯示的應用程式
        - OpenGL: 建立一個支援 OpenGL 的應用程式
    - Plug-in
        - Basic: 建立一個基本的音訊外掛，支援類型有 VST, AudioUnit 和 AAX 等
    - Library
        - Static/Dynamic Library: 支援開發 Library
3. 設定 Project Name
4. 設定 Path to Modules (選完下次就不用設定了)
5. 選擇 Project 編譯平台
6. 按 Create Project
![](https://i.imgur.com/dFjVy7D.png)

## Project 設定
1. 按左上方按鈕
2. 選擇 Plugin Format，勾選 VST3
3. 選擇 Plugin 類型與輸入型態
    - Example:
        - Midi Synthesizer: 勾選 Plugin is a Synth, Plugin MIDI input 
        - Audio plugin: 都不用勾選
    - 這邊我們選擇勾選 Plugin is a Synth, Plugin MIDI input
4. 在 IDE 打開
![](https://i.imgur.com/JPLfJ2I.png)


# Midi Sine Wave Synthesizer 範例說明
- 除了 JUCE 幫我們建立的四個檔案 PluginProcessor.h/.cpp, PluginEditor.h/.cpp，我們還需要建立兩個檔案，分別是 SynthSound.h 與 SynthVoice.h/.cpp

## PluginProcessor.h
- PluginProcessor 負責處理音訊與 Midi 訊號，處理音訊的演算法也會在這邊運行。
```cpp=
class NewProjectAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    NewProjectAudioProcessor();
    ~NewProjectAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState tree;
private:
    //==============================================================================
    juce::Synthesiser mySynth;
    
    double lastSampleRate;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)
};
```
- JUCE 提供一個好用的 class ，[juce::AudioProcessorValueTreeState](https://docs.juce.com/master/classAudioProcessorValueTreeState.html) 這個 class 能夠幫我們管理 PluginProcessor 的變數並連接對應的 GUI 元件。
- [juce::Synthesiser](https://docs.juce.com/master/classSynthesiser.html) 是一個 based class， 我們可以利用此 class 來開發我們的合成器
:::info
1. 要建立合成器我們必須建立 juce::SynthesiserSound 與 juce::SynthesiserVoice，Sound 用來描述合成器可以發出的音色，Voice 則用來發出聲音。
2. 使用 addVoice() 和 addSound() 方法為合成器提供一組或多組音色，以及一組可用於播放它們的聲音。如果你只給它一個 Voice，合成器就會是單音的，反之，合成器的 Voice 越多，可同時發出更多聲音。
3. 重複呼叫 renderNextBlock() 方法來產生音訊。任何 MIDI 事件都會被掃描以獲取 note on/off messenge，這些 MIDI messenges 用於啟動和停止播放 Voice。
4. 在 renderNextBlock() 之前，一定要呼叫 setCurrentPlaybackSampleRate() 告訴它目前的取樣頻率是多少，以便正確地調整輸出音高。
::: 

### 以下幾個為在 PluginProcessor 中常用且重要的函式
- 在 PluginProcessor 初始化時，我們必須先初始化 tree，然後再加入 sound 與 voice。這邊五個 voice 代表合成器一次最多可以發出五個聲音。
```cpp=
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
     tree(*this, nullptr, "PARAM",
          std::make_unique<juce::AudioParameterFloat>("level",
                                                      "Level",
                                                      juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f), 0.5f,
                                                      juce::String(),
                                                      juce::AudioProcessorParameter::genericParameter,
                                                      [](float value, int){return juce::String(value);},
                                                      [](juce::String text){ return text.getFloatValue();}))
#endif
{
    mySynth.clearSounds();
    mySynth.addSound(new SynthSound());
    
    mySynth.clearVoices();
    for (int i = 0; i < 5; i++)
    {
        mySynth.addVoice(new SynthVoice());
    }
}
```

- 在 prepareToPlay 函式中，處理的是 DAW 要播放前的前置動作，如前面介紹 juce::Synthesiser 時所述，在播放聲音前我們必須呼叫 setCurrentPlaybackSampleRate() 來設定取樣頻率。
```cpp=
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    lastSampleRate = sampleRate;
    mySynth.setCurrentPlaybackSampleRate(sampleRate);
}
```
- 在 processblock 函式中我們需要處理音訊演算法。在 DAW 中，會將待處理音訊切成許多block，常見的 block 點數為 128, 256, 512......等，意思就是說我們需要在一個 block 的時間完成運算。假設我們設定 block size 為 512 點，在取樣頻率為 44100 Hz 的情況下，我們必須在 0.0116 秒 (512/44100 秒)內完成運算。
```cpp=
void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    for (int i = 0; i < mySynth.getNumVoices(); i++)
    {
        auto* myVoice = dynamic_cast<SynthVoice*>(mySynth.getVoice(i));
        myVoice->setLevel(tree.getRawParameterValue("level")->load());
    }
    mySynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
 
}
```

## PluginEditor.h
- PluginEditor 負責 GUI 介面，透過傳入的 PluginProcessor 參數與合成器做溝通
- PluginEditor 繼承了 [juce::Component](https://docs.juce.com/master/classComponent.html) class，這個 class 是所有 ui 元件的 base class。
- paint() 決定要在視窗介面上畫什麼
- resized() 決定 ui 元件擺放的位置
- 我們會在視窗介面上畫出一個 [juce::Slider](https://docs.juce.com/master/classSlider.html)，並透過 [SliderAttachment](https://docs.juce.com/master/classAudioProcessorValueTreeState_1_1SliderAttachment.html) 與 PluginProcessor 中的變數做溝通。
```cpp=
class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& audioProcessor;
    
    juce::Slider levelSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};

```

### 以下為在 PluginEditor 中的函式
- 在初始化時我們必須決定視窗大小與 slider 型態。
- setSize(400, 300) 決定介面視窗大小，width = 400px, height = 300px
- setSliderStyle 決定 slider 的樣式
```cpp=
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    levelSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    levelSlider.setValue(0.5);
    levelSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,
                                true,
                                levelSlider.getTextBoxWidth(),
                                levelSlider.getTextBoxHeight());
    levelSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "level", levelSlider));
    
    addAndMakeVisible(levelSlider);
}


```
- 在 paint 函式中，我們畫上 “Level" 
```cpp=
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    int x = 50;
    int y = 100;
    int width = 50;
    int height = levelSlider.getHeight();
    
    g.drawFittedText("Level", x, y, width, height, juce::Justification::centred, 1);
    
}

```
- 在 resized 函式決定 slider 擺放的位置
```cpp=
void NewProjectAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    int sliderWidth = area.getWidth() / 2;
    int sliderHeight = area.getHeight() / 4;
    int x = 100;
    int y = 100;
    
    levelSlider.setBounds(x, y, sliderWidth, sliderHeight);
}
```
### 視窗介面如下圖所示
![](https://i.imgur.com/2f9kjn8.png)

## SynthVoice.h
- 繼承 [juce::SynthesiserVoice](https://docs.juce.com/master/classSynthesiserVoice.html) 需要 implement 以下函式
    - canPlaySound()
    - startNote()
    - stopNote()
    - renderNextBlock()
    - pitchWheelMoved()
    - controllerMoved()
```cpp=
class SynthVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    
    void stopNote (float velocity, bool allowTailOff) override;

    void pitchWheelMoved (int newPitchWheelValue) override;
    
    void controllerMoved (int controllerNumber, int newControllerValue) override;
   
    void renderNextBlock (juce::AudioBuffer <float> &outputBuffer, int startSample, int numSamples) override;
    
    void setLevel(float newLevel);
    
private:
    float level;
    float frequency;
    int noteMidiNumber;
    float currentAngle;
    float angleIncrement;
    float tailOff;
};

```
- level 決定音量大小
- frequency 決定音高
- currentAngle 為目前 sine wave 的相位
- angleIncrement 為 sine wave 經過一點後的增加多少相位
- tailOff 決定 note off 後音量大小

### 下圖為 sine wave synthesizer 運作原理
\begin{aligned} \sin(\omega t + \theta)
&= \sin(2 \pi f t + \theta) \\
&= \sin(2 \pi \dfrac{f}{f_s} n + \theta) \end{aligned}

![](https://i.imgur.com/nVPdW67.gif)




### SynthVoice 決定了合成器怎麼發出聲音，voice 個數代表合成器同時可以發幾個聲音。
- canPlaySound: 決定 voice 是否能利用這個 sound
```cpp=
bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}
```
- startNote: 合成器琴鍵按下時會呼叫這個函式
```cpp=
void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    noteMidiNumber = midiNoteNumber;
    frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    currentAngle = 0.f;
    angleIncrement = frequency / getSampleRate() * juce::MathConstants<float>::twoPi;
    tailOff = 0.0;
}
```
- stopNote: 合成器琴鍵放開時會呼叫這個函式
```cpp=
void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        if (tailOff == 0.0)
            tailOff = 1.0;
    }
    else
    {
        clearCurrentNote();
        level = 0;
        currentAngle = 0.f;
    }
}
```
- renderNextBlock: 產生聲音的演算法
```cpp=
void SynthVoice::renderNextBlock (juce::AudioBuffer <float> &outputBuffer, int startSample, int numSamples)
{
    if (tailOff > 0.0)
    {
        for (int i = startSample; i < (startSample + numSamples); i++)
        {
            float value = std::sin(currentAngle) * level * tailOff;
            outputBuffer.addSample(0, i, value);
            outputBuffer.addSample(1, i, value);
            
            currentAngle += angleIncrement;
            tailOff *= 0.99;
            
            if (tailOff <= 0.05)
            {
                clearCurrentNote();
                angleIncrement = 0.0;
                level = 0.0;
                break;
            }
        }

    }
    else
    {
        for (int i = startSample; i < (startSample + numSamples); i++)
        {
            float value = std::sin(currentAngle) * level;
            outputBuffer.addSample(0, i, value);
            outputBuffer.addSample(1, i, value);
            
            currentAngle += angleIncrement;
        }
    }
}

```


## SynthSound.h
- SynthSound 決定哪些音在哪些 Midi channels 是有效的
- SynthSound.h
```cpp=
class SynthSound : public juce::SynthesiserSound
{

public:
    bool appliesToNote (int midiNoteNumber) override
    {
        return true;
    }
    
    bool appliesToChannel (int midiNoteNumber) override
    {
        return true;
    }
};
```

### Debug
*  Xcode 可以設定每次 build 完後自動開啟 AudioPluginHost.app，方便 debug
    * Product -> Scheme -> Edit Scheme -> Executable
    路徑：JUCE/extras/AudioPluginHost/Builds/MacOSX/build/Debug/AudioPluginHost.app
![](https://i.imgur.com/hFcFjzf.png)
    * 跑完會出現這個視窗
  ![](https://i.imgur.com/kJO39ic.png)
        右鍵新增要測試的plugin，對他點兩下就可以打開
        ![](https://i.imgur.com/vdLblyp.png)
## JUCE 官方文件
- [Tutorial](https://juce.com/learn/tutorials)
- [JUCE API](https://docs.juce.com/master/index.html)












