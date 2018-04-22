# Delfi-PQ_FDIR

## Description and purpose of the project
Fault Detection, Isolation and Recovery (FDIR) is a vital part of satellite systems engineering due to the harmful effects of radiation in space. Testing of the FDIR system is challenging and especially lacks experience for CubeSats. This project aims to develop open-source software for testing and analyzing FDIR systems, suitable for continuous integration. The developed software will be modular and should be easy to adapt for uses other than the test case. Furthermore, the developed tool will serve to test the FDIR systems on the Delfi-PQ, a 3U PocketCube, developed at Delft University of Technology, which will function as a platform for technology demonstration and education. 

## Repository overview

Next to this README file including all necessary documentation, there are two main files required to use the software: 

- ArduinoPC.ino: This file is the Arduino script in the ArduinoPC folder.
- ComArduino1.py: This file is the Python script.

Besides, this repository consists of two verification Arduino files:

- Pi_Calculation.ino: This file verifies the calculation of pi, based on the Borwein algorithm, is working.
- simplebitflip.ino: This file verifies the bit flip is working. This file changes the bit for a specified value (e.g. 1000) on a specified location (e.g. bit n° 5) and gives the new value.
- Folder with trash?

## Literature study


### How are SEU’s caused?
An SEU, single event upset, is a change in state, caused by one single ionizing particle that strikes asensitive node in a device like a microprocessor, semiconductor or power transistor (micro-electronicdevice). The free charge, created by ionization in an important node of a logic element (such as amemory bit) results in the state change. The error in the output of the device or its operation because of the strike is called a SEU or a soft error.

High energy ionizing particles are part of the natural background in space, called galactic cosmic rays(GCR). Also solar particle events and high energy protons in the magnetosphere of the Earth, the VanAllen radiation belts (trapped particles), cause SEUs. The high energies associated with these particlesmake the spacecraft’s shielding useless in terms of preventing SEUs.

### What are other radiation related faults in electronics?

The critical areas for the design in the natural space radiation environment are:

- Long-term effects such as the total ionizing dose (leading to threshold shifts, timing changes and functional failures) and displacement damage (non-ionizing damage) resulting in device degradation, which is not particularly applicable to microelectronics.
- Transient or single particle effects: soft or hard errors.

If the Linear Energy Transfer (LET) of the particle is greater than the amount of energy or critical charge required, effects may be seen:

- Soft errors: Upset (SEU), transients (SET)
- Hard (destructive) errors: Latchup (SEL), burnout (SEB) and Gate rupture (SEGR)

Note that an SEU in itself is not considered to be permanently damaging to the functionality of the transistor or the circuit. Cases which are permanently damaging are: SEL (single event latchup), SEGR (single event gate rupture) and SEB (single event burnout). The above are examples of single event effects (SEE) which are a general class of radiation effects in electronic devices.

### How could you model SEU’s?

Literature exists on modelling the SEU rates for a given piece of hardware at a given orbit and radiation environment (See Single even effects in aerospace engineering book). This is out of the scope of this project however. SEU’s will be modelled randomly:

- The rate of errors is determined randomly (e.g. uniform distribution)
- The location of the error is determined randomly (e.g. uniform distribution)

### How can we access the memory on the Arduino?

The memory on the Arduino can be accessed randomly.

### What parts of the memory should we inject errors into?

Only into the SRAM, not into the part where the bootloader is stored, otherwise serial communication is not possible anymore and the Arduino will no longer be able to reboot until we flash the bootloader again. 

There will also have to be some code on the Arduino which has to inject the errors into the memory at the memory locations it receives through serial. This code should also not be injected with errors. 

### How will we determine whether an error has occurred? 

This will be done by analysing the housekeeping messages through the Python script.

### What is an error?

Any malfunctioning of the flight software.

### What different kinds of errors do we expect?

- Bad outputs
- Watchdog resets (the watchdog doesn't get kicked)
- Fatal errors (the Arduino stops working and resets)

### Will there be gradations in errors?

Fatal errors are worse than a watchdog reset, which is again worse than wrong outputs.

### How do we interface with the error simulation to know what part of the memory caused the error?

The time and memory address of all error injections will be stored and will be used for the interface with the error analysis software.

### How do we want to visualize the critical memory locations?

On a memory map (rectangular) with three different colors for three gradations in errors.

## Structure of software and hardware interface
For the purpose of testing the FDIR testing software, an Arduino Uno board serves as hardware, representing the OBC of a CubeSat. Furthermore, a serial connection provides the communication with the pc to read out the performance of the software, with is written in Python.

### Design choices

Write about the high level design choices that you had and the reason that you made them.

### Arduino Block diagram

The figure below gives a top-level overview of the script used for the Arduino. 

![img](https://imgur.com/gallery/yy97Ga5)

The script starts by initializing the Setup<sup>(1)</sup> function, indicating the start of the sketch. It is used to initialize the variables, the onboard LED pin mode and the serial connection with the Python script running on the PC. It also sends a debug message, reading "Arduino Reset", and the LED blinks ten times to indicate the Arduino board is functioning. Also the watchdog is initialized: watchdogSetup<sup>(2)</sup>, which in essense is a timer.  It forces a restart when a "system ok" signal is not received coming from the Arduino. The watchdog will reset when no message is received. It results in the same effect as pressing the reset button on the Arduino board.

After initializing and setting the initial values in the setup function, the Loop<sup>(3)</sup> function is looping continuously. It allows to change the program running on the Arduino, resulting in a different responce. This function is actively used to control the Arduino board. The script loops over two main functions:

- getSerialData<sup>(4)</sup>
- processData<sup>(7)</sup>

The getSerialData(4) function receives the data coming from the serial connection and puts it into temp-Buffer<sup>(5)</sup>. It recognizes a new message when the startMarker (byte: 254) is read and the end is markedwith the endMarker (byte: 255). The number of bytes sent by the PC are saved in tempBuffer.

The message sent from the PC is given as follows:

> original message: startMarker + count byte + sentString + endMarker = tempBuffer

and whenever the first byte is the startMarker,  the received message is stored.   The ’count byte’ ishandled in ’dataSentNum’ and the ’sentString’ is handled in thedecodeHighBytes<sup>(6)</sup>. The tempBuffer isused as input into the decodeHighBytes<sup>(6)</sup> function. This function only works with the 'real' data, which is the message in the tempBuffer (sentString), excluding the marker bytes (first and very last byte) and the count byte (tempBuffer[1]). It converts any bytes given as a pair into the intented byte number:

- 253 0 becomes 253
- 253 1 becomes 254
- 253 2 becomes 255

If the special byte is not detected, the byte is kept the same as how it was received.  The result is thus the tempBuffer, where the special bytes within the ’real’ message are decoded into the intended bytes. Now that the data is received, it can be processed in the functionprocessData<sup>(7)</sup>, which requires thetempBuffer as input which is transferred to the variable ’dataSend’.  The message "Arduino working" is also sent to the PC as the housekeeping message. 

The data then is used asinput in thedataToPc<sup>(9)</sup> function, which in principle sends the data from tempBuffer to the PC (Python) using the serial connection. However,  first another function is used before the data is sent,  as themessage  still  has  to  be  encoded,  theencodeHighBytes<sup>(10)</sup> function. The  goal  of  this  function  is  toconvert any bytes of 253 or more into a pair of bytes, 253 0, 253 1 or 253 2 as appropriate, the sameway as it was when the data was received:

- 253 becomes 253 0
- 254 becomes 253 1
- 255 becomes 253 2

The data is then sent to the PC and the data is structured the following way:

> sent message: startMarker + dataTotalSend + data + endMarker

where ’dataTotalSend’ is the number of bytes to send to the PC, taking into account the encoded bytesand data represents the tempBuffer encoded ’real’ data.  After sending the data, the watchdog is resetso that it again start counting from zero to check if a reset is required.

### Python Block diagram

HERE COMES THE PYTHON BLOCK DIAGRAM AND SOME BLABLA ON THE FUNCTIONS


## Flight Software
The flight software consists of several modules which have to be checked by the error analysis software whether the software is working as intended. Below are listed four modules, which could be extended with more advanced satellite software as desired by the user.

### Watchdog timer
In order to verify whether the flight software is functioning as desired, a watchdog kicker is implementedin the flight software. The system is typically designed so that the watchdog timer will be kicked only ifthe computer considers the system functional. The computer determines whether the system is func-tional by conducting one or more fault detection tests and it will kick the watchdog only if all tests havepassed. Whenever the Arduino fails to reset the watchdog timer, the board will reset. Before resetting, a set of actions can be defined to take place.  One option for this would be to store some important parameters in the EEPROM memory so they can be used again after reboot.

### Housekeeping Data
The Arduino will send a housekeeping message to the computer over serial connection at set intervals. These housekeeping messages will provide information to the error analysis software about the functionality of the software.  Currently, 3 housekeeping modules are implemented, which can be extended at anytime. These are:
- Strings: our names: Frederic, Jochim, Bas and Alexander
- Borwein pi: The following simple algorithm approximates the value of 1/π in an iterative way. This Borwein algorithm has quartic convergence properties, such that with only a few iterations, a reasonable approximation is reached. 

![img](http://latex.codecogs.com/svg.latex?y_%7Bk%2B1%7D%3D%5Cfrac%7B1-%281-y_k%5E4%29%5E%7B1%2F4%7D%7D%7B1-%281%2By_k%5E4%29%5E%7B1%2F4%7D%7D)

![img](http://latex.codecogs.com/svg.latex?a_%7Bk%2B1%7D%26%3Da_k%281%2By_%7Bk%2B1%7D%29%5E4-2%5E%7B2k%2B3%7Dy_%7Bk%2B1%7D%281%2By_%7Bk%2B1%7D%2By_%7Bk%2B1%7D%5E2%29)

  The following initial conitions have to be used:

  - ![img](http://latex.codecogs.com/svg.latex?y_0%3D%5Csqrt%7B2%7D-1)
  - ![img](http://latex.codecogs.com/svg.latex?a_0%3D2%28%5Csqrt%7B2%7D-1%29%5E2)
### Reboot message
At booting, the Arduino sends out a message to the PC: "Arduino Reset".  This can be used to keeptrack of the amount of times the Arduino has rebooted.
### Visual check
The on-board LED will be set to blink at set intervals to enable visual inspection of the correct functioning of the software.

Note: The time intervals are to be determined. The on-board watchdog timer on the Arduino board can be set to no more than 8000 ms, so the intervals mentioned above will have to be at least shorter than 8000 ms.

## Running of the code

Some kind of user manual of how to run the code (for the noobs): how to build/use the software and finally how to operate it. 

## Results

The results of the bit flips in various memory locations (have a nice plot).

## Unit tests

Verification of most important functions (the bitflip function, pi function, 

## Project complications

Document any issues that you had.

## Future work and recommendations

 - Write about any future developments that we could do.
 - more advanced flight software
 - testing on other hardware
 - ...
