"""Record from a Microbit.

Run this file on the command line by doing the following:
1) Change the constants according to one's desired settings.
2) Navigate to the sandbox directory of this project on the command line.
3) Enter `python -m Recorder`.

Troubleshooting:
- One may receive an OSError("Error 13 - Must be run as administrator") when attempting to run keyboard.is_pressed due to a security feature.
   - Command to run as administrator: sudo python -m Recorder
"""


import serial, binascii, librosa
import scipy.io.wavfile as wavf
import numpy as np
import keyboard

#change it to the port you want
PORT_NAME = 'COM3'
#change this to the directory and filename you want to save to
OUT_FILE = 'out.wav'

def main():
    result = ""

    while True:
        if keyboard.is_pressed('s'):
            print("Start Recording...")
            break

    ser = serial.Serial(port= PORT_NAME, baudrate=115200, timeout=None)
    while True:
        bytesToRead = ser.inWaiting()
        if bytesToRead > 0:
            result += binascii.hexlify(bytearray(ser.read(bytesToRead))).decode('ascii')
        if keyboard.is_pressed('q'):
            print("Stop Recording...")
            break
    ser.close()

    chunks, chunk_size = len(result), 2
    data = np.asarray([ twos_complement(result[i : i + chunk_size], 8) + 127 for i in range(0, chunks, chunk_size) ])
    fs = 11000

    wavf.write(OUT_FILE, fs, data.astype(np.uint8))
    song, fs = librosa.load(OUT_FILE)
    wavf.write(OUT_FILE, fs, librosa.effects.time_stretch(song, rate = 0.5))

    print("Output to " + OUT_FILE)


def twos_complement(hexstr,bits):
    value = int(hexstr,16)

    if value & (1 << (bits-1)):
        value -= 1 << bits
    return value


if __name__ == "__main__":
    main()