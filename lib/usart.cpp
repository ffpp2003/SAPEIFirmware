#include <avr/io.h>
#include <stdint.h>
#include "usart.h"

USART::USART(uint32_t baud, bool rxen, bool txen, uint8_t chsize, uint8_t mode,
             uint8_t parity, bool stop){
    setRxen(rxen);
    setTxen(txen);
    setStop(stop);
    setChsize(chsize);
    setMode(mode);
    setParity(parity);
    setBaud(baud);
}

void USART::setRxen(const bool rxen){
    UCSR0B |= rxen << RXEN0;
    this->rxen = getRxen();
}

void USART::setTxen(const bool txen){
    UCSR0B |= txen << TXEN0;
    this->txen = getTxen();
}

void USART::setStop(const bool stop){
    UCSR0C |= stop << USBS0;
    this->stop = getStop();
}

void USART::setChsize(const uint8_t chsize){
    if (chsize < 7){
        UCSR0B |= ((chsize & 4) << UCSZ02);
        UCSR0C |= ((chsize & 3) << UCSZ00);
    }
    this->chsize = getChsize();
}

void USART::setMode(const uint8_t mode){
    if (mode < 3){
        UCSR0C |= mode << UMSEL00;
    }
    this->mode = getMode();
}

void USART::setParity(const uint8_t parity){
    if (mode < 3){
        UCSR0C |= mode << UPM00;
    }
    this->mode = getParity();
}

void USART::setBaud(const uint32_t baud){
    uint16_t ubrrn = (uint16_t)(((uint32_t)F_CPU)/((uint32_t)(16*baud)) - 1);
    if (baud == 115200 || baud == 22800)
        ubrrn++;
    UBRR0H = (ubrrn >> 8) & 255;
    UBRR0L = ubrrn & 255;
    this->baud = getBaud();
}

bool USART::getRxen(){
    return (UCSR0B & _BV(RXEN0)) >> RXEN0;
}

bool USART::getTxen(){
    return (UCSR0B & _BV(TXEN0)) >> TXEN0;
}

bool USART::getStop(){
    return (UCSR0B & _BV(USBS0)) >> USBS0;
}

uint8_t USART::getChsize(){
    return (((UCSR0B & _BV(UCSZ02)) >> (UCSZ02 - 2))) |
           (((UCSR0C & _BV(UCSZ01)) >> (UCSZ01 - 1))) |
           ((UCSR0C & _BV(UCSZ00)) >> UCSZ00);
}

uint8_t USART::getMode(){
    return (UCSR0C & (_BV(UMSEL00) | _BV(UMSEL01))) >> UMSEL00;
}

uint8_t USART::getParity(){
    return (UCSR0C & (_BV(UPM00) | _BV(UPM01))) >> UPM00;
}

uint32_t USART::getBaud(){
    uint16_t ubrrn = (UBRR0H << 8) | UBRR0L;
    if (ubrrn == 8 || ubrrn == 34)
        ubrrn--;
    return F_CPU/(16 * (ubrrn + 1));;
}

void USART::send(const uint8_t data){
    while ((UCSR0A & _BV(UDRE0)) == 0);
    UDR0 = data;
}

void USART::send(const uint8_t *data, const uint16_t len){
    uint16_t i = 0;
    for (i = 0; i < len; i++){
        send(data[i]);
    }
}

uint16_t USART::send(const char *data){
    uint16_t i;
    for (i = 0; data[i] != '\0'; i++){
        send(data[i]);
    }
    return i;
}

void USART::receive(uint8_t *data){
    while( !(UCSR0A & _BV(RXC0)) ) ;
    *data = UDR0;
}

void USART::receive(uint8_t *data, const uint16_t max){
    uint16_t size = 0;
    do{
        receive(&data[size]);
        size++;
    } while(size < max);
}

uint16_t USART::receive(char *data, const uint16_t max){
    uint16_t size = 0;
    do{
        receive((uint8_t *) &data[size]);
        size++;
    } while (data[size - 1] != '\n' && data[size - 1] != '\r' && size < max);
    return size;
}
