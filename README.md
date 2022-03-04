## README

### 1. Setup

- Python package requirements (updated March 4, 2022):
  - Installation: `pip install -r requirements.txt`
  - Write file: `pip freeze > requirements.txt`
  - May need to use pip3 in place of pip for commands to work, or alternatively use conda
- Basic Hardware Information:
  - [Micro::bit](https://microbit.org/) V2 Specs: Arm Cortex-M4, RAM 128KB, Flash 512KB
  - Build-in microphone sensor (More details in [notes](Docs/notes.md))
- Code with python (micro-python):
  - More details under [Micropython](Micropython/README.md) folder
  - the micropython encapsulation is too upper-level and it stops us from accessing some important features (like recording a voiceline), so it is not recommended
- Code with C++ (CODAL):
  - Setup: See this [microbit-v2-samples](https://github.com/lancaster-university/microbit-v2-samples) repository.
  - The V2 version still doesn't have any online document, so I build the inline version with Doxygen and attach it under Docs/CODAL-V2.
    - Clone the Repository and open the [index.html](Docs/CODAL-V2/index.html) under the directory mentioned above (or use the local link).

### 2. Tasks | Related Resources

- Data Preparation 
  - Existing Dataset:
    - [Spoken Digit Dataset](https://www.tensorflow.org/datasets/catalog/spoken_digit)
      - number 0 to 9
      - 6 speakers
      - 3,000 recordings (50 of each digit per speaker)
      - English pronunciations
  - Prepare it Ourselves
    - Code a Voiceline Recorder with C++ on Microbit:
      - Some References:
        - [Official Example on the Microphone](https://github.com/lancaster-university/microbit-v2-samples/blob/master/source/samples/MicrophoneTest.cpp)
        - [An audio recorder done in V1](https://github.com/we-eff/RecordSavePlayAudioData)
      
- Voice Classification:
  - Some References:
    - [Tensorflow Lite Micro](https://github.com/tensorflow/tflite-micro)
      - [Micro Speech Example](https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech)
      - Doesn't support Micro:bit out of the box

    - [Edge Impulse Voice Activated Microbit](https://github.com/edgeimpulse/voice-activated-microbit)
      - Currently the most promising one
      - More details for setup in [steps](Docs/steps.md) or see the original post

    - [Yumium Microbit ML](https://github.com/yumium/microbit-ML)
      - the basic is similar to the Edge Impulse one, but they encapsulate their code into a micro-python library
      - Probably because they want to use this as a teaching method, but this is not necessary for us
      - See the original repo for documents and setup guide
        - Build and flash the micropython environment on the board with [this instruction](https://microbit-micropython.readthedocs.io/en/v2-docs/devguide/flashfirmware.html#micro-bit-v2)

