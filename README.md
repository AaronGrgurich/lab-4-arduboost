# Lab 4: ArduBoost
This lab will apply concepts from the past 3 labs into developing a boost power supply controlled by an Arduino (codenamed ArduBoost).  This lab will also introduce you to the basics of power supply design and control theory.

More specifically, you will be using feedback from a rotary encoder to precisely regulate the output voltage of a boost power supply and display the regulated voltage on a 4 digit LCD screen.
To facilitate the efficient completion of this lab, it is split into 5 parts:
- Controlling the LED digits
- Reading an encoder
- Changing the PWM frequency
- Open loop PS control
- Closed loop PS control
Without further ado, let's get started!

## Controlling the LED digits
Each group is provided with a 4 digit, 7 segment, display.  The internal circuit diagram can be found in the datasheet http://cdn.sparkfun.com/datasheets/Components/LED/1LEDBLUCC.pdf

Notice that, looking at the circuit diagram, each 7 segment character is organized in a common cathode configuration (all of the cathodes are connected together).  Also notice that since each 7 segment character has its own common cathode, we can turn individual characters on by simply pulling the cathode for that specific digit to ground.  

You may remember that properly implementing an LED requires a current limiting resistor in series with the LED.  We will be placing current limiting resistors at the anode of each LED, because if we had current limiting resistors on the common cathode, turning on different numbers of segments would yield different intensities.  Note that if we pull the cathodes of two different characters to ground at the same time, this will dim both characters significantly.  We will avoid this later on.

Place the 4 digit LED on your breadboard and add current limiting resistors at each of the anodes.  Connect the common cathode of <b>one</b> of the digits to ground.  Experiment by connecting the current limiting resistors to 5V and seeing which segments turn on.

As you can see, 12 outputs are needed to control the 4 digits (8 for the segments and 4 for the digits).  The arduino has only 13 digital IO pins, so we shall seek to reduce the number of IO pins we use to control the LED.  One common device designed to reduce numbers of IO pins is a serial to parallel shift register.  This integrated circuit can read a series of 1s and 0s and convert it to a set of parallel outputs.  These chips are useful if lots of digital outputs are needed and refresh rates are not critical (it takes a lot of time to send that serial).

Simple operation of serial to parallel shift registers requires a 3 wire interface.  One wire is denote as the data pin, which sends out the serialized data to be written to the shift register outputs.  Another wire is the clock pin, which tells the shift register when new bits are being sent.  The third pin is the latch pin, which tells the shift register when a transmission starts and stops.  On our chip (SN74HC595N), the data pin is 14 (SER), the clock pin is 11 (SRCLK), and the latch pin is 12 (RCLK).

Place the shift register in the breadboard and connect pins 11,12, and 14 on the shift register to pins 10,11, and 12 on the Arduino, respectively.  They can actually be connected to any digital output pins you want because Arduino handles shift registers in software, so if you want to change it up, just don't connect any of those pins to digital pins 0,1, or 6.  

As you can see, this shift register has 8 outputs.  Our LED has 8 anodes.  What a coincidence!  Connect each of the 8 outputs to the 8 current limiting resistors.

<b>TL;DR</b> Set up this schematic: 

As for the code, let us first define how we write each character to the shift register.  For example, if QA - QH on the shift register were mapped to A - DP on the LED, the letter A would be written as <code>0x11101110</code> if we assume that we shift out our least significant bit (LSB) first.  Go ahead and define all of the numbers for now, as those are the only ones we'll be using in the lab.

But hold on, how are we going to display 4 digits when earlier I mentioned that turning on more than one digit would dim the others.  The solution is that we cycle through turning on the characters quickly.  Our eyes can easily be tricked that more than one digit is being turned on if we simply cycle through them, only turning one on at a time.  Go ahead and write a function and accompanying variables so that, when the function is called it writes out the next digit in a four digit number (hint, store the digit position as a global variable at the top of your code.  You can also store your four digit number as a global variable).

Finally, let's go and test it!  Write the loop code that prints out a number on the LEDs and cycles the digits roughly every 2 milliseconds.  Upload your code and watch it run or break (hopefully not).  
