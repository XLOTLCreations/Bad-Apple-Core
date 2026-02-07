***IF YOU SKIPPED OVER 'README' PLEASE READ FIRST FOR PROJECT INSIGHT***

Programmer Used:  Arduino Nano as ISP
Interface:  4x tactile switch push buttons 
            Up: D0 (PD2, physical pin 20)
            Down: A5 (PF0, physical pin 41, ADC0)
            Right: A4 (PF1, physical pin 40, ADC1)
            Left: D11 (PB7, physical pin 12)





Notes: 
A. ISP and ICSP are one and the same, ICSP is the physical header and ISP the protocol,
B. ISP First Boot @1MHz ~10-15 Seconds, any more time elapsed might hint at (but not necessarily) a faulty behavior,
C. In Device Manager 'Bad Apple Core' board will enumerate as 'Arduino Leonardo' once connected through USB post finishing of the steps in this Doc1 document.





Parts List:
Arduino NANO/UNO (for burning), respective Arduino model cable to connect to your host PC, small breadboard, jumper wires, 3x 1KOhm 1/4Watt 1% (preferred but not strict) and 3x 2KOhm 1/4Watt 1% (preferred but not strict) through hole resistors.





Steps:
1.0 - We need to create a 'Custom Board Definition' for our custom 'Bad Apple Core' board
1.1 - Locate your Arduino installation folder:
    Windows: `C:\Users\[YourName]\AppData\Local\Arduino15\`

1.2 - Navigate to: `packages/arduino/hardware/avr/[version]/`

1.3 - Edit `boards.txt` and add this at the end:
##############################################################
# Custom Atmega32U4 3.3V 8MHz
##############################################################

atmega32u4_33v8mhz.name=Atmega32U4 3.3V 8MHz

atmega32u4_33v8mhz.vid.0=0x2341
atmega32u4_33v8mhz.pid.0=0x8036
atmega32u4_33v8mhz.vid.1=0x2341
atmega32u4_33v8mhz.pid.1=0x0036

atmega32u4_33v8mhz.upload.tool=avrdude
atmega32u4_33v8mhz.upload.tool.default=avrdude
atmega32u4_33v8mhz.upload.protocol=avr109
atmega32u4_33v8mhz.upload.maximum_size=28672
atmega32u4_33v8mhz.upload.maximum_data_size=2560
atmega32u4_33v8mhz.upload.speed=57600
atmega32u4_33v8mhz.upload.disable_flushing=true
atmega32u4_33v8mhz.upload.use_1200bps_touch=true
atmega32u4_33v8mhz.upload.wait_for_upload_port=true

atmega32u4_33v8mhz.bootloader.tool=avrdude
atmega32u4_33v8mhz.bootloader.tool.default=avrdude
atmega32u4_33v8mhz.bootloader.low_fuses=0xFF
atmega32u4_33v8mhz.bootloader.high_fuses=0xD8
atmega32u4_33v8mhz.bootloader.extended_fuses=0xFE
atmega32u4_33v8mhz.bootloader.file=caterina/Caterina-promicro8.hex
atmega32u4_33v8mhz.bootloader.unlock_bits=0x3F
atmega32u4_33v8mhz.bootloader.lock_bits=0x2F

atmega32u4_33v8mhz.build.mcu=atmega32u4
atmega32u4_33v8mhz.build.f_cpu=8000000L
atmega32u4_33v8mhz.build.vid=0x2341
atmega32u4_33v8mhz.build.pid=0x8036
atmega32u4_33v8mhz.build.usb_product="Atmega32U4 3.3V"
atmega32u4_33v8mhz.build.board=AVR_LEONARDO
atmega32u4_33v8mhz.build.core=arduino
atmega32u4_33v8mhz.build.variant=leonardo
atmega32u4_33v8mhz.build.extra_flags={build.usb_flags}


1.4 - Most important customization is this HEX file included in the project files by the name 'Caterina-promicro8.hex' the necessary bootloader for our board's hardware configuration i.e. 3.3V @8MHz clock by SparkFun.
Navigate to ->
Arduino15\packages\arduino\hardware\avr\1.8.7\bootloaders\caterina
and paste this hex file in the 'caterina' folder along with all the others.
If you had the Arduino IDE open this is a good time to close all open tabs and relaunch the app to include all the newly added files.




2.0 - Setting up 'Arduino NANO' (UNO too will work) as ISP programmer

2.1 - Upload Arduino Nano/Uno with ArduinoISP Sketch (modified for slow clock)
      Open Arduino IDE then File → Examples → ArduinoISP → ArduinoISP

2.2 - Find this line: 
      #define SPI_CLOCK (1000000/6) 
      Leave the above line intact for 1MHz clock speed bootloader burning. 

      In case this speed does not work for you make the edit: 
      #define SPI_CLOCK (500000) 
      for 500kHz upload speed instead. (use if your ISP interconnect wires are very long ~1/2 - 1 foot, also note the burn time will double up to ~20 to 30 sec)

2.3 - Select: Tools → Board → Arduino Nano
      Select: Tools → Processor → ATmega328P
      Upload the sketch to Nano.
  



3.0 - Interface 'Arduino Nano' (ISP Master) and 'Bad Apple Core' (ISP Slave)
3.1 - Disconnect Arduino NANO from host computer (NANO OFF/unpowered) before proceeding to the step3.2,

3.2 - ***FAILING TO ADHERE TO FOLLOWING INSTRUCTIONS WILL LIKELY RESULT IN IRREPARABLE BOARD DAMAGE AND POTENTIALLY BRICK THE DEVICE***
***!!! LEVEL SHIFT !!! THIS STEP IS PARAMOUNT***
5V to 3.3V signal level shifting using 1K-2K resistor divider for SCK, MOSI and RST signals.
Directly wire MISO and GND **do not level shift**. (refer to respective Arduino model pinout diagram online) Referring to the provided wiring diagram by the name 'Level Shifting' in project files will help a ton here.

3.3 - Once breadboarded the setup for the three required signals connect the Arduino NANO to host PC and select the appropriate COM port that enumerates with the Arduino model name. (not to confuse with point C in the notes section)




4.0 - Burn the Bootloader with the following Critical Settings in Arduino IDE:

4.1 - Now also connect 'Bad Apple Core' board to USB-C to power it on using either a wall adapter or host PC.
Which means now both Arduino NANO/UNO (ISP master connected to host PC) and Bad Apple Core (ISP slave) are simultaneously connected and therefore powered on.
Tools → Board: "Atmega32U4 3.3V 8MHz" (this is our Bad Apple Core now)
Tools → Programmer: "Arduino as ISP" (not "ArduinoISP.org"!)
Tools → Port: Select your Nano's COM port
Tools → Burn Bootloader

4.2 - Burn Process Serial Monitor Output @1MHz, wait ~10-15 seconds - you should see:
avrdude: verifying ...
avrdude: 1 bytes of efuse verified
avrdude: 1 bytes of hfuse verified
avrdude: 1 bytes of lfuse verified

avrdude done. Thank you.

5.1 - Troubleshooting:
"Invalid device signature" → Further slow down ISP clock to 250kHz in ArduinoISP sketch,
"Not in sync" → Check wiring, especially RESET connection,
"Property undefined" → Verify boards.txt was edited correctly and IDE restarted,

5.2 - USB Not Enumerating (Code 43)
Issue: Windows shows "Device Descriptor Request Failed"
Solution:
Wrong bootloader (16MHz on 8MHz system)
Download correct Caterina-promicro8.hex
Re-burn bootloader




After Arduino IDE prompts at successful bootloader burning, disconnect the wiring from ICSP header.

Bad Apple Core custom bootloader burn process DONE!

Next, connect the 'Bad Apple Core' board using USB-C cable to your host PC (Arduino Nano as ISP burner role completed, keep it aside) and check Win+X 'Device Manager' the 'Bad Apple Core' board should enumerate as 'Arduino Leonardo'.
This confirms all steps mentioned in this document to be successful and now we can run test sketches to verify connection integrity with all the peripherals onboard in the rare case of faulty/erroneous board assembly from the manufacturer or user mishandling scenario.


