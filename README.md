# Delfi-PQ_FDIR

## Description and purpose of the project
Fault Detection, Isolation and Recovery (FDIR) is a vital part of satellite systems engineering due to the harmful effects of radiation in space. Testing of the FDIR system is challenging and especially lacks experience for CubeSats. This project aims to develop open-source software for testing and analyzing FDIR systems, suitable for continuous integration. The developed software will be modular and should be easy to adapt for uses other than the test case. Furthermore, the developed tool will serve to test the FDIR systems on the Delfi-PQ, a 3U PocketCube, developed at Delft University of Technology, which will function as a platform for technology demonstration and education. 

## Intoduction to Single Event Upsets (SEU)
An SEU, single event upset, is a change in state, caused by one single ionizing particle that strikes asensitive node in a device like a microprocessor, semiconductor or power transistor (micro-electronicdevice). The free charge, created by ionization in an important node of a logic element (such as amemory bit) results in the state change. The error in the output of the device or its operation because of the strike is called a SEU or a soft error.

High energy ionizing particles are part of the natural background in space, called galactic cosmic rays(GCR). Also solar particle events and high energy protons in the magnetosphere of the Earth, the VanAllen radiation belts (trapped particles), cause SEUs. The high energies associated with these particlesmake the spacecraft’s shielding useless in terms of preventing SEUs.

## Structure of software and hardware interface



### Block diagram

The figure below gives a top-level overview of the script used for the Arduino.  The script starts by initializing the Setup(1) function, indicating the start of the sketch. It is used to initialize the variables, the onboard LED pin mode and the serial connection with the Python script running on the PC. It also sends adebug message, reading "Arduino Reset", and the LED blinks twice to indicate the Arduino board is functioning.  Also the watchdog is initialized: watchdogSetup(2), which in essense is a timer.  It forces a restart when a "system ok" signal is not received coming from the Arduino.  The watchdog will reset when no message is received. It results in the same effect as pressing the reset button on the Arduino board.

After initializing and setting the initial values in the setup function, theLoop(3)function is looping contin-uously.  It allows to change the program running on the Arduino, resulting in a different responce.  Thisfunction is actively used to control the Arduino board.The script loops over two main functions:

- getSerialData(4)
- processData(7)

![img](https://i.imgur.com/3O0UwtN.png)



## Flight Software
The flight software consists of several modules which have to be checked by the error analysis softwarewhether the software is working as intended. Below are listed four modules, which could be extended

### Watchdog timer
In order to verify whether the flight software is functioning as desired, a watchdog kicker is implementedin the flight software. The system is typically designed so that the watchdog timer will be kicked only ifthe computer considers the system functional.  The computer determines whether the system is func-tional by conducting one or more fault detection tests and it will kick the watchdog only if all tests havepassed. Whenever the Arduino fails to reset the watchdog timer, the board will reset. Before resetting,a set of actions can be defined to take place.  One option for this would be to store some importantparameters in the EEPROM memory so they can be used again after reboot.

### Housekeeping Data
The Arduino will send a housekeeping message to the computer over serial connection at set intervals. These housekeeping messages will provide information to the error analysis software about the functionality of the software.  Currently, 3 housekeeping modules are implemented, which can be extended at anytime. These are:
-Strings: our names
-Borwein pi: The following simple algorithm approximates the value of 1/π in an iterative way. This Borwein algorithm has quartic convergence properties, such that with only a few iterations, a reasonable approximation is reached.

![img](http://latex.codecogs.com/svg.latex?y_0+%3D%5Csqrt%7B2%7D-1)
![img](http://latex.codecogs.com/svg.latex?a_0+%3D2%28%5Csqrt%7B2%7D-1%29%5E2)

![img](http://latex.codecogs.com/svg.latex?y_%7Bk%2B1%7D+%3D+%5Cfrac%7B1-%281-y_k%5E4%29%5E%7B1%2F4%7D%7D%7B1-%281%2By_k%5E4%29%5E%7B1%2F4%7D%7D)

![img](http://latex.codecogs.com/svg.latex?a_%7Bk%2B1%7D+%26%3D+a_k%281%2By_%7Bk%2B1%7D%29%5E4-2%5E%7B2k%2B3%7Dy_%7Bk%2B1%7D%281%2By_%7Bk%2B1%7D%2By_%7Bk%2B1%7D%5E2%29)
### blabla
other text
## Running of the code

## Check of unit tests

## Future work and recommendations
