"""File to run a python script on a microbit device from the command line.

Run on the command line by doing the following:
1) Navigate to the root directory of this projecton the command line.
2) Enter `python -m run`.
"""


import uflash
import os


def main():
    # Change this path as necessary.
    uflash.flash(os.path.join(os.getcwd(), "test.py"))


if __name__ == "__main__":
    main()