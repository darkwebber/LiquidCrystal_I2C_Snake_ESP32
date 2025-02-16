#include <LiquidCrystal_I2C.h>
#include <cppQueue.h>
#define IMPLEMENTATION FIFO

typedef struct Pos {
    int row;
    int col;
} Pos;

int lcdColumns = 16;
int lcdRows = 2;
int gameColumns = 4;
int gameRows = 2;

volatile char dir = 'R';
Pos head;
Pos food;
int body_len;
bool food_found;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
cppQueue body(sizeof(Pos), 1280, IMPLEMENTATION);

void IRAM_ATTR handleRight() { if (dir != 'L') dir = 'R'; }
void IRAM_ATTR handleLeft() { if (dir != 'R') dir = 'L'; }
void IRAM_ATTR handleUp() { if (dir != 'D') dir = 'U'; }
void IRAM_ATTR handleDown() { if (dir != 'U') dir = 'D'; }

void draw_point(Pos pos, byte (&screen)[8][8]) {
    byte temp = (0b10000 >> (pos.col % 5));
    screen[(pos.row / 8) * gameColumns + (pos.col / 5)][pos.row % 8] |= temp;
}

void draw() {
    lcd.clear();
    byte screen[8][8] = {{0}};

    draw_point(head, screen);
    draw_point(food, screen);
    
    if (body_len > 0) {
        for (int i = 0; i < body_len; i++) {
            Pos seg;
            body.peekIdx(&seg, i);
            draw_point(seg, screen);
        }
    }

    for (int i = 0; i < 8; i++) {
        int x = (i < gameColumns) ? 0 : 1;
        int y = i % gameColumns;
        lcd.createChar(i, screen[i]);
        lcd.setCursor(y, x);
        lcd.write(i);
    }
}

void updateGameState() {
    body.push(&head);

    if (dir == 'R') head.col = (head.col + 1) % (gameColumns * 5);
    else if (dir == 'L') head.col = (head.col - 1 + (gameColumns * 5)) % (gameColumns * 5);
    else if (dir == 'U') head.row = (head.row - 1 + 16) % 16;
    else if (dir == 'D') head.row = (head.row + 1) % 16;

    if (head.row == food.row && head.col == food.col) {
        food_found = true;
        food.col = random(gameColumns * 5);
        food.row = random(16);
        body_len++;
        food_found = false;
    } else {
        Pos last_seg;
        body.pop(&last_seg);
    }
    draw();
}

void setup() {
    Serial.begin(9600);
    randomSeed(21);
    
    food_found = false;
    food.col = random(gameColumns * 5);
    food.row = random(16);
    
    head.col = 3;
    head.row = 0;
    body_len = 0;
    
    lcd.init();
    lcd.backlight();
    draw();

    pinMode(13, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
    pinMode(27, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(13), handleRight, FALLING);
    attachInterrupt(digitalPinToInterrupt(27), handleLeft, FALLING);
    attachInterrupt(digitalPinToInterrupt(12), handleUp, FALLING);
    attachInterrupt(digitalPinToInterrupt(14), handleDown, FALLING);
}

void loop() {
    updateGameState();
    delay(50);
}