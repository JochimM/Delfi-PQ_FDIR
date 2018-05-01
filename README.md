# Delfi-PQ_FDIR

## Description and purpose of the project
Fault Detection, Isolation and Recovery (FDIR) is a vital part of satellite systems engineering due to the harmful effects of radiation in space. Testing of the FDIR system is challenging and especially lacks experience for CubeSats. This project aims to develop open-source software for testing and analysing FDIR systems, suitable for continuous integration. The developed software is intented to be modular and should be easy to adapt for uses other than the test case. Furthermore, the developed tool will serve to test the FDIR systems on the Delfi-PQ, a 3U PocketCube, developed at Delft University of Technology, which will function as a platform for technology demonstration and education. 

During the process of this project, some design choices were made and are listed below:

- The protocol was kept as simple as possible, as this was the first experience with data error analysis using a microcontroller. Therefore, it was decided to send the data with a fixed structure. The data consists of a startmarker (character 254) indicating the beginning of the message followed by the number of bytes in the message (count byte), and an endmarker (character 255) at the very end indicating the end of the message. Furthermore the special byte (character 253) was used in case the begin or end marker was used in the real message of the data sent. A message is defined to be at most 16 bytes long, including the protocol characters. This means that at most 13 bytes are available for sending data. This can easily be changed in the Arduino code by changing maxMessage to a higher value.

- Again for the sake of simplicity, it was decided to only work with characters as the type of the data.

- To access the memory, it was decided to only access part of the SRAM. For the scope of this project, this was sufficient. To access other parts of the memory, more experience would be required with assembler.

- Due to time constraints, flipping of a bit is done in a random fashion in the byte location that gets sent to the Arduino script. In a later stage, it would be adviced to choose the location of the bit to be flipped, in order to investigate more precisely what location of a bit flip causes more problems. 


## Repository overview

Next to this README file including all necessary documentation, there are two main files required to use the software: 

- ArduinoPC.ino: This file is the Arduino script in the ArduinoPC folder.
- ComArduino1.py: This file is the Python script.

Besides, this repository consists of three verification Arduino files:

- Pi_Calculation.ino: This file verifies the calculation of π, based on the Borwein algorithm, is working.
- simplebitflip.ino: This file verifies the bit flip is working. This file changes the bit for a specified value (e.g. 1000) on a specified location (e.g. bit n° 4) and gives the new value.
- checkMemAddr.ino: This file verifies the function to check the memory address and can be found in the checkMemAddr folder.

There is also a python file 'plotter.py' to plot the errors on a memory map.

Furthermore, note there is a trash folder for the user to see what was done, but was found to be unsuccessful. However, there is no documentation on this trash folder.

## Literature study


### How are SEU’s caused?
An SEU, single event upset, is a change in state, caused by one single ionising particle that strikes a sensitive node in a device like a microprocessor, semiconductor or power transistor (micro-electronic device). The free charge, created by ionisation in an important node of a logic element (such as a memory bit) results in the state change. The error in the output of the device or its operation because of the strike is called a SEU or a soft error.

High energy ionising particles are part of the natural background in space, called galactic cosmic rays (GCR). Also solar particle events and high energy protons in the magnetosphere of the Earth, the Van Allen radiation belts (trapped particles), cause SEUs. The high energies associated with these particles make the spacecraft’s shielding useless in terms of preventing SEUs.

### What are other radiation related faults in electronics?

The critical areas for the design in the natural space radiation environment are:

- Long-term effects such as the total ionising dose (leading to threshold shifts, timing changes and functional failures) and displacement damage (non-ionising damage) resulting in device degradation, which is not particularly applicable to microelectronics.
- Transient or single particle effects: soft or hard errors.

If the Linear Energy Transfer (LET) of the particle is greater than the amount of energy or critical charge required, effects may be seen:

- Soft errors: Upset (SEU), transients (SET).
- Hard (destructive) errors: Latchup (SEL), burnout (SEB) and gate rupture (SEGR).

Note that an SEU in itself is not considered to be permanently damaging to the functionality of the transistor or the circuit. Cases which are permanently damaging are: SEL (single event latchup), SEGR (single event gate rupture) and SEB (single event burnout). The above are examples of single event effects (SEE) which are a general class of radiation effects in electronic devices.

### How could you model SEU’s?

Literature exists on modelling the SEU rates for a given piece of hardware at a given orbit and radiation environment. However, this is out of the scope of this project. SEU’s are be modelled randomly:

- The rate of errors is determined randomly (e.g. uniform distribution).
- The location of the error is determined randomly (e.g. uniform distribution).

### How can we access the memory on the Arduino?

The memory on the Arduino can be accessed randomly. The memory access is done using pointers. The pointers are randomly generated on python as strings, then sent to arduino as characters, converted to ints and then accessed in arduino.

### What parts of the memory should we inject errors into?

Only into the SRAM, not into the part where the bootloader is stored, otherwise serial communication is not possible anymore and the Arduino will no longer be able to reboot until we flash the bootloader again. 
The specific locations this code targets are in the interval [257, 2303].

There also has to be some code on the Arduino which has to inject the errors into the memory at the memory locations it receives through serial. This code should also not be injected with errors. 

### How will we determine whether an error has occurred? 

This is done by analysing the housekeeping messages through the Python script.

### What is an error?

Any malfunctioning of the flight software.

### What different kinds of errors do we expect?

- Bad outputs
- Watchdog resets (the watchdog doesn't get kicked)
- Fatal errors (the Arduino stops working and resets)
- Errors in the communication protocol

### Will there be gradations in errors?

Fatal errors are worse than a watchdog reset, which is again worse than wrong outputs.

### How do we interface with the error simulation to know what part of the memory caused the error?

The memory address of all error injections is stored and used for the interface with the error analysis software.

### How do we want to visualize the critical memory locations?

On a memory map (rectangular) with red indicating an error occurred due to a bitflip at that location.

## Structure of software and hardware interface
For the purpose of testing the FDIR testing software, an Arduino Uno board serves as hardware, representing the OBC of a CubeSat. Furthermore, a serial connection provides the communication with the PC to read out the performance of the software,  written in Python. Note that the maximum length of the message is set to 16 bits. 

### Arduino Block diagram

The figure below gives a top-level overview of the script used for the Arduino. 

![img](https://i.imgur.com/gNe45XR.png)

The script starts by initialising the Setup<sup>(0)</sup> function, indicating the start of the sketch. It is used to initialise the variables, the onboard LED pin mode and the serial connection with the Python script running on the PC. It also sends a debug message: debugToPC<sup>(1)</sup>, reading "Arduino Reset", and the LED blinks ten times to indicate the Arduino board is functioning: blinkLED<sup>(16)</sup>. Also the watchdog is initialised: watchdogSetup<sup>(2)</sup>, which is in essense a timer.  It forces a restart when a "system ok" signal is not received coming from the Arduino. The watchdog will reset when no message is received. It results in the same effect as pressing the reset button on the Arduino board.

After initialising and setting the initial values in the setup function, the Loop<sup>(3)</sup> function is looping continuously. It allows to change the programme running on the Arduino, resulting in a different responce. This function is actively used to control the Arduino board. The script loops over three main functions:

- getSerialData<sup>(4)</sup>
- processData<sup>(7)</sup>
- sendHouseKeep<sup>(14)</sup>

The getSerialData(4) function, put in an if-loop for when the microcontroller is receiving data, receives the data coming from the serial connection and puts it into temp-Buffer<sup>(5)</sup>. It recognizes a new message when the startMarker (byte: 254) is read and the end is marked with the endMarker (byte: 255). The number of bytes sent by the PC are saved in tempBuffer.

The message sent from the PC is given as follows:

> original message: startMarker + count byte + sentString + endMarker = tempBuffer

and whenever the first byte is the startMarker,  the received message is stored.   The ’count byte’ is handled in ’dataSentNum’ and the ’sentString’ is handled in decodeHighBytes<sup>(6)</sup>. The tempBuffer is used as input into the decodeHighBytes<sup>(6)</sup> function. This function only works with the 'real' data, which is the message in the tempBuffer (sentString), excluding the marker bytes (first and very last byte) and the count byte (tempBuffer[1]). It converts any bytes given as a pair into the intented byte number:

- 253 0 becomes 253
- 253 1 becomes 254
- 253 2 becomes 255

If the special byte is not detected, the byte is kept the same as how it was received.  The result is thus the tempBuffer, where the special bytes within the ’real’ message are decoded into the intended bytes. Now that the data is received, it can be processed in the functionprocessData<sup>(7)</sup>, which requires the tempBuffer as input. If the full data message is received, the function checkIfMemAddress<sup>(8)</sup> is called. This function checks for the memory address in the data and saves it as 'memAddress'. The function pingFromPC<sup>(9)</sup> does reset the watchdog when a specific message is sent from the PC. 

If a memory address was received, the bitFlip<sup>(10)</sup> is called. This function is changing a random bit from the value corresponding to the memory address. 

The data then is used as input in the dataToPc<sup>(11)</sup> function, which in principle sends the data from tempBuffer (now called 'dataSend' to the PC (Python) using the serial connection. However,  first another function is used before the data is sent,  as the message  still  has  to  be  encoded,  the encodeHighBytes<sup>(12)</sup> function. The  goal  of  this  function  is  to convert any bytes of 253 or more into a pair of bytes, 253 0, 253 1 or 253 2 as appropriate, the same way as it was when the data was received:

- 253 becomes 253 0
- 254 becomes 253 1
- 255 becomes 253 2

The data is then sent to the PC and the data is structured the following way:

> sent message: startMarker + dataTotalSend + data + endMarker

where ’dataTotalSend’ is the number of bytes to be sent to the PC, taking into account the encoded bytes and data represents the tempBuffer encoded ’real’ data.  After sending the data, the watchdog is reset: wdt_reset<sup>(13)</sup> so that it again start counting from zero to check if a reset is required.

The last main function to be discussed is sendHouseKeep<sup>(14)</sup>, where again the dataToPc<sup>(11)</sup> and encodeHighBytes<sup>(12)</sup> functions are used to send two types of information:

- Housekeeping data with the names: Frederic, Jochim, Bas and Alexander.
- Housekeeping data with the π calculation using getBorweinPi<sup>(17)</sup>.

Finally, the function debugToPC<sup>(18)</sup> is used throughout the script, which sends a 0 on the location of the 'count byte' as described above. 

### Python Block diagram

The figure below gives a top level overview of the architecture of the Python script.

![img](https://i.imgur.com/60VrOWo.png)

At first the serial connection (1) with the Arduino is made using the integrated serial module in Python. The Arduino is then set up using the waitforArduino function (2). This function initialises the Arduino and allows time for Arduino reset, it also ensures that any bytes left over from a previous message are discarded. Once the Arduino has been initialised and is ready for operational use, the message 'Arduino Ready' is printed. Once these functions are finished, the operational phase of the python script is entered by using a loop (3) which runs 'numLoops' amount of time.

The first action that is done in this loop is creating a random memory address (4) at which a bitflip will occur, this bitflip is performed in the Arduino script. Since the bitflip has been chosen to occur only on the RAM memory, the random address for usage with an Arduino is a random value between the byte numbers 257 and 2303. The figure below shows the corresponding of these byte numbers with the internal SRAM:

![img](https://i.imgur.com/Q8RK7FS.png)

This number is then send to the Arduino using the sendToArduino function (5), which adds the start and end marker to this number and converts the random location string to the proper format (Bytes) using the encodeHighBytes function. The program now waits for a reply from the Arduino. When a reply has been sent, the script decodes the message by using the opposite function of the sendToArduino function, which is the recvFromArduino (6) function. To finally check whether the bitflip has caused an error in the Arduino RAM memory the content of the received message is checked using the errorcheck (7) function. In this function the housekeeping data that is saved on the Arduino's RAM memory is checked for correctness. When an error has been found the bitflip's memory location is stored using the analyse (8) function. After 'numLoops' have passed, the serial connection is closed. 


## Flight Software
The flight software consists of several modules which have to be checked by the error analysis software whether the software is working as intended. Below are listed four modules, which could be extended with more advanced satellite software as desired by the user.

### Watchdog timer
In order to verify whether the flight software is functioning as desired, a watchdog kicker is implemented in the flight software. The system is typically designed so that the watchdog timer will be kicked only if the computer considers the system functional. The computer determines whether the system is functional by conducting one or more fault detection tests and it will kick the watchdog only if all tests have passed. Whenever the Arduino fails to reset the watchdog timer, the board will reset. Before resetting, a set of actions can be defined to take place. One option for this would be to store some important parameters in the EEPROM memory so they can be used again after reboot.

### Housekeeping Data
The Arduino sends a housekeeping message to the computer over serial connection at set intervals. These housekeeping messages provides information to the error analysis software about the functionality of the software. Currently, three housekeeping modules are implemented, which can be extended at anytime. These are:
- Strings: our names: Frederic, Jochim, Bas and Alexander.
- Borwein π: The following simple algorithm approximates the value of 1/π in an iterative way. This Borwein algorithm has quartic convergence properties, such that with only a few iterations, a reasonable approximation is reached. 

    ![img](http://latex.codecogs.com/svg.latex?y_%7Bk%2B1%7D%3D%5Cfrac%7B1-%281-y_k%5E4%29%5E%7B1%2F4%7D%7D%7B1-%281%2By_k%5E4%29%5E%7B1%2F4%7D%7D)

    ![img](http://latex.codecogs.com/svg.latex?a_%7Bk%2B1%7D%26%3Da_k%281%2By_%7Bk%2B1%7D%29%5E4-2%5E%7B2k%2B3%7Dy_%7Bk%2B1%7D%281%2By_%7Bk%2B1%7D%2By_%7Bk%2B1%7D%5E2%29)

    The following initial conitions have to be used:

    - ![img](http://latex.codecogs.com/svg.latex?y_0%3D%5Csqrt%7B2%7D-1)
    - ![img](http://latex.codecogs.com/svg.latex?a_0%3D2%28%5Csqrt%7B2%7D-1%29%5E2)
### Reboot message
At booting, the Arduino sends out a message to the PC: "Arduino Reset".  This can be used to keep track of the amount of times the Arduino has rebooted.
### Visual check
The on-board LED is set to blink at set intervals to enable visual inspection of the correct functioning of the software.

Note: The time intervals are to be determined. The on-board watchdog timer on the Arduino board can be set to no more than 8000 ms, so the intervals mentioned above are at least shorter than 8000 ms.

## Running of the code
Below it can be found how the project can be build and used.

1. The software should be cloned/forked on your computer. This can be done using Git.
2. Install the Arduino IDE (https://www.arduino.cc/en/main/software).
3. Install Python 2.7 (https://www.python.org/download/releases/2.7/).
4. Connect the Arduino Uno and run the file "ArduinoPC.ino". Now the script is running on the Arduino.
5. Open the file "ComArduino.py".
6. Select the port that the Arduino is connected with. The name of the port can be found in the Arduino IDE. For the example below, this is "COM5".

![img](https://i.imgur.com/7hNDMa7.png)

7. Run the Python script (F5). The script should be working now and detect errors in the RAM memory.

## Results

The results of the program that was run can be seen in the figure below. The locations of the bit flips that did not cause an error have not been plotted since this was shown to be uniformly distributed over the memory map. In total 101 bit flips caused errors (yellow dots) of which 10 were hard errors (red dots) which caused the Arduino to crash. These errors occured during the process of inserting around 2000 randomly located bit flips to the Arduino's RAM memory. 

![img](https://i.imgur.com/nnuJHEN.png)

From this figure, no clear conclusion can be drawn in terms of what memory locations are most affected by the bit flips. It would be recommended to perform more iterations to investigate the most sensitive memory locations.

After having the results for the bit flips in various memory locations, limited time was used to investigate the reason why the microcontroller is crashing or stops working. One of the reasons why this happens is found to be a change in local variables. Since one of these local variables is changing, the script does not know how to react, which results in a crash. This can be seen in the figure below, where the final item can be observed to contain a second 'thorn' þ in the string. This phenomenon was observed in all hard errors. Since this character is also the start marker it is assumed the effect of having two start markers in the string causes the program to crash.

![img](https://i.imgur.com/htH8T1s.png)

The results from a limited run (+- 10000 iterations) can be found in the Errors.csv file, where the first column contains locations causing soft errors (errors in the housekeeping data) and the second column contains locations causing hard errors (reboot loops).

## Unit tests

### Bit flip

The verification of the bit flip can be found in simplebitflip.ino. In this file, the value 1000 is used to check the bit flip is working properly. The value 1000 in binary code is 1111101000. 

> (1000,BIN) = 1111101000

The normal bit flip function chooses a random bit location. However, for the sake of verification, a known bit is selected to be flipped. Below some examples are given. Note that counting starts from the end of the binary number (low bits). 

Changing the first bit (randI = 0) gives (thus changing 0 into 1):

> 1111101001 = 1001

Changing the fourth bit (randI = 3) gives (thus changing 1 into 0):

> 1111100000 = 992

Changing the seventh bit (randI = 6) gives (thus changing 1 into 0):

> 1110101000 = 936

From the above examples, it is clear that the bit flip is working as expected, since the bit flip for given bit locations is giving the expected result (eg. changing the fourth bit from 1 into 0 results in a subtraction of 2<sup>3</sup> = 8, thus 1000 - 8 = 992).

### Pi calculation

The verification of the calculation of π is done in Pi_Calculation.ino. π is calculated using the Borwein algorithm, which requires a small amount of iterations. For this verification, the number of iterations was set to 10.

Using the following initial conditions:

  - ![img](http://latex.codecogs.com/svg.latex?y_0%3D%5Csqrt%7B2%7D-1)
  - ![img](http://latex.codecogs.com/svg.latex?a_0%3D2%28%5Csqrt%7B2%7D-1%29%5E2)
  
π is then calculated using the algorithm discussed above.

> output = 3.141593 ≈ π

From the above result, it is clear the right number is obtained from the Borwein algorithm. Therefore, the π calculation is verified.

### Check memory address

The verification of the check for the memory address can be found in checkMemAddr.ino in the checkMemAddr folder. As input, the following memory address is used:

> char memAddressTry[] = "MA0276x";

which is according to the actual representation used in the main software. MA stands for memory address and the four following numbers represent the memory location. 'x' is used as terminator.

This checkIfMemAddress function is checking if a memory message is received and is giving the memory location as output. With the above input, the expected result is '276' as the memory location.

The output when running this unit test is:

> output = 276

With this result, it can be concluded the function is performing according to the expectations.

## Project complications

- Pointer, memory locations

Issues arose when trying to use the pointer with varying formats of the memory locations. At first it was tried to use the following format of memory location: 0x000F where the last three digits were determined at random. This however was proven unsuitable for use with the pointer. Therefore a different approach was used, where the Arduino's RAM memory boundaries were obtained in bytes (256 to 2303) and a random number between these values is chosen in the Python script and sent to the Arduino. This new approach was successful.

- Communication over Serial

The communication over serial was in times troublesome due to the way data has to be send over from Python to Arduino and vice versa. When viewing both scripts it can be seen that before data can be send a specific function is used to encode the data in the proper format, for example adding start and end markers to the string that is sent. Next to that, before the data can be read, it needs to be decoded again. It has taken some time to get used to the way data has to be sent and can be read afterwards in each respective programme. Alot of the problems that occurred were due to the fact that the program was first setup to read the incoming messages byte by byte. It would read one byte, then perform the rest of the main arduino loop, then read the next byte, etc. Once a complete message was received it would perform the bitflip. This made it impossible to know where errors came from and was changed to read the message in its entirety before executing the rest of the main loop.

- Types of variables

The types of variables that were used in Arduino were difficult to distinguish since it was not always clear what kind of variable was present and what kind of variable a specific function required. This was also unclear due to the unknown method of the communication over serial, as previously described.

- Reading of the data

In the later phase of the project, it was discovered that the programme was constantly reading one single byte using 'serial.read' and consequently sending the housekeeping data. Changing it to 'serial.readbytesuntil' fixed this issue. Now, the microcontroller first reads the entire message in one go before sending the housekeeping data back to the PC.

## Future work and recommendations

After working on this project some recommendations for future work can be made and are listed below.

- The programme should be tested using actual flight software. At the moment the software is running through some basic calculations, creating lists which makes up the memory that can be subjected to bitflips. Since these calculations are relatively simple the memory area that is vulnerable or subject to bitflips is limited. Running the programme on actual flight software will probably increase the likelihood of a memory location, where a bitflip is inflicted, containing information. This will in turn require the FDIR software to increase its capabilities of detecting errors caused by bitflips, therefore increasing the softwares capabilities.

- At the moment only error detection is performed while no error correction is done. In future software the ability to correct the errors caused by bitflips should be added since this is one of the requirements of FDIR software. The ability of the FDIR to correct detected errors is crucial for mission survival.

- Influence other memory types besides the Arduino's RAM memory. Next to the RAM memory which is the only memory currently subject to bitflips, future work should include the other memory types of the on-board computer that is used.

- Implementation of a SEU simulator. Right now the errors are generated at random. However, a more realistic SEU simulator would also model different types of radiation. If for example part of the memory would be affected, this could results in more severe damage.

- Analysing different data types, as for now only characters are used as data. 

- Implement the pingFromPC function. This would reset the watchdog timer on the Arduino only if the Python programme sends a specific 'kick' message. This can be used to reset the board from the python programme. 

- Perform more iterations in order to investigate the most sensitive memory locations in the RAM memory.
