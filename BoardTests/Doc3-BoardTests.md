\*\*\*IF YOU SKIPPED OVER 'README' PLEASE READ FIRST FOR PROJECT INSIGHT\*\*\*



The tests described in this doc are not strictly necessary, although they will come handy while troubleshooting the board, in which case the user might want to test all the subsystems individually to rule out any physical malfunctions or incurred damages to the board. This includes any under chip damages and therefore invisible.



1.0 - LED Blink Sketch:

      'Bad Apple Core' has a board health/sanity test Red LED on Digital pin#6.



1.1 - In Arduino IDE:

Tools → Board: "Atmega32U4 3.3V 8MHz"

Tools → Port: COM10 (or whichever COM port it enumerates as, this is your newly flashed Bad Apple Core board)



 void setup() {

     pinMode(6, OUTPUT);

   }



   void loop() {

     digitalWrite(6, HIGH);

     delay(300);

     digitalWrite(6, LOW);

     delay(300);

   }



1.2 - Copy paste the above sketch in Arduino IDE, compile and upload.

The LED should blink at 300msec on/off rate.
First signs of life...





2.0 - Move onto testing Matrix driver with some LED Matrix pattern sketches included. Start by searching for and installing 'Adafruit IS31FL3731 Library' and 'Adafruit GFX Library'. Note that LED Matrix sub-system features a user accessible active-high shutdown control on digital pin#9, if LED matrix ever flickers during large file uploads to and from the Flash Memory onboard (unlikely but not impossible).





3.0 - Next we will attempt to establish communication with the onboard 16MByte NOR Flash storage. Massive amount of storage for this board capability, memory for days...

3.1 - Start by installing SPIMemory library in Arduino IDE:



Arduino IDE → Sketch → Include Library → Manage Libraries

Search: "SPIMemory"

Install: "SPIMemory by Prajwal Bhattaram (Marzogh)"



Now go ahead and upload the included Flash test sketch while reading the Serial Monitor Output for confirmation. The output should look like:

=== Flash Test Complete ===

Flash is ready for video storage!





4.0 - Last and Final we will check the four push-buttons with the included sketch.





5.0 - As a bonus feature and completely optional, the design features USB bus voltage monitoring on the pin# ADC7 Analog Input 0. This feature comes in handy when dealing with a very weak PC USB port. User can utilize this to frequently monitor the USB bus voltage droop to not overload the PC USB port and maybe cause any very unlikely damages to the host PC by modifying the LED Matrix brightness to a safer level.

Ratio: Vmeasured = Vbus × 0.641, where Vbus = 5.0 - 5.1 Volts.





Hence concludes all our board tests...!

