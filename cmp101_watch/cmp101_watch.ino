#include <TM1638plus.h>
#include <Streaming.h>
#include <DS3231.h>

#define STROBE_TM D5 // strobe = GPIO connected to strobe line of module
#define CLOCK_TM D6 // clock = GPIO connected to clock line of module
#define DIO_TM D7 // data = GPIO connected to data line of module
bool high_freq = false; //default false, If using a high freq CPU > ~100 MHZ set to true.
// Constructor object (GPIO STB , GPIO CLOCK , GPIO DIO, use high freq MCU)
TM1638plus tm(STROBE_TM, CLOCK_TM , DIO_TM, high_freq);

DS3231 rtc;
bool century = false;
bool h12Flag;
bool pmFlag;

void setDateAndTime(){
  rtc.setClockMode(false); // false = 24hr clock mode
  rtc.setYear(25);
  rtc.setMonth(11);
  rtc.setDate(6);
  rtc.setHour(12);
  rtc.setMinute(54);
  rtc.setSecond(0);
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  Serial << (F("\nDS3231 Hi Precision Real Time Clock")) << endl;
  setDateAndTime(); // Only need to do this once ever.
  // You should comment this out after you've successfully set the RTC
  tm.displayBegin();
}

void loop() {
 tm.reset();
 for (int i = 0; i < 255; i++) {
 tm.displayIntNum(i, false);
 delay(100);
 Serial << rtc.getDate() << "/" << rtc.getMonth(century) << "/" << rtc.getYear() << " " ;
 Serial << rtc.getHour(h12Flag, pmFlag) << ":" << rtc.getMinute() << ":" << rtc.getSecond() << endl;
 }
}

