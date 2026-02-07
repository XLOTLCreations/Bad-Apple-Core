/*
 * SNAKE GAME
 * Classic snake game on 12x9 LED matrix
 * 
 * Controls:
 * - Up:    D0  (move up)
 * - Down:  A5  (move down)
 * - Right: A4  (move right)
 * - Left:  D11 (move left)
 * 
 * Rules:
 * - Eat food to grow
 * - Don't hit walls or yourself
 * - Game over = restart
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>

// Pin definitions
#define BTN_UP    0
#define BTN_DOWN  A5
#define BTN_RIGHT A4
#define BTN_LEFT  11
#define SDB_PIN   9

// Game constants
#define GRID_WIDTH  12
#define GRID_HEIGHT 9
#define MAX_SNAKE_LENGTH 108  // Max possible on 12x9

Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

// Snake structure
struct Point {
  int8_t x;
  int8_t y;
};

Point snake[MAX_SNAKE_LENGTH];
uint8_t snakeLength = 3;
Point food;
int8_t dirX = 1;  // Start moving right
int8_t dirY = 0;
uint16_t score = 0;
bool gameOver = false;
unsigned long lastMoveTime = 0;
uint16_t moveDelay = 500;  // milliseconds between moves

void setup() {
  // Enable IS31FL3731
  pinMode(SDB_PIN, OUTPUT);
  digitalWrite(SDB_PIN, HIGH);
  delay(5);
  
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== SNAKE GAME ===");
  Serial.println("Use arrow buttons to move");
  Serial.println("Eat food to grow!");
  Serial.println();
  
  // Setup buttons
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  
  // Initialize LED matrix
  if (!matrix.begin(0x74)) {
    Serial.println("IS31FL3731 not found!");
    while (1);
  }
  
  matrix.clear();
  
  // Seed random number generator
  randomSeed(analogRead(A0));
  
  // Start game
  initGame();
  
  Serial.println("Game started! Score: 0");
}

void initGame() {
  // Reset snake to center, length 3, moving right
  snakeLength = 3;
  snake[0] = {6, 4};  // Head
  snake[1] = {5, 4};
  snake[2] = {4, 4};  // Tail
  
  dirX = 1;
  dirY = 0;
  score = 0;
  gameOver = false;
  moveDelay = 300;
  
  // Spawn first food
  spawnFood();
}

void spawnFood() {
  // Find empty spot for food
  bool validSpot = false;
  
  while (!validSpot) {
    food.x = random(0, GRID_WIDTH);
    food.y = random(0, GRID_HEIGHT);
    
    // Check if food spawned on snake
    validSpot = true;
    for (uint8_t i = 0; i < snakeLength; i++) {
      if (snake[i].x == food.x && snake[i].y == food.y) {
        validSpot = false;
        break;
      }
    }
  }
  
  Serial.print("Food spawned at (");
  Serial.print(food.x);
  Serial.print(", ");
  Serial.print(food.y);
  Serial.println(")");
}

void handleInput() {
  // Read buttons (active LOW)
  bool up = (digitalRead(BTN_UP) == LOW);
  bool down = (digitalRead(BTN_DOWN) == LOW);
  bool right = (digitalRead(BTN_RIGHT) == LOW);
  bool left = (digitalRead(BTN_LEFT) == LOW);
  
  // Change direction (prevent 180-degree turns)
  if (up && dirY != 1) {
    dirX = 0;
    dirY = -1;
  } else if (down && dirY != -1) {
    dirX = 0;
    dirY = 1;
  } else if (right && dirX != -1) {
    dirX = 1;
    dirY = 0;
  } else if (left && dirX != 1) {
    dirX = -1;
    dirY = 0;
  }
}

void moveSnake() {
  // Calculate new head position
  Point newHead;
  newHead.x = snake[0].x + dirX;
  newHead.y = snake[0].y + dirY;
  
  // Check wall collision
  if (newHead.x < 0 || newHead.x >= GRID_WIDTH || 
      newHead.y < 0 || newHead.y >= GRID_HEIGHT) {
    gameOver = true;
    Serial.println("GAME OVER! Hit wall!");
    Serial.print("Final Score: ");
    Serial.println(score);
    return;
  }
  
  // Check self collision
  for (uint8_t i = 0; i < snakeLength; i++) {
    if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
      gameOver = true;
      Serial.println("GAME OVER! Hit yourself!");
      Serial.print("Final Score: ");
      Serial.println(score);
      return;
    }
  }
  
  // Check food collision
  bool ateFood = (newHead.x == food.x && newHead.y == food.y);
  
  if (ateFood) {
    score++;
    Serial.print("Food eaten! Score: ");
    Serial.println(score);
    
    // Speed up slightly
    if (moveDelay > 100) {
      moveDelay -= 10;
    }
    
    // Grow snake (don't remove tail)
    snakeLength++;
    if (snakeLength >= MAX_SNAKE_LENGTH) {
      // YOU WIN!
      Serial.println("YOU WIN! Maximum length reached!");
      gameOver = true;
      return;
    }
    
    spawnFood();
  }
  
  // Move snake body
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  
  // Move head
  snake[0] = newHead;
  
  // If didn't eat food, we already moved tail by shifting array
  // If ate food, snake grew by 1
}

void drawGame() {
  matrix.clear();
  
  // Draw snake body
  for (uint8_t i = 1; i < snakeLength; i++) {
    matrix.drawPixel(snake[i].x, snake[i].y, 150);
  }
  
  // Draw snake head (brighter)
  matrix.drawPixel(snake[0].x, snake[0].y, 255);
  
  // Draw food (blink every 500ms)
  if ((millis() / 250) % 2 == 0) {
    matrix.drawPixel(food.x, food.y, 200);
  }
}

void showGameOver() {
  // Flash screen 3 times
  for (uint8_t flash = 0; flash < 3; flash++) {
    matrix.clear();
    delay(200);
    
    // Fill screen
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      for (uint8_t x = 0; x < GRID_WIDTH; x++) {
        matrix.drawPixel(x, y, 100);
      }
    }
    delay(200);
  }
  
  matrix.clear();
  
  // Show score as filled pixels (up to 108)
  uint16_t pixelsToShow = min(score, (uint16_t)108);
  uint8_t count = 0;
  
  for (uint8_t y = 0; y < GRID_HEIGHT && count < pixelsToShow; y++) {
    for (uint8_t x = 0; x < GRID_WIDTH && count < pixelsToShow; x++) {
      matrix.drawPixel(x, y, 150);
      count++;
    }
  }
  
  delay(3000);
  
  Serial.println();
  Serial.println("Press any button to restart...");
  
  // Wait for button press
  while (true) {
    if (digitalRead(BTN_UP) == LOW || 
        digitalRead(BTN_DOWN) == LOW ||
        digitalRead(BTN_RIGHT) == LOW || 
        digitalRead(BTN_LEFT) == LOW) {
      delay(200); // Debounce
      break;
    }
    delay(10);
  }
  
  // Restart game
  Serial.println();
  Serial.println("=== NEW GAME ===");
  initGame();
  Serial.println("Game started! Score: 0");
}

void loop() {
  if (gameOver) {
    showGameOver();
    return;
  }
  
  // Handle input every frame
  handleInput();
  
  // Move snake at controlled speed
  if (millis() - lastMoveTime >= moveDelay) {
    moveSnake();
    lastMoveTime = millis();
  }
  
  // Draw every frame
  drawGame();
  
  delay(10);  // Small delay to prevent button bouncing
}
