/*
 * Scrolling Text on 12x9 LED Matrix
 * Displays "Hello World" with smooth horizontal scrolling
 * 
 * The Adafruit GFX library provides built-in text rendering
 * with 5x7 pixel font (perfect for our 9-pixel height!)
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>

#define SDB_PIN 9

Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

// Text to display
const char* message = "Hello World!";

// Scrolling parameters
int16_t textX = 12;  // Start off-screen to the right
int16_t textY = 1;   // Vertical position (0-8, we use 1 for slight padding)
uint8_t textBrightness = 30;  // LED brightness (0-255)
uint16_t scrollSpeed = 100;    // Milliseconds between scroll steps (lower = faster)

unsigned long lastScrollTime = 0;

void setup() {
  // Enable IS31FL3731
  pinMode(SDB_PIN, OUTPUT);
  digitalWrite(SDB_PIN, HIGH);
  delay(5);
  
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== Scrolling Text Demo ===");
  Serial.print("Message: ");
  Serial.println(message);
  Serial.println();
  
  // Initialize LED matrix
  if (!matrix.begin(0x74)) {
    Serial.println("IS31FL3731 not found!");
    while (1);
  }
  
  matrix.clear();
  
  // Set text properties
  matrix.setTextSize(1);      // 1:1 pixel scale
  matrix.setTextWrap(false);  // Don't wrap at edge
  matrix.setTextColor(textBrightness);
  
  Serial.println("Scrolling started!");
}

void loop() {
  // Scroll at controlled speed
  if (millis() - lastScrollTime >= scrollSpeed) {
    // Clear display
    matrix.clear();
    
    // Draw text at current position
    matrix.setCursor(textX, textY);
    matrix.print(message);
    
    // Move text left
    textX--;
    
    // Calculate text width (approximately 6 pixels per character with default font)
    int16_t textWidth = strlen(message) * 6;
    
    // Reset position when text scrolls completely off screen
    if (textX < -textWidth) {
      textX = 12;  // Start from right edge again
    }
    
    lastScrollTime = millis();
  }
  
  delay(10);  // Small delay to prevent overwhelming the I2C bus
}
