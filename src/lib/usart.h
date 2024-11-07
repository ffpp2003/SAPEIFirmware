#ifndef USART_H
#define USART_H
#include <avr/io.h>
#include <stdint.h>

#define SIZE_5_BITS 0
#define SIZE_6_BITS 1
#define SIZE_7_BITS 2
#define SIZE_8_BITS 3
#define SIZE_9_BITS 7

#define PARITY_NO 0
#define PARITY_EVEN 2
#define PARITY_ODD 3

#define STOP_ONE 0
#define STOP_TWO 1

class USART {
    private:
        bool rxen, txen, stop;
        uint8_t chsize, mode, parity;
        uint32_t baud;

    public:
        USART(uint32_t baud = 115200, bool rxen = 1, bool txen = 1, uint8_t chsize = SIZE_8_BITS,
              uint8_t mode = 0, uint8_t parity = PARITY_NO, bool stop = STOP_ONE);
        void setRxen(const bool rxen);
        void setTxen(const bool txen);
        void setStop(const bool stop);
        void setChsize(const uint8_t chsize);
        void setMode(const uint8_t mode);
        void setParity(const uint8_t parity);
        void setBaud(const uint32_t baud);
        bool getRxen();
        bool getTxen();
        bool getStop();
        uint8_t getChsize();
        uint8_t getMode();
        uint8_t getParity();
        uint32_t getBaud();
        void send(const uint8_t data);
        void send(const uint8_t *data, const uint16_t len);
        uint16_t send(const char *data);
        void receive(uint8_t *data);
        void receive(uint8_t *data, const uint16_t len);
        uint16_t receive(char *data, const uint16_t max);

};
#endif
