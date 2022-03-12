import serial, binascii, librosa
import scipy.io.wavfile as wavf
import numpy as np
import keyboard

#change it to the port you want
port_name = 'COM3'
#change this to the directory and filename you want to save to
out_f = 'out.wav'

def twos_complement(hexstr,bits):
    value = int(hexstr,16)

    if value & (1 << (bits-1)):
        value -= 1 << bits
    return value

result = ""

while True:
    if keyboard.is_pressed('s'):
        print("Start Recording...")
        break

ser = serial.Serial(port= port_name, baudrate=115200, timeout=None)
while True:
    bytesToRead = ser.inWaiting()
    if bytesToRead > 0:
        result += binascii.hexlify(bytearray(ser.read(bytesToRead))).decode('ascii')
    if keyboard.is_pressed('q'):
        print("Stop Recording...")
        break
ser.close()

chunks, chunk_size = len(result), 4
data = np.asarray([ twos_complement(result[i : i + chunk_size], 16) for i in range(0, chunks, chunk_size) ])
fs = 11000
print(data.size)
wavf.write(out_f, fs, data.astype(np.int16))
song, fs = librosa.load(out_f)
wavf.write(out_f, fs, librosa.effects.time_stretch(song, rate = 0.382))

print("Output to " + out_f)