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

#define LEDS_PORT PORTD
#define BUZZER_PORT PORTB

#define RED 5
#define YEL 6
#define GRN 7
#define BZZ 0

MFRC522DriverPinSimple ss_1_pin(10);
MFRC522DriverSPI driver_1{ss_1_pin};
MFRC522 reader{driver_1};

void blink(uint8_t *port, uint8_t pin, uint16_t period);
void flash(uint8_t *port, uint8_t pin, uint16_t onTime, uint16_t period);
void manageFeedback(uint16_t code = 0xffff);

void setup(){
  Serial.begin(115200);
  DDRd = 0xE0;
  DDRb |= _BV(PORTB0);

  Serial.print("INIT ");

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
    flash(&PORTD, 5, 50, 1000);
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

  if(Serial.available()){
    char rxbuff[5] = {0};
    Serial.readBytesUntil('\n', rxbuff, 5);
    manageFeedback(atoi(rxbuff));
  }
  manageFeedback();
}

void blink(uint8_t *port, uint8_t pin, uint16_t period){
    static uint32_t timestamp;
    if (millis() >= timestamp + period){
        *port &= ~_BV(pin);
        timestamp = millis();
    }
    if (millis() >= timestamp + period/2)
        *port |= _BV(pin);
}

void flash(uint8_t *port, uint8_t pin, uint16_t onTime, uint16_t period){
    static uint32_t timestamp;
    if (millis() >= timestamp + period)
        timestamp = millis();
    if (millis() >= timestamp + onTime){
        *port &= ~_BV(pin);
    } else {
        *port |= _BV(pin);
    }
}

void manageFeedback(uint16_t code){
    static uint32_t timestamp;
    static uint8_t mode = 0, led = 0;

    if (code == 0xffff){
        switch (mode){
            case 0:
                RED_LED = YEL_LED = GRN_LED = BUZZER = 0;
                break;
            case 1:
                blink(&LEDS_PORT, led, 500);
                flash(&BUZZER_PORT, BZZ, 50, 5500);
                break;
            case 2:
                flash(&BUZZER_PORT, BZZ, 850, 5500);
                break;
            default:
                break;
        }
        if (millis() >= timestamp + 5000){
            timestamp = millis();
            mode = 0;
        }
    } else {
        switch (code){
            case OK:
                mode = 1;
                GRN_LED = 1;
                //BUZZER = 1;
                led = GRN;
                break;
            case TR_NOT_ENOUGH_FUNDS:
                mode = 1;
                RED_LED = 1;
                //BUZZER = 1;
                led = RED;
                break;
            case TR_JUST_ENOUGH_FUNDS:
                mode = 1;
                YEL_LED = 1;
                //BUZZER = 1;
                led = YEL;
                break;
            case TR_CLIENT_NOT_FOUND:
                mode = 2;
                RED_LED = 1;
                //BUZZER = 1;
                led = RED;
                break;
            default:
                break;
        }
        timestamp = millis();
    }
}
