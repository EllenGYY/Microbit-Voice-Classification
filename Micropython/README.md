# Microbit Sandbox
A folder for testing out the programming of microbit hardware.

## Microbit Resources
- [Official website](https://microbit.org/)

## Useful Packages (Install with pip)
- `microbit`
    -  Useful links:
        - [Official Microbit Package Documentation](https://microbit-micropython.readthedocs.io/en/v1.0.1/microbit.html)
            - [Microphone documentation](https://microbit-micropython.readthedocs.io/en/v2-docs/microphone.html)
        - [MicroPython for Microbit](https://github.com/bbcmicrobit/micropython/tree/v1.0.1)
- `pseudo-microbit`
    - Simply import as `microbit` (ex. `from microbit import speech`)
    - Useful links:
        - [Source repository](https://github.com/MrYsLab/pseudo-microbit)
        - [Documentation](https://mryslab.github.io/pseudo-microbit/install/) 
            - It is written for PyCharm, but Python syntax/concepts apply in VSCode.
- `uflash`
    - For automatically generating hex files from the command line and loading it onto the microbit device.
        - See `run.py` for one way to do this.
        - It may take half a minute or so for the microbit device to run the program. Please be patient.
    - More information can be found in the [official documentation](https://uflash.readthedocs.io/en/latest/). 
