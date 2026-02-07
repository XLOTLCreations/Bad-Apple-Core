/*
 * Screen Mirror Display - Arduino Side
 * Receives 12x9 grayscale frames via Serial from PC
 * Displays in real-time on LED matrix
 * 
 * Protocol:
 * - Start byte: 0xFF
 * - 108 bytes of pixel data (12x9, row-major order)
 * - Each byte = brightness 0-255
 * 
 * Baud rate: 115200 (can handle ~30fps)
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>

#define SDB_PIN 9
#define GRID_WIDTH 12
#define GRID_HEIGHT 9
#define FRAME_SIZE 108  // 12 * 9

Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

uint8_t frameBuffer[FRAME_SIZE];
uint16_t frameCount = 0;
unsigned long lastFrameTime = 0;
unsigned long fpsTimer = 0;
uint16_t framesPerSecond = 0;

void setup() {
  // Enable IS31FL3731
  pinMode(SDB_PIN, OUTPUT);
  digitalWrite(SDB_PIN, HIGH);
  delay(5);
  
  Serial.begin(115200);
  Serial.setTimeout(100);
  
  // Initialize LED matrix
  if (!matrix.begin(0x74)) {
    // Can't use Serial here as Python is connected
    while (1);
  }
  
  matrix.clear();
  
  // Show "ready" pattern - single pixel in center
  matrix.drawPixel(6, 4, 100);
  delay(500);
  matrix.clear();
  
  lastFrameTime = millis();
  fpsTimer = millis();
}

void loop() {
  // Wait for start byte (0xFF)
  if (Serial.available() > 0) {
    uint8_t startByte = Serial.read();
    
    if (startByte == 0xFF) {
      // Read 108 bytes of frame data
      size_t bytesRead = Serial.readBytes(frameBuffer, FRAME_SIZE);
      
      if (bytesRead == FRAME_SIZE) {
        // Display frame
        displayFrame();
        
        // Update statistics
        frameCount++;
        framesPerSecond++;
        
        // Calculate FPS every second
        if (millis() - fpsTimer >= 1000) {
          // Could send FPS back to PC if needed
          // Serial.print("FPS:");
          // Serial.println(framesPerSecond);
          framesPerSecond = 0;
          fpsTimer = millis();
        }
        
        lastFrameTime = millis();
      }
    }
  }
  
  // Timeout detection - clear display if no frames for 2 seconds
  if (millis() - lastFrameTime > 2000) {
    matrix.clear();
    // Show "waiting" indicator - blinking center pixel
    if ((millis() / 500) % 2 == 0) {
      matrix.drawPixel(6, 4, 50);
    }
  }
}

void displayFrame() {
  matrix.clear();
  
  uint8_t index = 0;
  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    for (uint8_t x = 0; x < GRID_WIDTH; x++) {
      uint8_t brightness = frameBuffer[index++];
      if (brightness > 0) {  // Skip writing 0s for efficiency
        matrix.drawPixel(x, y, brightness);
      }
    }
  }
}
