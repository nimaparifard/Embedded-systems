#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>

void lcd_write(char c) {
    PORTA = c;
    PORTB |= 0x03;
    _delay_ms(100);
    PORTB &= ~0x01;
}

void lcd_command(char c) {
    PORTA = c;
	PORTB &= ~0x02;
	PORTB |= 0x01;
	_delay_ms(5);
	PORTB &= ~0x01;
}

int main() {

    DDRA = 0xff;
    DDRB = 0x03;

    lcd_command(0x38);
	lcd_command(0x06);
	lcd_command(0x0E);
	lcd_command(0x01);
	lcd_command(0x80);

    lcd_write('H');
	lcd_write('E');
	lcd_write('L');
	lcd_write('L');
	lcd_write('O');
    lcd_write(' ');
    lcd_write('W');
	lcd_write('O');
	lcd_write('R');
	lcd_write('L');
	lcd_write('D');
    
    while(1){

    }
}