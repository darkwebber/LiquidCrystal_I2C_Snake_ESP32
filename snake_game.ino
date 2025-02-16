#include <LiquidCrystal_I2C.h>
#include <cppQueue.h>
#include <EEPROM.h>

#define IMPLEMENTATION FIFO
#define EEPROM_SIZE 1

const int lcdColumns   = 16;
const int lcdRows      = 2;
const int gameColumns  = 4;
const int gameRows     = 2;
const int gridWidth    = gameColumns * 5;
const int gridHeight   = 16;             

struct Pos {
  int row;
  int col;
};

volatile char dir    = 'R';
volatile char dirOld = 'R';

Pos head;
Pos food;
int bodyLen   = 0;
int highScore = 0;
bool gameOver = false;
bool menu     = false;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
cppQueue body(sizeof(Pos), 1280, IMPLEMENTATION);

void IRAM_ATTR handleRight() {
  if (dir != 'L') {
    dirOld = dir;
    dir = 'R';
  }
}

void IRAM_ATTR handleLeft() {
  if (dir != 'R') {
    dirOld = dir;
    dir = 'L';
  }
}

void IRAM_ATTR handleUp() {
  if (dir != 'D') {
    dirOld = dir;
    dir = 'U';
  }
}

void IRAM_ATTR handleDown() {
  if (dir != 'U') {
    dirOld = dir;
    dir = 'D';
  }
}

bool isOccupied(const Pos &position) {
  if (position.row == head.row && position.col == head.col) {
    return true;
  }
  for (int i = 0; i < bodyLen; i++) {
    Pos segment;
    body.peekIdx(&segment, i);
    if (segment.row == position.row && segment.col == position.col) {
      return true;
    }
  }
  return false;
}

void spawnFood() {
  do {
    food.col = random(gridWidth);
    food.row = random(gridHeight);
  } while (isOccupied(food));
}

void drawPoint(const Pos &position, byte screen[8][8], bool isBody = true) {
  int charIndex = (position.row / 8) * gameColumns + (position.col / 5);
  int rowInChar = position.row % 8;
  byte bitMask  = (0b10000 >> (position.col % 5));
  screen[charIndex][rowInChar] |= bitMask;

  if (isBody && (position.row == head.row && position.col == head.col)) {
    gameOver = true;
  }
}

void drawHighScore() {
  lcd.setCursor(13, 0);
  lcd.print("Max");
  lcd.setCursor(14, 1);
  lcd.print(highScore);
}

void drawScore() {
  lcd.setCursor(6, 0);
  lcd.print("Score");
  int score = bodyLen;
  if (score > highScore) {
    highScore = score;
    EEPROM.write(0, highScore);
    EEPROM.commit();
  }
  lcd.setCursor((score < 10) ? 8 : 7, 1);
  lcd.print(score);
}

void drawGameOver() {
  lcd.setCursor(4, 0);
  lcd.print("Restart?");
  int score = bodyLen;
  lcd.setCursor((score < 10) ? 8 : 7, 1);
  lcd.print(score);
}

void draw() {
  byte screen[8][8] = {{0}};

  drawPoint(head, screen, false);
  drawPoint(food, screen, false);

  for (int i = 0; i < bodyLen; i++) {
    Pos segment;
    body.peekIdx(&segment, i);
    drawPoint(segment, screen);
  }

  for (int i = 0; i < 8; i++) {
    int row = (i < gameColumns) ? 0 : 1;
    int col = i % gameColumns;
    lcd.createChar(i, screen[i]);
    lcd.setCursor(col, row);
    lcd.write(i);
  }

  if (gameOver) {
    drawGameOver();
  } else {
    drawScore();
  }
  drawHighScore();
}

void updateGameState() {
  body.push(&head);

  switch (dir) {
    case 'R': head.col = (head.col + 1) % gridWidth; break;
    case 'L': head.col = (head.col - 1 + gridWidth) % gridWidth; break;
    case 'U': head.row = (head.row - 1 + gridHeight) % gridHeight; break;
    case 'D': head.row = (head.row + 1) % gridHeight; break;
  }

  if (head.row == food.row && head.col == food.col) {
    bodyLen++;     
    spawnFood();  
  } else {
    Pos tail;
    body.pop(&tail);
  }
  draw();
}

void initGameState() {
  head.col = 3;
  head.row = 0;
  bodyLen  = 0;
  body.clean();
  gameOver = false;
  menu     = false;

  highScore = EEPROM.read(0);

  spawnFood();

  lcd.clear();
  draw();
}

void setup() {
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);
  randomSeed(21);

  lcd.init();
  lcd.backlight();

  initGameState();

  pinMode(13, INPUT_PULLUP); // Right
  pinMode(12, INPUT_PULLUP); // Up
  pinMode(14, INPUT_PULLUP); // Down
  pinMode(27, INPUT_PULLUP); // Left

  attachInterrupt(digitalPinToInterrupt(13), handleRight, FALLING);
  attachInterrupt(digitalPinToInterrupt(27), handleLeft, FALLING);
  attachInterrupt(digitalPinToInterrupt(12), handleUp, FALLING);
  attachInterrupt(digitalPinToInterrupt(14), handleDown, FALLING);
}

void loop() {
  if (!gameOver) {
    updateGameState();
  } else if ((dirOld == 'R') && (dir == 'U')) {
    menu = true;
    dirOld = 'U';
    lcd.clear();
  } else if (menu) {
    lcd.setCursor(3, 0);
    lcd.print("Clear Max?");
    lcd.setCursor(2, 1);
    lcd.print("No(L)/Yes(R)");
    if ((dirOld == 'U') && (dir == 'L')) {
      initGameState();
    } else if ((dirOld == 'U') && (dir == 'R')) {
      EEPROM.write(0, 0);
      EEPROM.commit();
      initGameState();
    }
  } else {
    lcd.clear();
    draw();
  }
  delay(50);
}