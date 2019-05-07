#include <SPI.h>
#include <RH_RF95.h>
#include "src/RTCZero/src/RTCZero.h"

/* for feather m0  */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 868.0
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

const byte button = 10;

const byte timerSeconds = 30;
const byte timerMinutes = 0;
const byte timerHours = 0;

// debounce ISR
const unsigned long debounceTime = 500;
volatile bool ir_selector_pressed = false;
volatile bool wake_timer = false;


RTCZero rtc;

void setup() {  

  for (uint32_t ul = 0 ; ul < NUM_DIGITAL_PINS ; ul++ )
  {
    pinMode( ul, OUTPUT ) ;
    digitalWrite(ul, LOW);
  }

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    blink(LED_BUILTIN, 9999);
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    blink(LED_BUILTIN, 9999);
    while (1);
  }
  
  rf95.sleep();

  int wait = millis();
  while(!Serial && millis()-wait < 3000){}
  Serial.println("== Interrupt test ==");
  blink(LED_BUILTIN, 3);
  pinMode(button, INPUT_PULLDOWN);

  int eint_pin = digitalPinToInterrupt(button);
  attachInterrupt(eint_pin, ISR_pin, HIGH);
  rtc.begin();
  blink(LED_BUILTIN, 2);
  Serial.println("== Starting ==");
  resetTimer();
  rtc.attachInterrupt(ISR_timer);
  rtc.standbyMode();  
}

void blink(int pin, int n_blinks)
{
  for(int i=0;i<n_blinks;i++)
  {
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(500);
  }
}

void resetTimer() {
  byte seconds = 0;
  byte minutes = 0;
  byte hours = 0;
  byte day = 1;
  byte month = 1;
  byte year = 1;
  
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);
  rtc.setAlarmTime(timerHours, timerMinutes, timerSeconds);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
}

void loop() {
  if(ir_selector_pressed){
    blink(LED_BUILTIN, 4);
    ir_selector_pressed = false;
    rtc.standbyMode();  
  }
  else if(wake_timer){
    blink(LED_BUILTIN, 3);
    wake_timer = false;
    resetTimer();
    rtc.standbyMode();
  }
  else {
    blink(LED_BUILTIN, 2);
    ir_selector_pressed = false;
    rtc.standbyMode();  
  }
}

void ISR_timer(){
  wake_timer = true;
}

static unsigned long previousStateChangeMillis = 0;
static bool previousPinState = LOW;

void ISR_pin() {

  int now = millis();
  bool pinState = digitalRead(button);
  
  if (pinState != previousPinState) { // ignore pin changes of pins other than SELECTOR_BTN
    if (pinState == HIGH) { // only rising events
      if ((now - previousStateChangeMillis) > debounceTime) { // debounce
        ir_selector_pressed = true;
      }
    }
    previousPinState = pinState;
    previousStateChangeMillis = now;
  }
  
}
