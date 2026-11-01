/*
Pragotron slave clock controller - nRF52840 Supermini
System ON deep sleep, RTC2 for 1Hz tick, USB Serial disabled after boot for production use.
*/

#include <Arduino.h>
#include "nrf.h"
//#include "Adafruit_TinyUSB.h"


const uint8_t PIN_POLARITY  = 11;      // P0.10
const uint8_t PIN_DRV_EN    = 12;      // P1.11
const uint8_t PIN_BUTTON    = 14;      // P1.13
const uint8_t PIN_DRV_SLEEP = 15;      // P0.2
const uint8_t PIN_BOOST_EN  = 16;      // P0.29
//const uint8_t PIN_LED       = 15;      // P0.15

#define DEBUG_SERIAL 0   

volatile uint8_t sec = 0;
volatile bool minute_passed = false;
volatile bool event_button = false;
volatile bool in_move_hand = false;
bool polarity = false;

void setupRTC2()
{
  NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal;
  NRF_CLOCK->TASKS_LFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;

  NRF_RTC2->TASKS_STOP = 1;
  NRF_RTC2->PRESCALER = 0;
  NRF_RTC2->CC[0] = 32767;
  NRF_RTC2->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
  NRF_RTC2->INTENSET = RTC_INTENSET_COMPARE0_Msk;
  NRF_RTC2->TASKS_CLEAR = 1;

  NVIC_SetPriority(RTC2_IRQn, 3);
  NVIC_ClearPendingIRQ(RTC2_IRQn);
  NVIC_EnableIRQ(RTC2_IRQn);

  NRF_RTC2->TASKS_START = 1;
}

extern "C" void RTC2_IRQHandler(void)
{
  if (NRF_RTC2->EVENTS_COMPARE[0])
  {
    NRF_RTC2->EVENTS_COMPARE[0] = 0;
    NRF_RTC2->TASKS_CLEAR = 1;

    sec++;
    if (sec >= 60)
    {
      sec = 0;
      minute_passed = true;
    }
  }
}

void onButtonPress()
{
  if (!in_move_hand)
    event_button = true;
}

void move_hand()
{
  in_move_hand = true;
  digitalWrite(PIN_LED, HIGH);

  digitalWrite(PIN_BOOST_EN, HIGH);
  digitalWrite(PIN_DRV_SLEEP, HIGH);
  delay(400);

  digitalWrite(PIN_POLARITY, polarity);
  digitalWrite(PIN_DRV_EN, HIGH);
  polarity = !polarity;
  delay(500);

  digitalWrite(PIN_DRV_EN, LOW);
  delay(100);

  digitalWrite(PIN_BOOST_EN, LOW);
  digitalWrite(PIN_DRV_SLEEP, LOW);

#if DEBUG_SERIAL
  Serial.println("Moved hand.");
#endif
  digitalWrite(PIN_LED, LOW);
  in_move_hand = false;
}

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);  // start OFF

  pinMode(PIN_POLARITY, OUTPUT);
  pinMode(PIN_DRV_EN, OUTPUT);
  pinMode(PIN_DRV_SLEEP, OUTPUT);
  pinMode(PIN_BOOST_EN, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  digitalWrite(PIN_DRV_EN, LOW);
  digitalWrite(PIN_DRV_SLEEP, LOW);
  digitalWrite(PIN_BOOST_EN, LOW);

  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButtonPress, FALLING);

#if DEBUG_SERIAL
  Serial.begin(115200);
  delay(1000);
  Serial.println("Controller start");
#else
  NRF_USBD->ENABLE = 0;
  // NRF_POWER->DCDCEN = 1;   
#endif

  setupRTC2();
}

void loop()
{

  digitalWrite(PIN_LED, HIGH); // indicate loop pass

  if(minute_passed)
  {
    minute_passed = false;
    move_hand();
  }

  if(event_button)
  {
    event_button = false;
    move_hand();
    delay(500);
    while (digitalRead(PIN_BUTTON) == LOW)
      move_hand();
  }

  digitalWrite(PIN_LED, LOW);

  __WFE();
  __SEV();  
  __WFE();


  __WFE();
  __SEV();  
  __WFE();


}

