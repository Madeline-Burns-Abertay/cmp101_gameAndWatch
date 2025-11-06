#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C // because the screen is 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define FRAMERATE 60.0 // the game's target framerate, in frames per second
#define FRAME_DELTA 1000/FRAMERATE

typedef struct Vector2 {
  int8_t x;
  int8_t y;
};

class Ball {
  int8_t pos;
  Vector2 vel;
  public:

  void draw() {

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

class Paddle {
  bool player;
  Vector2 pos;
  int8_t length = 5;

  public:
  Paddle(bool p, int8_t y) {
    player = p;
    pos.x = (player ? 0 : display.width()-1);
    pos.y = y;
  }

  void draw() {
    display.drawLine(pos.x, pos.y, pos.x, pos.y+length-1, SSD1306_WHITE); // without the -1, the paddle would be 6 pixels long
    display.display();
  }

  void move(int8_t vel) {
    if (pos.y + vel <= 7) return;
    pos.y += vel;
  }

  Vector2 getPos() {
    return pos;
  }
};

Paddle player(true, 20);
Paddle cpu(false, 20);

void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    pinMode(LED_BUILTIN, OUTPUT);
    for(;;) { // 30hz blinking led of doom
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1/FRAMERATE * 1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1/FRAMERATE * 1000);
    } 
  }
  player.draw();
  cpu.draw();
}

void loop() {
  
  display.clearDisplay();
  player.draw();
  cpu.draw();
  delay(1000);
}
