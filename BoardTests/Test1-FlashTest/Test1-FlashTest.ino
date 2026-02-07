/*
 * W25Q128JVP Flash Test
 * Tests 16MB NOR Flash for video frame storage
 * CS pin: PB0 (Physical pin 8, Arduino SS/RXLED)
 */

#include <SPIMemory.h>

#define FLASH_CS SS  // PB0 - Arduino's SS pin
#define SDB_PIN 9    // IS31FL3731 shutdown pin

SPIFlash flash(FLASH_CS);

void setup() {
  // Enable IS31FL3731 (so it doesn't interfere)
  pinMode(SDB_PIN, OUTPUT);
  digitalWrite(SDB_PIN, HIGH);
  delay(5);
  
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== W25Q128JVP Flash Memory Test ===");
  Serial.println();
  
  // Initialize Flash
  Serial.print("Initializing Flash... ");
  if (flash.begin()) {
    Serial.println("SUCCESS!");
  } else {
    Serial.println("FAILED!");
    Serial.println("Check:");
    Serial.println("- CS pin connected to PB0?");
    Serial.println("- MOSI/MISO/SCK connected?");
    Serial.println("- Flash powered at 3.3V?");
    while (1);
  }
  
  Serial.println();
  
  // Get Flash info
  Serial.println("=== Flash Information ===");
  
  uint32_t jedecID = flash.getJEDECID();
  Serial.print("JEDEC ID: 0x");
  Serial.println(jedecID, HEX);
  
  uint32_t capacity = flash.getCapacity();
  Serial.print("Capacity: ");
  Serial.print(capacity);
  Serial.print(" bytes (");
  Serial.print(capacity / 1024 / 1024);
  Serial.println(" MB)");
  
  uint16_t maxPages = flash.getMaxPage();
  Serial.print("Pages: ");
  Serial.println(maxPages);
  
  Serial.println();
  
  // Test write/read
  Serial.println("=== Write/Read Test ===");
  
  uint32_t testAddr = 0x1000; // Test at address 4096
  uint8_t writeData[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                           0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  uint8_t readData[16];
  
  Serial.print("Writing 16 bytes to address 0x");
  Serial.print(testAddr, HEX);
  Serial.print("... ");
  
  if (flash.writeByteArray(testAddr, writeData, 16)) {
    Serial.println("SUCCESS!");
  } else {
    Serial.println("FAILED!");
  }
  
  Serial.print("Reading back... ");
  if (flash.readByteArray(testAddr, readData, 16)) {
    Serial.println("SUCCESS!");
  } else {
    Serial.println("FAILED!");
  }
  
  // Verify
  Serial.print("Verification: ");
  bool match = true;
  for (uint8_t i = 0; i < 16; i++) {
    if (writeData[i] != readData[i]) {
      match = false;
      break;
    }
  }
  
  if (match) {
    Serial.println("✓ PASSED! Data matches!");
  } else {
    Serial.println("✗ FAILED! Data mismatch!");
    Serial.print("Written: ");
    for (uint8_t i = 0; i < 16; i++) {
      Serial.print("0x");
      if (writeData[i] < 0x10) Serial.print("0");
      Serial.print(writeData[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    Serial.print("Read:    ");
    for (uint8_t i = 0; i < 16; i++) {
      Serial.print("0x");
      if (readData[i] < 0x10) Serial.print("0");
      Serial.print(readData[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  Serial.println();
  Serial.println("=== Bad Apple Video Frame Calculations ===");
  
  // 12x9 matrix = 108 pixels
  // Grayscale: 4-bit per pixel (for IS31FL3731's 8-bit PWM, we'll use full 8-bit)
  // So: 108 bytes per frame (1 byte per pixel for grayscale value 0-255)
  
  uint32_t bytesPerFrame = 108; // 12 x 9 pixels, 1 byte each
  uint32_t totalFrames = capacity / bytesPerFrame;
  
  Serial.print("Frame size: ");
  Serial.print(bytesPerFrame);
  Serial.println(" bytes (12x9 pixels, 8-bit grayscale)");
  
  Serial.print("Maximum frames: ");
  Serial.println(totalFrames);
  
  // Bad Apple is ~3:39 (219 seconds) at 30fps = 6570 frames
  uint32_t badAppleFrames = 30 * 219; // 30fps × 219 seconds
  uint32_t badAppleSize = badAppleFrames * bytesPerFrame;
  
  Serial.print("Bad Apple video (3:39 @ 30fps): ");
  Serial.print(badAppleFrames);
  Serial.print(" frames = ");
  Serial.print(badAppleSize / 1024);
  Serial.print(" KB (");
  Serial.print((badAppleSize * 100) / capacity);
  Serial.println("% of Flash)");
  
  Serial.println();
  Serial.print("You can store ~");
  Serial.print(totalFrames / 30 / 60);
  Serial.println(" minutes of video at 30fps!");
  
  Serial.println();
  Serial.println("=== Memory Layout Suggestion ===");
  Serial.println("Address 0x000000 - 0x0FFFFF : Birthday letter animation (1MB)");
  Serial.println("Address 0x100000 - 0x4FFFFF : Bad Apple video (4MB)");
  Serial.println("Address 0x500000 - 0x5FFFFF : Snake game assets (1MB)");
  Serial.println("Address 0x600000 - 0x6FFFFF : Pong game assets (1MB)");
  Serial.println("Address 0x700000 - 0xFFFFFF : Reserved (9MB)");
  
  Serial.println();
  Serial.println("=== Flash Test Complete ===");
  Serial.println("Flash is ready for video storage!");
}

void loop() {
  // Nothing to do
  delay(1000);
}
