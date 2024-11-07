#include <Arduino.h>
#include "src/rfid/MFRC522v2.h"
#include "src/rfid/MFRC522DriverSPI.h"
#include "src/rfid/MFRC522DriverPinSimple.h"
#include "src/rfid/MFRC522Debug.h"
#include "src/lib/gpio.h"
//#include "src/lib/usart.h"

MFRC522DriverPinSimple ss_1_pin(10);
MFRC522DriverSPI driver_1{ss_1_pin};
MFRC522 reader{driver_1};

//USART serial;

void blink(uint8_t *port, uint8_t pin, uint16_t period);
void flash(uint8_t *port, uint8_t pin, uint16_t OnPeriod, uint16_t OffPeriod);

void setup(){
  Serial.begin(115200);
  DDRd = 0xE0;

  Serial.print("INIT ");

  if(reader.PCD_Init() == 1){
    Serial.println("OK");
  } else {
    Serial.println("FAIL");
    while(1);
  }

  bool init = 0;
  char rxbuff[50];
  while (init == 0){
    if (Serial.available()){
      Serial.readBytesUntil('\n', rxbuff, 50);
      if (strcmp(rxbuff, "SAPEI_INIT") == 0)
        init = 1;
      else
        memset(rxbuff, 0, 50);
    }
    flash(&PORTD, 5, 50, 950);
  }
  Serial.println("CONNECTED");
}

void loop(){
  if (reader.PICC_IsNewCardPresent() && reader.PICC_ReadCardSerial()) {
    MFRC522Debug::PrintUID(Serial, reader.uid);
    Serial.println();
    // Halt PICC.
    reader.PICC_HaltA();
    // Stop encryption on PCD.
    reader.PCD_StopCrypto1();
    delay(1000);
  }
}

void blink(uint8_t *port, uint8_t pin, uint16_t period){
    static uint32_t timestamp = millis();
    if (millis() >= timestamp + period){
        *port ^= _BV(pin);
        timestamp = millis();
    }
}

void flash(uint8_t *port, uint8_t pin, uint16_t OnPeriod, uint16_t OffPeriod){
    static uint32_t timestamp = millis();
    if (millis() >= timestamp + OnPeriod){
        *port &= ~_BV(pin);
    } else {
        *port |= _BV(pin);
    }
    if (millis() >= timestamp + OnPeriod + OffPeriod)
        timestamp = millis();
}
