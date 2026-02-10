#include <SPI.h>
#include <SPIMemory.h>

static const uint8_t CS_PIN = 17;  // PB0 on ATmega32U4 (often D17)
SPIFlash flash(CS_PIN);

static uint32_t crc32_update(uint32_t crc, uint8_t data) {
  crc ^= data;
  for (uint8_t i = 0; i < 8; i++) {
    crc = (crc & 1) ? (crc >> 1) ^ 0xEDB88320UL : (crc >> 1);
  }
  return crc;
}

static void readExact(uint8_t *buf, size_t n) {
  size_t got = 0;
  while (got < n) {
    if (Serial.available()) {
      buf[got++] = (uint8_t)Serial.read();
    }
  }
}

static uint32_t readU32LE() {
  uint8_t b[4];
  readExact(b, 4);
  return (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

void setup() {
  Serial.begin(921600);
  while (!Serial) {}

  Serial.println("FLASH UPLOADER READY");
  Serial.println("Send: 4-byte LE length, then raw bytes");

  SPI.begin();

  if (!flash.begin()) {
    Serial.println("ERROR: flash.begin() failed");
    while (1) {}
  }

  Serial.println("Flash begin OK");
}

void loop() {
  if (Serial.available() < 4) return;

  uint32_t length = readU32LE();
  if (length == 0 || length > 16UL * 1024UL * 1024UL) {
    Serial.println("ERROR: bad length");
    return;
  }

  Serial.print("Length: ");
  Serial.println(length);

  Serial.println("Erasing chip...");
  flash.eraseChip(); // blocks until done on your library build

  Serial.println("Receiving + writing...");
  const uint16_t CHUNK = 256;
  uint8_t buf[CHUNK];

  uint32_t addr = 0;
  uint32_t remaining = length;
  uint32_t crc_tx = 0xFFFFFFFFUL;

  while (remaining) {
    uint16_t n = (remaining > CHUNK) ? CHUNK : (uint16_t)remaining;
    readExact(buf, n);

    for (uint16_t i = 0; i < n; i++) crc_tx = crc32_update(crc_tx, buf[i]);

    if (!flash.writeByteArray(addr, buf, n)) {
      Serial.println("ERROR: writeByteArray failed");
      return;
    }

    addr += n;
    remaining -= n;

    if ((addr & 0x3FFF) == 0) { // every 16KB
      Serial.print("Wrote ");
      Serial.println(addr);
    }
  }

  crc_tx ^= 0xFFFFFFFFUL;
  Serial.print("TX CRC32: 0x");
  Serial.println(crc_tx, HEX);

  Serial.println("Verifying...");
  uint32_t crc_rx = 0xFFFFFFFFUL;
  addr = 0;
  remaining = length;

  while (remaining) {
    uint16_t n = (remaining > CHUNK) ? CHUNK : (uint16_t)remaining;
    if (!flash.readByteArray(addr, buf, n)) {
      Serial.println("ERROR: readByteArray failed");
      return;
    }
    for (uint16_t i = 0; i < n; i++) crc_rx = crc32_update(crc_rx, buf[i]);

    addr += n;
    remaining -= n;
  }

  crc_rx ^= 0xFFFFFFFFUL;
  Serial.print("RX CRC32: 0x");
  Serial.println(crc_rx, HEX);

  Serial.println(crc_rx == crc_tx ? "OK: verify pass" : "ERROR: verify mismatch");
  Serial.println("DONE.");
  while (1) {}
}
