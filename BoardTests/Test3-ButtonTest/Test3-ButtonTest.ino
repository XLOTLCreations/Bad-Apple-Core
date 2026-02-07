/*
 * Button Test + VBUS Monitor
 * Tests 4 directional buttons and USB voltage monitoring
 * 
 * Buttons (Active LOW with 100nF debounce caps):
 * - Up:    D0  (PD2)
 * - Down:  A5  (PF0, ADC0)
 * - Right: A4  (PF1, ADC1)
 * - Left:  D11 (PB7)
 * 
 * VBUS Monitor: A0 (PF7, ADC7)
 * - Rtop=5.6K, Rbot=10K, 100nF filter, 1K series to ADC
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>

// Pin definitions
#define BTN_UP    0   // D0
#define BTN_DOWN  A5  // A5 (PF0)
#define BTN_RIGHT A4  // A4 (PF1)
#define BTN_LEFT  11  // D11
#define VBUS_MON  A0  // A0 (PF7, ADC7)
#define SDB_PIN   9   // IS31FL3731 shutdown

Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

void setup() {
  // Enable IS31FL3731
  pinMode(SDB_PIN, OUTPUT);
  digitalWrite(SDB_PIN, HIGH);
  delay(5);
  
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== Button Test + VBUS Monitor ===");
  Serial.println();
  
  // Setup buttons with internal pull-ups (active LOW)
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  
  Serial.println("Buttons configured (active LOW with pull-ups)");
  Serial.println("- Up:    D0");
  Serial.println("- Down:  A5");
  Serial.println("- Right: A4");
  Serial.println("- Left:  D11");
  Serial.println();
  
  // Initialize LED matrix
  if (!matrix.begin(0x74)) {
    Serial.println("IS31FL3731 not found!");
    while (1);
  }
  
  matrix.clear();
  Serial.println("LED Matrix ready!");
  Serial.println();
  
  // Test VBUS monitoring
  Serial.println("=== VBUS Monitor Test ===");
  float vbus = readVBUS();
  Serial.print("USB Voltage: ");
  Serial.print(vbus);
  Serial.println(" V");
  
  if (vbus < 4.5) {
    Serial.println("⚠ WARNING: Low USB voltage detected!");
    Serial.println("  Consider dimming LEDs to prevent brownout");
  } else if (vbus > 5.5) {
    Serial.println("⚠ WARNING: High USB voltage!");
  } else {
    Serial.println("✓ USB voltage normal");
  }
  
  Serial.println();
  Serial.println("=== Button Test ===");
  Serial.println("Press buttons to see response!");
  Serial.println("Each button press will light up a direction arrow on the matrix");
  Serial.println();
}

// Read VBUS voltage
// Voltage divider: Rtop=5.6K, Rbot=10K
// Vout = Vin * (Rbot / (Rtop + Rbot))
// Vout = Vin * (10K / 15.6K) = Vin * 0.641
// Therefore: Vin = Vout / 0.641 = Vout * 1.560
float readVBUS() {
  int adcValue = analogRead(VBUS_MON);
  float vout = (adcValue / 1023.0) * 3.3; // ADC reading to voltage
  float vin = vout * 1.560; // Voltage divider ratio
  return vin;
}

// Draw arrow on LED matrix
void drawArrow(char direction) {
  matrix.clear();
  
  switch(direction) {
    case 'U': // Up arrow
      matrix.drawPixel(6, 1, 200);
      matrix.drawPixel(5, 2, 200);
      matrix.drawPixel(6, 2, 200);
      matrix.drawPixel(7, 2, 200);
      matrix.drawPixel(6, 3, 200);
      matrix.drawPixel(6, 4, 200);
      matrix.drawPixel(6, 5, 200);
      break;
      
    case 'D': // Down arrow
      matrix.drawPixel(6, 3, 200);
      matrix.drawPixel(6, 4, 200);
      matrix.drawPixel(6, 5, 200);
      matrix.drawPixel(6, 6, 200);
      matrix.drawPixel(5, 6, 200);
      matrix.drawPixel(6, 6, 200);
      matrix.drawPixel(7, 6, 200);
      matrix.drawPixel(6, 7, 200);
      break;
      
    case 'L': // Left arrow
      matrix.drawPixel(2, 4, 200);
      matrix.drawPixel(3, 3, 200);
      matrix.drawPixel(3, 4, 200);
      matrix.drawPixel(3, 5, 200);
      matrix.drawPixel(4, 4, 200);
      matrix.drawPixel(5, 4, 200);
      matrix.drawPixel(6, 4, 200);
      break;
      
    case 'R': // Right arrow
      matrix.drawPixel(5, 4, 200);
      matrix.drawPixel(6, 4, 200);
      matrix.drawPixel(7, 4, 200);
      matrix.drawPixel(8, 3, 200);
      matrix.drawPixel(8, 4, 200);
      matrix.drawPixel(8, 5, 200);
      matrix.drawPixel(9, 4, 200);
      break;
  }
}

void loop() {
  static unsigned long lastPrint = 0;
  static bool lastUp = HIGH;
  static bool lastDown = HIGH;
  static bool lastRight = HIGH;
  static bool lastLeft = HIGH;
  
  // Read buttons (active LOW, so LOW = pressed)
  bool up = digitalRead(BTN_UP);
  bool down = digitalRead(BTN_DOWN);
  bool right = digitalRead(BTN_RIGHT);
  bool left = digitalRead(BTN_LEFT);
  
  // Detect button press (transition from HIGH to LOW)
  if (up == LOW && lastUp == HIGH) {
    Serial.println("⬆ UP pressed");
    drawArrow('U');
  }
  if (down == LOW && lastDown == HIGH) {
    Serial.println("⬇ DOWN pressed");
    drawArrow('D');
  }
  if (right == LOW && lastRight == HIGH) {
    Serial.println("➡ RIGHT pressed");
    drawArrow('R');
  }
  if (left == LOW && lastLeft == HIGH) {
    Serial.println("⬅ LEFT pressed");
    drawArrow('L');
  }
  
  // Save button states
  lastUp = up;
  lastDown = down;
  lastRight = right;
  lastLeft = left;
  
  // Print VBUS voltage every 5 seconds
  if (millis() - lastPrint > 5000) {
    float vbus = readVBUS();
    Serial.print("VBUS: ");
    Serial.print(vbus);
    Serial.println(" V");
    lastPrint = millis();
  }
  
  delay(10); // Small delay for debouncing (hardware caps help too!)
}
