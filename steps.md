# Outline of steps

Source: [Edge Impulse](https://www.edgeimpulse.com/blog/voice-activated-microbit)

1) Train model using the links [here](https://www.edgeimpulse.com/blog/voice-activated-microbit) and download as C++ library (should have three folders)
2) Clone [the voice-activated microbit repo](https://github.com/edgeimpulse/voice-activated-microbit/tree/master/source) 
3) If they are in the repository, use `rm -r` on the following directories:
    - source/edge-impulse-sdk/
    - source/model-parameters/
    - source/tflite-model/
4) Drag the folders from the downloaded library into the source directory
5) Build the library by running `python build.py`
6) Drag the resulting hex file onto your microbit!