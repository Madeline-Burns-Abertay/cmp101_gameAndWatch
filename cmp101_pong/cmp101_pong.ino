#include <SPI.h>
#include <Streaming.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// taken from worksheet
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCORE_BAR_BOTTOM 8 // original

// taken from worksheet
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C // because the screen is 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// original
#define FRAMERATE 60.0 // the game's target framerate, in frames per second
#define FRAME_DELTA 1000/FRAMERATE

struct Vector2 { // original, inspired by unity
  int16_t x;
  int8_t y;
};

class Ball { // original
  Vector2 pos;
  Vector2 vel;
  public:

  Ball(int8_t posX, int8_t posY, int8_t velX, int8_t velY) {
    pos.x = posX;
    pos.y = posY;
    vel.x = velX;
    vel.y = velY;
  }

  Vector2 getPos() {
    return pos;
  }

  void draw() {
    display.drawPixel(pos.x, pos.y, SSD1306_WHITE);
  }

  void move() {
    pos.x += vel.x;
    pos.y += vel.y;
  }

  void bounce(bool offPaddle) {
    if (offPaddle) {
      vel.x = -vel.x;
      // todo: what to do with the vertical speed
    }
    else { // if it didn't bounce off a paddle, it must've bounced off the ceiling or floor
      vel.y = -vel.y; // either way, the vertical speed is just negated
    }
  }
};

class Paddle { // original
  bool player;
  Vector2 pos;
  int8_t score = 0;
  int8_t length = 5;

  public:
  Paddle(bool p, int8_t y) {
    player = p;
    pos.x = (player ? 0 : display.width()-1);
    pos.y = y;
  }

  void draw() {
    display.drawLine(pos.x, pos.y, pos.x, pos.y+length-1, SSD1306_WHITE); // without the -1, the paddle would be 6 pixels long
  }

  void move(int8_t newPos) {
    pos.y = newPos;
    if (newPos < SCORE_BAR_BOTTOM) pos.y = SCORE_BAR_BOTTOM;
    if (newPos > SCREEN_HEIGHT - length) pos.y = SCREEN_HEIGHT - length;
  }

  Vector2 getPos() {
    return pos;
  }
};

// original
Paddle player(true, 20);
Paddle cpu(false, 20);
Ball ball(20, 10, 1, 1);
int8_t playerScore, cpuScore;
bool gameOn = true;

void setup() { 
  Serial.begin(9600); // worksheet
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;) { // 30hz blinking led of doom
              // everything from here down is original
      digitalWrite(LED_BUILTIN, HIGH);
      delay(FRAME_DELTA);
      digitalWrite(LED_BUILTIN, LOW);
      delay(FRAME_DELTA);
    } 
  }
  player.draw();
  player.move(analogRead(A0) >> 5);
  cpu.draw();
  ball.draw();
  display.display();
}

void moveThings() {
  player.move(analogRead(A0) >> 5);
  ball.move();
  Vector2 ballPos = ball.getPos();
  if (ballPos.y == SCREEN_HEIGHT || ballPos.y == SCORE_BAR_BOTTOM) ball.bounce(false);
}

void displayFrame() {
  display.clearDisplay();
  if (gameOn) {
    player.draw();
    cpu.draw();
    ball.draw();
    display.drawChar(50, 0, playerScore + 0x30, SSD1306_WHITE, SSD1306_BLACK, 1);
    display.drawChar(SCREEN_WIDTH - 50, 0, playerScore + 0x30, SSD1306_WHITE, SSD1306_BLACK, 1);
  }
  else {
    display.print((playerScore == 10 ? F("player wins") : F("cpu wins")))
  }
  display.display();
}

void loop() {
  if (gameOn) {
    moveThings();
  }
  displayFrame();
  gameOn = !(playerScore == 10 || cpuScore == 10);
  delay(FRAME_DELTA);
}


