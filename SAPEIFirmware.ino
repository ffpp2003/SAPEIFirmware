#include <Arduino.h>
#include "src/rfid/MFRC522v2.h"
#include "src/rfid/MFRC522DriverSPI.h"
#include "src/rfid/MFRC522DriverPinSimple.h"
#include "src/rfid/MFRC522Debug.h"
#include "src/lib/gpio.h"
#include "src/SAPEICore/Error.h"

#define RED_LED PORTdBits.b5
#define YEL_LED PORTdBits.b6
#define GRN_LED PORTdBits.b7
#define BUZZER PORTbBits.b0

#define LEDS_PORT &PORTD
#define BUZZER_PORT &PORTB

#define RED 5
#define YEL 6
#define GRN 7
#define BZZ 0

#define SIMULTANEOUS_CONTROL_NUMBER 2

#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0
#define USE_TIMER_1 true

#include "TimerInterrupt.h"

MFRC522DriverPinSimple ss_1_pin(10);
MFRC522DriverSPI driver_1{ss_1_pin};
MFRC522 reader{driver_1};

void blink(uint8_t channel, uint8_t *port, uint8_t pin, uint16_t period, uint8_t repetitions = 0);
void flash(uint8_t channel, uint8_t *port, uint8_t pin, uint16_t period, uint16_t onTime, uint8_t repetitions = 0);
void pinsControlHandler();
void pinControlHandler(uint8_t channel);
void pinControl(uint8_t channel, uint8_t *port, uint8_t pin, uint8_t type, uint8_t repetitions, uint16_t period, uint16_t onTime = 0);

typedef struct{
  volatile bool alreadyControlling;
  volatile uint8_t *conPort;
  volatile uint8_t conPin;
  volatile uint8_t conType;
  volatile uint8_t conRepetitions;
  volatile uint16_t conPeriod;
  volatile uint16_t conOnTime;
  volatile uint16_t passes;
  volatile uint16_t repetitions;
} pinStatus_s;

pinStatus_s pins[SIMULTANEOUS_CONTROL_NUMBER];

void setup(){
  Serial.begin(115200);
  DDRd = 0xE0;
  DDRb |= _BV(PORTB0);

  Serial.print("INIT ");

  ITimer1.init();
  if (!ITimer1.attachInterruptInterval(10, pinsControlHandler)){
    Serial.println("FAIL");
    while(1);
  }

  if(reader.PCD_Init() == 1){
    Serial.println("OK");
  } else {
    Serial.println("FAIL");
    while(1);
  }

  bool init = 0;
  char rxbuff[50] = {0};
  while (init == 0){
    if (Serial.available()){
      Serial.readBytesUntil('\n', rxbuff, 50);
      if (strcmp(rxbuff, "SAPEI_INIT") == 0)
        init = 1;
      else
        memset(rxbuff, 0, 50);
    }
    flash(0, LEDS_PORT, RED, 1000, 50);
  }
  Serial.println("CONNECTED");
}

void loop(){
  static uint32_t readerTimestamp = millis();

  if (reader.PICC_IsNewCardPresent() && reader.PICC_ReadCardSerial() && millis() >= readerTimestamp + 5000) {
    MFRC522Debug::PrintUID(Serial, reader.uid);
    Serial.println();
    reader.PICC_HaltA();
    reader.PCD_StopCrypto1();
    readerTimestamp = millis();
  }

  if (Serial.available()){
    char rxbuff[5] = {0};
    Serial.readBytesUntil('\n', rxbuff, 5);
    uint16_t code = atoi(rxbuff);
    switch (code){
        case OK:
            blink(0, LEDS_PORT, GRN, 500, 10);
            flash(1, BUZZER_PORT, BZZ, 100, 75, 1);
            break;
        case TR_NOT_ENOUGH_FUNDS:
            blink(0, LEDS_PORT, RED, 500, 10);
            flash(1, BUZZER_PORT, BZZ, 500, 300, 3);
            break;
        case TR_JUST_ENOUGH_FUNDS:
            blink(0, LEDS_PORT, YEL, 500, 10);
            flash(1, BUZZER_PORT, BZZ, 100, 75, 3);
            break;
        case TR_CLIENT_NOT_FOUND:
            flash(0, LEDS_PORT, RED, 100, 50, 25);
            flash(1, BUZZER_PORT, BZZ, 100, 50, 15);
            break;
        default:
            break;
    }
  }
}

/**
 * @brief Funcion tipo "wrapper" para parpadear una salida
 */
void blink(uint8_t channel, uint8_t *port, uint8_t pin, uint16_t period, uint8_t repetitions){
    pinControl(channel, port, pin, 1, repetitions, period);
}

/**
 * @brief Funcion tipo "wrapper" para parpadear por un periodo determinado una salida
 */
void flash(uint8_t channel, uint8_t *port, uint8_t pin, uint16_t period, uint16_t onTime, uint8_t repetitions){
    pinControl(channel, port, pin, 2, repetitions, period, onTime);
}

/**
 * @brief Funcion que administra el estado de las salidas en base a su configuracion (blink o flash)
 */
void pinControlHandler(uint8_t channel){ // ISR ONLY!!!
  if(pins[channel].alreadyControlling == 1){
    switch (pins[channel].conType){
      case 1: // blink
        if (pins[channel].passes < pins[channel].conPeriod/2)
          *(pins[channel].conPort) |= _BV(pins[channel].conPin);
        else
          *(pins[channel].conPort) &= ~_BV(pins[channel].conPin);
        break;
      case 2: // flash
        if (pins[channel].passes < pins[channel].conOnTime)
          *(pins[channel].conPort) |= _BV(pins[channel].conPin);
        else
          *(pins[channel].conPort) &= ~_BV(pins[channel].conPin);
        break;
    }
    if (pins[channel].passes >= pins[channel].conPeriod){
      if (pins[channel].repetitions >= pins[channel].conRepetitions){
        pins[channel].alreadyControlling = 0;
        pins[channel].conPort == NULL;
        pins[channel].conPin = 0;
        pins[channel].conType = 0;
        pins[channel].conPeriod = 0;
        pins[channel].conOnTime = 0;
        pins[channel].conRepetitions = 0;
        pins[channel].passes = 0;
        pins[channel].repetitions = 0;
      } else {
        pins[channel].repetitions++;
        pins[channel].passes = 0;
      }
    } else {
      pins[channel].passes++;
    }
  }
}

/**
 * @brief Funcion que define los parametros para el control de las salidas
 */
void pinControl(uint8_t channel, uint8_t *port, uint8_t pin, uint8_t type, uint8_t repetitions, uint16_t period, uint16_t onTime){
  if(pins[channel].alreadyControlling == 0){
    pins[channel].conPort = port;
    pins[channel].conPin = pin;
    pins[channel].conType = type;
    pins[channel].conPeriod = period/10;
    pins[channel].conOnTime = onTime/10;
    pins[channel].conType = type;
    pins[channel].conRepetitions = repetitions;
    pins[channel].repetitions = 0;
    pins[channel].passes = 0;
    pins[channel].alreadyControlling = 1;
  }
}

/**
 * @brief Funcion para manipular todos los pines definidos al "mismo tiempo"
 */
void pinsControlHandler(){
  for (uint8_t i = 0; i < SIMULTANEOUS_CONTROL_NUMBER; i++)
    pinControlHandler(i);
}
