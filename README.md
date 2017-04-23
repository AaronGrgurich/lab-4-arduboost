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

<b>Place the 4 digit LED on your breadboard and add current limiting resistors at each of the anodes.  Connect the common cathode of ONE of the digits to ground.  Experiment by connecting the current limiting resistors to 5V and seeing which segments turn on.</b>

As you can see, 12 outputs are needed to control the 4 digits (8 for the segments and 4 for the digits).  The arduino has only 13 digital IO pins, so we shall seek to reduce the number of IO pins we use to control the LED.  One common device designed to reduce numbers of IO pins is a serial to parallel shift register.  This integrated circuit can read a series of 1s and 0s and convert it to a set of parallel outputs.  These chips are useful if lots of digital outputs are needed and refresh rates are not critical (it takes a lot of time to send that serial).

Simple operation of serial to parallel shift registers requires a 3 wire interface.  One wire is denote as the data pin, which sends out the serialized data to be written to the shift register outputs.  Another wire is the clock pin, which tells the shift register when new bits are being sent.  The third pin is the latch pin, which tells the shift register when a transmission starts and stops.  On our chip (SN74HC595N), the data pin is 14 (SER), the clock pin is 11 (SRCLK), and the latch pin is 12 (RCLK).

<b>Place the shift register in the breadboard and connect pins 11,12, and 14 on the shift register to pins 10,11, and 12 on the Arduino, respectively.</b>  They can actually be connected to any digital output pins you want because Arduino handles shift registers in software, so if you want to change it up, just don't connect any of those pins to digital pins 0,1,2,3 or 6.  

As you can see, this shift register has 8 outputs.  Our LED has 8 anodes.  What a coincidence!  <b>Connect each of the 8 outputs to the 8 current limiting resistors.</b>

Now, to handle the 4 different digits.  As I mentioned earlier, if we connected multiple cathodes to ground the light output of each segment would drop due to increased current through the current limiting resistors.  We are going to mitigate this by cycling through each character one by one quickly.  Our eyes can easily be tricked that more than one digit is being turned on if we simply cycle through them, only turning one on at a time.  Grab four 2n4401 transistors from the lab and hook them up so that the emitter is to ground, the collector is to the cathodes of the LED, and the base is connected to four digital pins (not 0,1,2,3 or 6) through a base resistor (4.7k ohms).  When the digial pins are set to HIGH, they will drive the base of the transistor and pull current through the LEDs of that digit, allowing you to turn each digit on individually.  The reason that we don't use the arduino pins for this is because they can only sink ~20mA of current.

<b>TL;DR Set up this schematic:</b> 

As for the code, let us first define how we write each character to the shift register.  For example, if QA - QH on the shift register were mapped to A - DP on the LED, the letter A would be written as <code>0x11101110</code> if we assume that we shift out our least significant bit (LSB) first.  To make your lives easier, I've written the map from character to segment representation for you already: 

~~~~
byte mapseg[36] = {
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100000, // 7
  0b11111110, // 8
  0b11100110, // 9
  0b11101110, // A
  0b00111110, // b
  0b10011100, // C
  0b01111010, // d
  0b10011110, // E
  0b10001110, // F
  0b11110110, // g
  0b01101110, // H
  0b00001100, // I
  0b01111000, // J
  0b01101110, // K
  0b00011100, // L
  0b10101000, // M
  0b00101010, // N
  0b11111100, // O
  0b11001110, // P
  0b11100110, // Q
  0b00001010, // R
  0b10110110, // S
  0b00011110, // t
  0b01111100, // U
  0b00111000, // v
  0b01010100, // W
  0b01101110, // X
  0b01110110, // Y
  0b11011010 // Z
};
~~~~

<b>Write a method that takes in a character and a digit number and uses the shift register and the transistors to write the character to the proper digit.</b>  Test this by trying to print the character '5' to the first digit. 

<b>Then, write a method that cycles through the digits of a four letter set of characters and writes each one to the proper position on the LCD segments.</b>  One design might be to have this event called frequently from the loop() method and each call it increments one digit.

Finally, let's go and test it!  Upload your code and watch it run or break (hopefully not).  

## Reading an Encoder
Due to unforseen issues with the interrupts on the arduino uno, we will not be using encoders today.  Instead, we will be using a potentiometer.  

Hopefully you all have used potentiometers before, <b>grab a potentiometer from the lab and hook it up with the swiper to an analog input pin.</b>  This potentiometer will be used to control the target voltage of the boost converter we'll be controlling later.

## Changing the PWM frequency
In order to use our boost converter properly, we will have to change the PWM frequency of our Arduino.  The default frequency is ~490Hz which is way too low to operate a switching power supply on.  In short, inductors would have to be massive.  PWM frequency is, however, easily changed on the Arduino.  

Each PWM pin is slightly different, and it turns out the the pin with the highest achievable PWM frequency is pin 6.  PWM's are controlled by timer/counter control registers.  The register that controls pins 5 and 6 is TCCR0B, which stands for the second Timer/Counter Control Register for Timer 0.  Anyway, all that we need to do is change the clock scaling by setting the last 3 bits to 001.  However, we need to keep the other bits set to their current values, so here's a cheat code for doing this:
<code>
TCCR0B = TCCR0B & 0b11111000 | 0x01;
</code>

<b>Write the code to change the PWM frequency and output a square wave (half duty cycle) on pin 6.  Have a TA come and check your signal before you proceed.</b>  A different operating frequency could cause overheating in the power supply and parts to explode.

## Open-loop PS control
Once your frequency has been verified by a TA, you are ready to get started with the Arduboost!  If you are curious, the circuit is essentially the same circuit as we have below but with the switch replaced by a logic level mosfet:

![Alt](https://upload.wikimedia.org/wikipedia/commons/thumb/0/09/Boost_operating.svg/250px-Boost_operating.svg.png "title")

The basic operation is that it shorts the inductor which builds up a magnetic field in its core which it then stacks on top of the input voltage to create a higher voltage.  Depending on how long the inductor is shorted, the output voltage will change.  In other words, if we increase the PWM duty cycle, the output voltage will increase.  <b>Before we get started, though, set your PWM duty cycle to around 5-10% (analogWrite of 20), so that we don't break anything off the bat.</b>

<b>Note: the maximum analogWrite value that you should ever send is 80!  Please don't go above that!</b>  Actually, how about we go an write a method that sets our analog voltage but keeps us within our limits.  For example, it could be something like: <code>analogWrite(6,max(min(val,80),0));</code>

<b>Let's get started by hooking up the ground, power, and pwm signal to the Arduboost (make sure that your arduino is off first).  Confirm that you did low the duty cycle of the pwm, and then power on the Arduino.  Attach a probe between the output of the Arduboost and ground and observe the voltage on the scope.</b>  If everythinng is working, you should have a DC value of somewhere above 10V.

<b>Try doubling the duty cycle in your code and see what happens to the output voltage.</b>

<b>Now, use the analog input voltage from your potentiometer to automatically set the duty cycle.</b>  Keep in mind that the maximum pwm value you must ever set is 80, so you will need to scale the raw data down to utilize the full sweep of the pot.

## Closed-loop PS control
Now for closed loop control.  Hook up the output voltage of the boost converter to a voltage divider that brings the output voltage to 0-5V and hook up the middle of that divider to an analog input pin.  Now we have feedback!  Instead of guessing a PWM frequency for a given voltage output, we can adjust the output and look at the feedback until it's what we want.  

The easiest form of control is a proportional controller.  It short, if the measured value is less than the target value then increase the output, if the measured value is more than the target value then decrease the output.  <b>Implement a proportional controller that converges the output voltage around a specified voltage.</b>  Notice that it probably overshoots your voltage first and then undershoots and overshoots and so on until it stabilizes.  We can stop some of this by adding a deadband, aka a range of values around the target value for which we say that the output is fine.  For example, if our target is 30V and we it 29.5, we can call that fine.  <b>Implement a deadband on this controller and observe the change in response</b>

Look at that, you have an adjustable power supply!  Now let's put the cherry on top by writing the output voltage on that 4 digit display we made earlier.  <b>Determine the ratio between the feedback value and the output voltage and then display the output voltage on the display.</b>  If you want a decimal after a digit, you must set the 8th bit to 1 (bitwise or by 0x01).  
