**\*\*\*IF YOU SKIPPED OVER 'README' PLEASE READ FIRST FOR PROJECT INSIGHT\*\*\***



Assuming the board is loaded with the correct bootloader, therefore this document onwards one does not require any other tool except their personal computer (host computer) and the 'Bad Apple Core' board itself.

Now, the only thing one needs to do to start with coding this board is to just create a custom board definition. To do that all you need to do is add the following custom board definition into the Arduino IDE.





Steps:

1.1 - Locate your Arduino installation folder:

&nbsp;   Windows: `C:\\Users\\\[YourName]\\AppData\\Local\\Arduino15\\`



1.2 - Navigate to: `packages/arduino/hardware/avr/\[version]/`



1.3 - Edit `boards.txt` and add this at the end:



\##############################################################

\# Custom Atmega32U4 3.3V 8MHz

\##############################################################



atmega32u4\_33v8mhz.name=Atmega32U4 3.3V 8MHz



atmega32u4\_33v8mhz.vid.0=0x2341

atmega32u4\_33v8mhz.pid.0=0x8036

atmega32u4\_33v8mhz.vid.1=0x2341

atmega32u4\_33v8mhz.pid.1=0x0036



atmega32u4\_33v8mhz.upload.tool=avrdude

atmega32u4\_33v8mhz.upload.tool.default=avrdude

atmega32u4\_33v8mhz.upload.protocol=avr109

atmega32u4\_33v8mhz.upload.maximum\_size=28672

atmega32u4\_33v8mhz.upload.maximum\_data\_size=2560

atmega32u4\_33v8mhz.upload.speed=57600

atmega32u4\_33v8mhz.upload.disable\_flushing=true

atmega32u4\_33v8mhz.upload.use\_1200bps\_touch=true

atmega32u4\_33v8mhz.upload.wait\_for\_upload\_port=true



atmega32u4\_33v8mhz.bootloader.tool=avrdude

atmega32u4\_33v8mhz.bootloader.tool.default=avrdude

atmega32u4\_33v8mhz.bootloader.low\_fuses=0xFF

atmega32u4\_33v8mhz.bootloader.high\_fuses=0xD8

atmega32u4\_33v8mhz.bootloader.extended\_fuses=0xFE

atmega32u4\_33v8mhz.bootloader.file=caterina/Caterina-promicro8.hex

atmega32u4\_33v8mhz.bootloader.unlock\_bits=0x3F

atmega32u4\_33v8mhz.bootloader.lock\_bits=0x2F



atmega32u4\_33v8mhz.build.mcu=atmega32u4

atmega32u4\_33v8mhz.build.f\_cpu=8000000L

atmega32u4\_33v8mhz.build.vid=0x2341

atmega32u4\_33v8mhz.build.pid=0x8036

atmega32u4\_33v8mhz.build.usb\_product="Atmega32U4 3.3V"

atmega32u4\_33v8mhz.build.board=AVR\_LEONARDO

atmega32u4\_33v8mhz.build.core=arduino

atmega32u4\_33v8mhz.build.variant=leonardo

atmega32u4\_33v8mhz.build.extra\_flags={build.usb\_flags}









Save and quit the text file, also make sure you quit Arduino IDE by closing all it's instances open on your computer for the new board definition to load at re-launch.

Once done, re-launch your Arduino IDE as it is now ready to communicate with 'Bad Apple Core'. Just make sure you select 'Atmega32U4 3.3V 8MHz' from the drop down list of available boards from 'tools'.

