#include <Wire.h>
#include <SPI.h>
#include <SPIMemory.h>

// ---------- Hardware config ----------
static const uint8_t PIN_SDB   = 9;    // IS31 SDB pin
static const uint8_t FLASH_CS  = 17;   // PB0 (often D17 on 32U4 cores)
static const uint8_t IS31_ADDR = 0x74;

// Buttons (active-low, use internal pull-ups)
static const uint8_t PIN_UP    = 0;    // Digital 0
static const uint8_t PIN_DOWN  = A5;   // ADC0
static const uint8_t PIN_RIGHT = A4;   // ADC1
static const uint8_t PIN_LEFT  = 11;   // Digital 11

// ---------- Video config ----------
static const uint16_t W = 12;
static const uint16_t H = 9;
static const uint16_t BYTES_PER_FRAME = W * H; // 108
static const uint16_t NUM_FRAMES = 6572;
static const uint32_t VIDEO_BASE_ADDR = 0x000000UL;

// ---------- IS31 registers ----------
static const uint8_t REG_PAGE  = 0xFD;
static const uint8_t PAGE_FUNC = 0x0B;
static const uint8_t PAGE_F0   = 0x00;

SPIFlash flash(FLASH_CS);

// Brightness control
static uint8_t brightness = 255;       // 0..255
static const uint8_t BRIGHT_STEP = 16; // per press
static const uint16_t BTN_DEBOUNCE_MS = 120;

static void is31_write(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(IS31_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

static void is31_page(uint8_t p) {
  is31_write(REG_PAGE, p);
}

static void is31_init() {
  pinMode(PIN_SDB, OUTPUT);
  digitalWrite(PIN_SDB, HIGH);
  delay(5);

  Wire.begin();
  Wire.setClock(400000);

  // Function page: exit shutdown
  is31_page(PAGE_FUNC);
  is31_write(0x0A, 0x01); // normal operation

  // Enable all LEDs (we'll only write PWM to our 12x9 window)
  is31_page(PAGE_F0);
  for (uint8_t r = 0x00; r <= 0x11; r++) is31_write(r, 0xFF);

  // Clear PWM
  for (uint16_t r = 0x24; r <= 0xB3; r++) is31_write((uint8_t)r, 0x00);
}

// Map (x,y) in our 12x9 window into IS31's 16x9 linear index
static inline uint8_t is31_index(uint8_t x, uint8_t y) {
  return (uint8_t)(y * 16 + x); // using columns 0..11 of 16
}

// Efficiently write one frame: 108 bytes -> 108 PWM regs.
// Writes 12 bytes per row, skipping unused columns 12..15.
static void is31_write_frame_scaled(const uint8_t *frame) {
  is31_page(PAGE_F0);

  uint16_t src = 0;
  for (uint8_t y = 0; y < H; y++) {
    uint8_t startReg = (uint8_t)(0x24 + is31_index(0, y));

    Wire.beginTransmission(IS31_ADDR);
    Wire.write(startReg);

    for (uint8_t x = 0; x < W; x++) {
      uint8_t v = frame[src++];
      v = (uint16_t(v) * brightness) >> 8;  // scale 0..255
      Wire.write(v);
    }

    Wire.endTransmission();
  }
}

static void handleBrightnessButtons() {
  static uint32_t lastMs = 0;
  uint32_t now = millis();
  if (now - lastMs < BTN_DEBOUNCE_MS) return;

  bool upPressed = (digitalRead(PIN_UP) == LOW);
  bool dnPressed = (digitalRead(PIN_DOWN) == LOW);

  if (upPressed || dnPressed) {
    lastMs = now;

    if (upPressed) {
      if (brightness <= 255 - BRIGHT_STEP) brightness += BRIGHT_STEP;
      else brightness = 255;
    }
    if (dnPressed) {
      if (brightness >= BRIGHT_STEP) brightness -= BRIGHT_STEP;
      else brightness = 0;
    }
  }
}

// ---------- 30 FPS timing ----------
static uint32_t nextTickUs = 0;
static uint16_t frameNo = 0;
static uint8_t frameBuf[BYTES_PER_FRAME];

void setup() {
  // Buttons (active-low)
  pinMode(PIN_UP,    INPUT_PULLUP);
  pinMode(PIN_DOWN,  INPUT_PULLUP);
  pinMode(PIN_RIGHT, INPUT_PULLUP);
  pinMode(PIN_LEFT,  INPUT_PULLUP);

  // Flash
  SPI.begin();
  if (!flash.begin()) {
    while (1) {}
  }

  // Display
  is31_init();

  nextTickUs = micros();
}

void loop() {
  handleBrightnessButtons();

  // 30 FPS = 33,333 us
  const uint32_t framePeriodUs = 33333UL;
  uint32_t now = micros();
  if ((int32_t)(now - nextTickUs) < 0) return;
  nextTickUs += framePeriodUs;

  // Read next frame from flash
  uint32_t addr = VIDEO_BASE_ADDR + (uint32_t)frameNo * BYTES_PER_FRAME;
  if (!flash.readByteArray(addr, frameBuf, BYTES_PER_FRAME)) {
    for (uint16_t i = 0; i < BYTES_PER_FRAME; i++) frameBuf[i] = 0;
  }

  // Push to IS31 (scaled by brightness)
  is31_write_frame_scaled(frameBuf);

  // Next frame
  frameNo++;
  if (frameNo >= NUM_FRAMES) frameNo = 0;
}
