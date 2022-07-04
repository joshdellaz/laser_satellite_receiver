# S.O.C.S (Satellite Optical Comminications System)
Code for transmitting and receiving data to/from a satellite using laser light via on-off keying (OOK). Current project scope includes:
* Physical Layer
  * OOK modulation and demodulation
  * Synchronization using integrated maximum length sequence autocorrelator and polyphase filterbank
  * Automatic gain control (to be implemented)
  * Interface with Eclypse Z7 + Zmod ADC and DAC modules (obselete)
* Data Link Layer
  * Forward Error Correction (low density parity check)
  * Cyclic redundancy check
  * Scrambling, interleaving, packetizing

All code is written in C(++) f

## matlab_files
contains a range of matlab and associated files used for basic testing
* /pole_filter_phasesync_test.m is a working example of a polyphase filterbank used for phase syncronization of oversampled square wave

## c_project
Contains all project code for C/C++ implementation

**Development instructions:**
* Install linux (preferably latest ubuntu LTS release) on a PC or in a VM
* install git, gcc, and make via command line
* Install [liquid-dsp]. Follow all instructions on readme
* Install VS Code and its C/C++ extension
* File -> Open Folder -> select "c_project"
* To build, open main.c and either press F5 or click Run -> Start Debugging
* To test changes to code, it is recommend to implement the functionality in fullSendTest() and debug the program. This way, the new functionality is tested in an end-to-end (transmit to receive) manner.

[liquid-dsp]: https://github.com/jgaeddert/liquid-dsp
