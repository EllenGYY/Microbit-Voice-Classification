## README

### 1. Setup

- Basic Hardware Information:
  - [Micro::bit](https://microbit.org/) V2 Specs: Arm Cortex-M4, RAM 128KB, Flash 512KB
  - Build-in microphone sensor
- Code with python (micro-python):
  - Environment:
    - [Online Editor](https://python.microbit.org/v/2)
    - [Visual Studio Code Extension](https://marketplace.visualstudio.com/items?itemName=MAKinteract.micro-bit-python)
  - [Read The Docs](https://microbit-micropython.readthedocs.io/en/v2-docs/index.html)

### 2. Resources

- [Tensorflow Lite Micro](https://github.com/tensorflow/tflite-micro)
  - [Micro Speech Example](https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech)

- https://github.com/yumium/microbit-ML

  - Build and flash the micropython environment on the board with the instruction below:
    - https://microbit-micropython.readthedocs.io/en/v2-docs/devguide/flashfirmware.html

- Possible Dataset:
  - https://www.tensorflow.org/datasets/catalog/spoken_digit
    - number 0 to 9
    - 6 speakers
    - 3,000 recordings (50 of each digit per speaker)
    - English pronunciations

- Voice classification (C++):
  - https://github.com/edgeimpulse/voice-activated-microbit

- Basic Tools (edit: now in `sandbox` folder):
  - https://github.com/christineiym/microbit-sandbox