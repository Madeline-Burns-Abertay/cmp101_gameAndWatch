#include <SPI.h>
#include <Streaming.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// taken from worksheet
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C  // because the screen is 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// original
#define FRAMERATE 60.0  // the game's target framerate, in frames per second
#define FRAME_DELTA 1000 / FRAMERATE
#define SCORE_BAR_BOTTOM 8
#define OUT_FIELD_DIST static_cast<int16_t>(FRAMERATE / 2) // the ball should take half a second to go home after it goes out
#define POT A0
#define BUZZER D3
#define WINNING_SCORE 5


struct Vector2 {  // original, inspired by unity
  int16_t x;
  int8_t y;
};

class Paddle {  // original
  Vector2 pos;
  int8_t score = 0;
  int8_t length = 5;

public:
  Paddle(bool isPlayer, int8_t y) {
    pos.x = (isPlayer ? 0 : SCREEN_WIDTH - 1);
    pos.y = y;
  }

  void draw() {
    display.drawLine(pos.x, pos.y, pos.x, pos.y + length - 1, SSD1306_WHITE);  // without the -1, the paddle would be 6 pixels long
  }

  void move(int8_t newPos) {
    pos.y = newPos;
    if (newPos < SCORE_BAR_BOTTOM) pos.y = SCORE_BAR_BOTTOM;
    if (newPos > SCREEN_HEIGHT - length) pos.y = SCREEN_HEIGHT - length;
  }

  Vector2 getPos() {
    return pos;
  }

  int8_t getLength() {
    return length;
  }
};

class Ball {  // original
  Vector2 posInit;
  Vector2 pos;
  Vector2 vel;
public:
  Ball(int8_t posX, int8_t posY, int8_t velX, int8_t velY) {
    pos.x = posX;
    pos.y = posY;
    vel.x = velX;
    vel.y = velY;
    posInit = pos;
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
    } else {           // if it didn't bounce off a paddle, it must've bounced off the ceiling or floor
      vel.y = -vel.y;  // either way, the vertical speed is just negated
    }
    digitalWrite(BUZZER, HIGH); // the buzzer should go off regardless of what it hits
  }

  void teleportHome() {
    pos = posInit;
  }

  bool checkCollision(Paddle *paddle) {
    Vector2 paddlePos = paddle->getPos();
    return (pos.x == paddlePos.x && paddlePos.y <= pos.y && pos.y < paddlePos.y + paddle->getLength());
  }
};

// original
class Game {
  Paddle *player, *cpu;
  Ball *ball;
  Vector2 playerPos, cpuPos, ballPos;
  int8_t playerScore, cpuScore;
  bool gameOn = true;
public:

  ~Game() {
    delete player;
    delete cpu;
    delete ball;
  }

  void startGame() {
    player = new Paddle(true, analogRead(POT) >> 5);
    cpu = new Paddle(false, SCREEN_WIDTH / 2);
    cpuPos = cpu->getPos();
    ball = new Ball(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 1, 1);
    player->draw();
    cpu->draw();
    ball->draw();
    display.display();
  }

  void moveThings() {
    digitalWrite(BUZZER, LOW);
    ball->move();
    ballPos = ball->getPos();
    player->move(analogRead(POT) >> 5);
    cpu->move((random(20) < 5 ? ballPos.y - 2 : cpuPos.y));
    playerPos = player->getPos();
    cpuPos = cpu->getPos();
    if (ballPos.y == SCREEN_HEIGHT - 1 || ballPos.y == SCORE_BAR_BOTTOM) {
      ball->bounce(false);
    }

    if (ball->checkCollision(player) || ball->checkCollision(cpu)) {
      ball->bounce(true);
    }

    if (ballPos.x == SCREEN_WIDTH + OUT_FIELD_DIST) {
      playerScore++;
      ball->bounce(true);
      ball->teleportHome();
    }
    if (ballPos.x <= -OUT_FIELD_DIST) {
      cpuScore++;
      ball->bounce(true);
      ball->teleportHome();
    }
  };

  void displayFrame() {
    display.clearDisplay();
    player->draw();
    cpu->draw();
    ball->draw();
    display.drawChar(50, 0, playerScore + 0x30, SSD1306_WHITE, SSD1306_BLACK, 1);
    display.drawChar(SCREEN_WIDTH - 50, 0, cpuScore + 0x30, SSD1306_WHITE, SSD1306_BLACK, 1);
    display.setCursor(ballPos.x, ballPos.y);
    display.printf("(%d, %d)", ballPos.x, ballPos.y);
    display.display();
  }

  void setGameOnFlag() {
    gameOn = !(playerScore == WINNING_SCORE || cpuScore == WINNING_SCORE);
  }

  bool isGameOn() {
    return gameOn;
  }

  bool didPlayerWin() {
    return playerScore == WINNING_SCORE;
  }
};

// original
Game game;

void setup() {
  Serial.begin(9600);  // worksheet
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial << F("SSD1306 allocation failed");
    for (;;) {  // 30hz blinking led of doom
                // everything from here down is original
      digitalWrite(LED_BUILTIN, HIGH);
      delay(FRAME_DELTA);
      digitalWrite(LED_BUILTIN, LOW);
      delay(FRAME_DELTA);
    }
  }
  pinMode(BUZZER, OUTPUT);
  game.startGame();
}

void loop() {
  if (game.isGameOn()) {
    game.moveThings();
    game.displayFrame();
    game.setGameOnFlag();
    delay(FRAME_DELTA);
  }
  else {
    game.~Game();
    display.clearDisplay();
    digitalWrite(BUZZER, LOW);
    String winMessage = (game.didPlayerWin() ? "player wins" : "cpu wins");
    int8_t charPos; // because the print functions just refuse to work
    for (;;) {
      charPos = 30;
      for (char c : winMessage) {
        display.drawChar(charPos, 0, c, SSD1306_WHITE, SSD1306_BLACK, 1);
        charPos += 6;
      }
      display.println(winMessage);
      display.display();
      delay(FRAME_DELTA);
    }
  }
}
