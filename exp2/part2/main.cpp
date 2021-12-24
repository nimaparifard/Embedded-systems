#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{

    DDRA = 0x00;
    PORTA = 0x01;

    DDRB = 0xFF;
    PORTB = 0x00;

    while (1)
    {
        while (PINA & 1);
        PORTB = ~PORTB;
        _delay_ms(500);
    }


}