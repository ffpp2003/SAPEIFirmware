#include "gpio.h"

bitreg_u PORTb_bitreg = {&PORTB}; /// Registro personalizado para PORTB.
bitreg_u PINb_bitreg = {&PINB}; /// Registro personalizado para PINB.
bitreg_u DDRb_bitreg = {&DDRB}; /// Registro personalizado para DDRB.

bitreg_u PORTc_bitreg = {&PORTC}; /// Registro personalizado para PORTC.
bitreg_u PINc_bitreg = {&PINC}; /// Registro personalizado para PINC.
bitreg_u DDRc_bitreg = {&DDRC}; /// Registro personalizado para DDRC.

bitreg_u PORTd_bitreg = {&PORTD}; /// Registro personalizado para PORTD.
bitreg_u PINd_bitreg = {&PIND}; /// Registro personalizado para PINC.
bitreg_u DDRd_bitreg = {&DDRD}; /// Registro personalizado para DDRD.
