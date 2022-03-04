"""Simple application to record from a Microbit.

Run this file on the command line by doing the following:
1) Change the constants in this file and in utils.py according to one's desired settings.
2) Navigate to the sandbox directory of this project on the command line.
3) Enter `python -m ui`.

Resources:
http://tkdocs.com/tutorial/firstexample.html 
"""


import os
from tkinter import ttk
from tkinter import *
import utils


FRAME_PADDING: str = "3 3 12 12"


def main():
    """Entrypoint to the application."""
    # Set up the main application window
    root: Tk = Tk()
    root.title("Record from Microbit")

    # Create a frame to hold contents of the UI
    mainframe: ttk.Frame = ttk.Frame(root, padding = FRAME_PADDING)  # Create frame
    mainframe.grid(column= 0, row= 0, sticky=(N, W, E, S))  # Place it in the main application window
    root.columnconfigure(0, weight= 1)  # Expand frame to fill extra space if window is resized
    root.rowconfigure(0, weight= 1)  # Continuation of above

    # Create a scrollable box to list files in (based on https://www.geeksforgeeks.org/scrollable-listbox-in-python-tkinter/)
    listbox = Listbox(mainframe)
    listbox.pack(side = LEFT, fill = BOTH, padx = 5)  # Position the list on the left
    scrollbar = Scrollbar(mainframe)  # Create scrollbar
    scrollbar.pack(side = LEFT, fill = BOTH, padx = 10)  # Position the scrollbar on the left
    file_list_var = StringVar(value = os.listdir(utils.OUT_FOLDER))
    listbox.config(yscrollcommand = scrollbar.set, listvariable=file_list_var)  # Attach scrollbar to listbox and associate files with listbox
    scrollbar.config(command = listbox.yview)  # Set scrollbar command parameter

    # Create a start button to begin recording
    start_button = Button(mainframe, text="Start", command=utils.start)
    start_button.pack(side = TOP, padx = 5, pady = 5)

    # Create a split button to split the recording
    split_button = Button(mainframe, text="Split", command=utils.split)
    split_button.pack(side = TOP, padx = 5, pady = 5)

    # Create a stop button to stop recording
    stop_button = Button(mainframe, text="Stop", command=utils.stop)
    stop_button.pack(side = TOP, padx = 5, pady = 5)

    # TODO: Create a recording indicator

    # # Insert elements into the listbox (inefficient)
    def refreshList():  # Taken from https://stackoverflow.com/questions/46625722/how-to-list-files-in-a-folder-to-a-tk-listbox-python3 
        """Refresh the list of files shown."""
        fileList = os.listdir(utils.OUT_FOLDER)
        listbox.delete(0, END)
        for file in fileList:
            if file.endswith(utils.OUT_FILE_EXTENSION):
                listbox.insert(END, file)
        root.after(1000, refreshList)  # Taken from https://www.tutorialspoint.com/how-do-i-create-an-automatically-updating-gui-using-tkinter-in-python 
    refreshList()

    root.mainloop()


if __name__ == "__main__":
    main()