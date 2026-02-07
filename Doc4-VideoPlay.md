\*\*\*IF YOU SKIPPED OVER 'Doc0' PLEASE READ FIRST FOR PROJECT INSIGHT\*\*\*



I have provided two video related examples to demonstrate key possibilities with this project and to get started impressing your friends.



Titled 'BadApplePlayback' and 'ScreenMirror' does what they exactly say.





BadApplePlayback:

* Example comes with two files, the Arduino Sketch accompanied with its associated python script.
* Upload the 'BadApplePlayback.ino' Arduino sketch as usual to the board, this sketch is what instructs the MCU to read from the USB and write to the Flash simultaneously effectively storing the video in the Flash memory.
* Next, open your terminal (Win+R -> cmd) in the same folder as you saved all these files i.e your python code and the 'badapple\_12x9\_30fps\_8bit.bin' raw binary video file, 
* Download and install following python libraries/dependencies: pillow, mss, numpy and pyserial, you can use a single command to install them all 'pip install pillow mss pyserial numpy'.
* Type in or copy paste 'python send\_bin.py COM12 badapple\_12x9\_30fps\_8bit.bin' in the terminal, change the 'COM12' to whatever COM port your board enumerates to.



You should see a smooth display of 'Bad Apple' at 30FPS. 





ScreenMirror:

* Since we installed: pillow, mss, numpy and pyserial in the last example we can skip this step,
* Upload the 'ScreenMirror.ino' sketch to the board, you can edit 'matrix.drawPixel(x, y, brightness / 2);  // Dim by 50%' line in the sketch to desired LED Matrix brightness. Once uploaded it is best to quit all open 'Arduino IDE' instances or at least make sure the serial monitor is not open and reading the COM port. Board blinks with center pixel when waiting for frame reception from host PC.
* Launch cmd in this folder containing the 'screen\_mirror.py' Python file and in terminal type in the command 'python screen\_mirror.py COM12'. Change COM12 with your enumerated COM port.

&nbsp;You can play with the python code parameters such as:
 CAPTURE\_REGION = {'left': 100, 'top': 100, 'width': 800, 'height': 600}

&nbsp;TARGET\_FPS = 20  # Lower for stability, higher for smoothness

&nbsp;CAPTURE\_REGION = {'left': 0, 'top': 0, 'width': 960, 'height': 540} #Top-left quadrant on a 1920×1080 monitor/display

&nbsp;CAPTURE\_REGION = {'left': 460, 'top': 240, 'width': 1000, 'height': 600} #Screen center capture on a 1920×1080 monitor/display

