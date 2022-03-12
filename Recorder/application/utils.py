"""Utility functions for recording from a Microbit."""


import os, threading, time
import serial, binascii, librosa
import scipy.io.wavfile as wavf
import numpy as np


EMPTY: str = ""
PORT_NAME: str = 'COM3'  # On MacOS, find your serial port by running `ls /dev/tty.*`
OUT_FOLDER: str = os.path.join(os.getcwd(), 'output', 'christine', 'test')
OUT_FILE_NAME: str = 'out_'
OUT_FILE_EXTENSION: str = '.wav'
NUM_BITS: int = 8
HEX_BASE: int = 16
CONSTANT: int = 127  # TODO: rename
CHUNK_SIZE: int = 2
RECORDING_HERTZ: int = 11000
RECORDING_RATE: int = 0.5


INITIAL_LIST_FILES = os.listdir(OUT_FOLDER)
sound_files_only: list[str] = [file for file in INITIAL_LIST_FILES if file.endswith(OUT_FILE_EXTENSION)]
RESULT_START: int = len(sound_files_only)


result: str = EMPTY
stop_flag: bool = False
num_results: int = RESULT_START
ser = serial.Serial(port= PORT_NAME, baudrate=115200, timeout=None)


def record():
    """Start recording and writing to the result."""
    global result
    global stop

    while not stop_flag:
        bytesToRead = ser.inWaiting()
        if bytesToRead > 0:
            result += binascii.hexlify(bytearray(ser.read(bytesToRead))).decode('ascii')
        
        time.sleep(0.2)
record_thread = threading.Thread(target=record)


def start():
    """Start a thread for recording."""
    global result
    global record_thread

    print("Recording...")
    record_thread.start()


def split():
    """Stop current recording and immediately initiate a new file."""
    write_to_file()


def stop():
    """Write last result and stop recording."""
    global record_thread
    global ser
    global stop

    write_to_file()
    ser.close()

    stop = True
    record_thread.join()  # TODO: fix to exit gracefully

    print("Stopped recording.")


def write_to_file():
    """Write result to the specified file."""
    global result
    global num_results

    data = np.asarray([ twos_complement(result[i : i + CHUNK_SIZE]) + CONSTANT for i in range(RESULT_START, len(result), CHUNK_SIZE) ])
    print(data)
    fs = RECORDING_HERTZ

    num_results += 1
    outfile: str = os.path.join(OUT_FOLDER, OUT_FILE_NAME + str(num_results) + OUT_FILE_EXTENSION)

    wavf.write(outfile, fs, data.astype(np.uint8))
    song, fs = librosa.load(outfile)
    wavf.write(outfile, fs, librosa.effects.time_stretch(song, rate = RECORDING_RATE))

    # Reset result data
    result = EMPTY

    print("Output to " + outfile)


def twos_complement(hexstr):
    """Determine the two's complement representation of a given hex string."""
    value = int(hexstr, HEX_BASE)

    if value & (1 << (NUM_BITS - 1)):
        value -= 1 << NUM_BITS
    return value