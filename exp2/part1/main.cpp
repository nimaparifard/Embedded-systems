#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRA = 0x01;

    while (1)
    {
        _delay_ms(500);
        PORTA ^= 1;
    }
}