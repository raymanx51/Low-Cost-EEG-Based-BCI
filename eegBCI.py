# eegBCI.py
# 
# Runs two scripts to receive EEG data from a microcontroller and 
# to control a keyboard using the EEG data. A pipe is created to 
# share data between the two scripts.
# Author: Ronan Byrne
# Last Updated: 09/05/2018
#

import multiprocessing
import numpy as np
import eegInterface
import eegScope

if __name__ == '__main__':
    # Interface arguments
    window_size = [1200, 700]
    checker_cycles = 4                                  # Number of times texture repeats in box
    checker_size = 160
    checker_tex = np.array([[1, -1], [-1, 1]])          # One black and one white box
    checker_frequency = np.array([10, 20, 15, 5, 12])   # Flashing Frequencies

    # Serial port
    # port = '/dev/ttyUSB0'
    port = 'COM4'

    # Creating a pipe
    parent_conn, child_conn = multiprocessing.Pipe()

    # Create processes for user interface and scope
    interface_pro = multiprocessing.Process(target=eegInterface.BCI, args=(window_size, checker_frequency,
                                                        checker_size, checker_cycles, checker_tex, child_conn))
    graphing_pro = multiprocessing.Process(target=eegScope.Scope, args=(port, parent_conn))

    interface_pro.start()
    graphing_pro.start()

    # Close the processes before finishing
    interface_pro.join()
    graphing_pro.join()