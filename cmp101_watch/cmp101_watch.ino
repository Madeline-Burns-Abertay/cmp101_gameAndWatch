// taken from worksheet
#include <DS3231.h>
#include <Streaming.h>
#include <Wire.h>
#include <TM1638plus.h>

// taken from worksheet
#define STROBE_TM D5 // strobe = GPIO connected to strobe line of module
#define CLOCK_TM D6 // clock = GPIO connected to clock line of module
#define DIO_TM D7 // data = GPIO connected to data line of module

// taken from worksheet
bool high_freq = false; //default false, If using a high freq CPU > ~100 MHZ set to true.
DS3231 rtc;
bool h12Flag;
bool pmFlag;
TM1638plus tm(STROBE_TM, CLOCK_TM , DIO_TM, high_freq); 

// original
const uint8_t dpMask = 0b10000000;
const uint8_t sevenSegDecode[] = {
  0b00111111,
  0b00000110,
  0b01011011,
  0b01001111,
  0b01100110,
  0b01101101,
  0b01111101,
  0b00000111,
  0b01111111,
  0b01101111
}; 

void setup() { // taken from worksheet - the game part is where the lion's share of the effort went
  Wire.begin();
  Serial.begin(115200);
  //setDateAndTime();
  tm.displayBegin();

  tm.setLEDs(0);

}

void loop() { // mostly original i think
			  // i don't really remember
  tm.display7Seg(2, sevenSegDecode[rtc.getHour(h12Flag, pmFlag) / 10]);
  tm.display7Seg(3, sevenSegDecode[rtc.getHour(h12Flag, pmFlag) % 10] | dpMask);
  tm.display7Seg(4, sevenSegDecode[rtc.getMinute() / 10]);
  tm.display7Seg(5, sevenSegDecode[rtc.getMinute() % 10] | dpMask);
  tm.display7Seg(6, sevenSegDecode[rtc.getSecond() / 10]);
  tm.display7Seg(7, sevenSegDecode[rtc.getSecond() % 10]);
  Serial << rtc.getHour(h12Flag, pmFlag) << ":" << rtc.getMinute() << ":" << rtc.getSecond() << endl;
  delay(1000);
}
