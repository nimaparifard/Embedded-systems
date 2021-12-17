#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
int main(void)
{
    int a;

    DDRA = 0x00;
    PORTA = 0x01;

    DDRB = 0xFF;
    PORTB = 0x00;

    while (1)
    {
        if (!(PINA & 1)) {
            for(a=0 ; a<8 ; a++) {
                PORTB |= (1 << a);
                _delay_ms(300);
            }
            for(a=7 ; a>=0 ; a--) {
                PORTB &= ~(1 << a);
                _delay_ms(300);
            }
        }
    }
}