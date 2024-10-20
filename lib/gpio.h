#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Estructura de bits de un byte.
 */
typedef struct {
  volatile unsigned b0 : 1;
  volatile unsigned b1 : 1;
  volatile unsigned b2 : 1;
  volatile unsigned b3 : 1;
  volatile unsigned b4 : 1;
  volatile unsigned b5 : 1;
  volatile unsigned b6 : 1;
  volatile unsigned b7 : 1;
} bitfield_s;

/**
 * @brief Registro de hardware personalizado.
 *
 * Union que permite acceder a los registros de hardware
 * de forma directa, como un registro de 8 bits, o de forma
 * parcial, modificando/leyendo un solo bit a la vez.
 */
typedef union {
  volatile uint8_t *REG;
  bitfield_s *bits;

} bitreg_u;

extern bitreg_u PORTb_bitreg; /// Registro personalizado para PORTB.
#define PORTb *(PORTb_bitreg.REG)
#define PORTbBits (*(PORTb_bitreg.bits))
extern bitreg_u PINb_bitreg; ///Registro personalizado para PINB.
#define PINb *(PINb_bitreg.REG)
#define PINbBits (*(PINb_bitreg.bits))
extern bitreg_u DDRb_bitreg; ///Registro personalizado para DDRB.
#define DDRb *(DDRb_bitreg.REG)
#define DDRbBits (*(DDRb_bitreg.bits))

extern bitreg_u PORTc_bitreg; ///Registro personalizado para PORTC.
#define PORTc *(PORTc_bitreg.REG)
#define PORTcBits (*(PORTc_bitreg.bits))
extern bitreg_u PINc_bitreg; ///Registro personalizado para PINC.
#define PINc *(PINc_bitreg.REG)
#define PINcBits (*(PINc_bitreg.bits))
extern bitreg_u DDRc_bitreg; ///Registro personalizado para DDRC.
#define DDRc *(DDRc_bitreg.REG)
#define DDRcBits (*(DDRc_bitreg.bits))

extern bitreg_u PORTd_bitreg; ///Registro personalizado para PORTD.
#define PORTd *(PORTd_bitreg.REG)
#define PORTdBits (*(PORTd_bitreg.bits))
extern bitreg_u PINd_bitreg; ///Registro personalizado para PINC.
#define PINd *(PINd_bitreg.REG)
#define PINdBits (*(PINd_bitreg.bits))
extern bitreg_u DDRd_bitreg; ///Registro personalizado para DDRD.
#define DDRd *(DDRd_bitreg.REG)
#define DDRdBits (*(DDRd_bitreg.bits))

#endif
