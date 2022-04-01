import PySimpleGUI as sg
import scipy.io.wavfile as wavf
import numpy as np
import os, binascii, serial, threading, time, librosa

PORT_NAME: str = 'COM3' #'/dev/tty.usbmodem14102'

stop_flag = False
data_result = ""
ser = serial.Serial(port=PORT_NAME, baudrate=115200, timeout=None)
record_thread = None

folder, subdir, filename = "","",""
choices = []

# SERIAL PART

def twos_complement(hexstr,bits):
    value = int(hexstr, 16)
    if value & (1 << (bits-1)):
        value -= 1 << bits
    return value

def record():
    global data_result
    while not stop_flag:
        bytesToRead = ser.inWaiting()
        if bytesToRead > 0:
            data_result += binascii.hexlify(bytearray(ser.read(bytesToRead))).decode('ascii')        
        time.sleep(0.2)

def start_recording():
    record_thread.start()

def stop_recording():
    global stop_flag
    write_to_file(data_result)
    ser.close()
    stop_flag = True
    record_thread.join()

def write_to_file(data_result):
    chunks, chunk_size = len(data_result), 2
    print(data_result + "\n")
    data = np.asarray([ twos_complement(data_result[i : i + chunk_size], 8) + 127 for i in range(0, chunks, chunk_size) ]) # 8-bit
    fs = 11000
    out_f = os.path.join(folder, subdir, filename)
    wavf.write(out_f, fs, data.astype(np.uint8)) # 8-bit
    song, fs = librosa.load(out_f)
    wavf.write(out_f, fs, librosa.effects.time_stretch(song, rate = 0.5))

# GUI PART

def defaultNewFilename(path):
    count = len(list(filter(lambda subfile: os.path.isfile(os.path.join(path, subfile)) and not subfile.startswith('.'), os.listdir(path))))
    return str(count + 1)

if __name__ == "__main__":
    folder_browse = [sg.Text('Save to Directory: '), sg.In(size=(40,1), enable_events=True ,key='-FOLDER-'), sg.FolderBrowse()]
    subdir_listbox = sg.Listbox(choices, select_mode = sg.LISTBOX_SELECT_MODE_SINGLE, size=(30, len(choices)), key='-SUBDIR-')
    choose_class_btn = sg.Button("Choose Class", key='-ChooseClass-', disabled = True, disabled_button_color='#778899')
    add_class_btn = sg.Button("Add a new Class", key='-AddClass-', disabled = True, disabled_button_color='#778899')
    text_show_dir = sg.Text('Save the recordings to __ with the name', size=(25,3),auto_size_text=True)
    new_file_name = sg.Input(size=(15,1), key='-FILENAME-')
    start_recording_btn = sg.Button("Start Recording", key='-StartRecording-', disabled = True, disabled_button_color='#778899')
    stop_recording_btn = sg.Button("Stop Recording", key='-StopRecording-', disabled = True, disabled_button_color='#778899')

    layout = [ folder_browse,
            [sg.Column([[subdir_listbox], [choose_class_btn], [sg.In(size=(15,1), key='-NEWCLASS-'), add_class_btn]]),
            sg.Column([[text_show_dir],[new_file_name, sg.Text('.wav')],[start_recording_btn,stop_recording_btn]],vertical_alignment='top')]]

    window = sg.Window('Microbit Recorder', layout,resizable=True)

    while True:
        event, values = window.read()
        if event in (sg.WIN_CLOSED, 'Exit'):
            break
        if event == '-FOLDER-':
            folder = values['-FOLDER-']
            choices = list(filter(lambda subdir: os.path.isdir(os.path.join(folder, subdir)), os.listdir(folder)))
            subdir_listbox.update(choices)
            choose_class_btn.update(disabled = False)
            add_class_btn.update(disabled = False)
        if event == '-ChooseClass-':
            if(folder != "" and values["-SUBDIR-"]):
                subdir = values["-SUBDIR-"][0]
                text_show_dir.update('Save the recordings to %s with the name ' % (os.path.join(folder, subdir)))
                new_file_name.update(value=defaultNewFilename(os.path.join(folder, subdir)))
                start_recording_btn.update(disabled = False)
        if event == '-AddClass-':
            if(folder != "" and values["-NEWCLASS-"] != "" and values["-NEWCLASS-"] not in choices):
                new_subdir = values["-NEWCLASS-"]
                os.mkdir(os.path.join(folder, new_subdir))
                choices = list(filter(lambda subdir: os.path.isdir(os.path.join(folder, subdir)), os.listdir(folder)))
                subdir_listbox.update(choices)
        if event == '-StartRecording-':
            start_recording_btn.update(disabled = True)
            stop_recording_btn.update(disabled = False)
            record_thread = threading.Thread(target=record)
            start_recording()
        if event == '-StopRecording-':
            filename = values['-FILENAME-'] + ".wav"
            if(folder != "" and subdir != "" and filename != ".wav"):
                stop_recording()
                data_result = ""
                record_thread = None
                new_file_name.update(value=defaultNewFilename(os.path.join(folder, subdir)))
            start_recording_btn.update(disabled = False)
            stop_recording_btn.update(disabled = True)