#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/pgmspace.h>

unsigned char sev_seg_nums[10] PROGMEM = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

void timer1_init()
{
    // prescaler = clk/64
    TCCR1B |= (1 << CS11);
    TCCR1B |= (1 << CS10);

    // initialization of timer 1 register
    TCNT1 = 0;
}

void timer0_init()
{
    //prescaler = clk/64
    TCCR0 |= (1 << CS00) | (1 << CS01);

    // initialization of timer 0 register
    TCNT0 = 0;
}

int main(void)
{
    DDRD = 0xff;
    DDRC = 0x0f;

    unsigned int ones = 0;
    unsigned int tens = 0;
    unsigned int hundreds = 0;
    unsigned int thousends = 0;

    unsigned int display_counter = 1;

    PORTC = ~display_counter;

    timer0_init();
    timer1_init();

    while (1)
    {
        // 10 ms
        if (TCNT0 >= 156) {
            if (display_counter < 8) {
                display_counter = display_counter << 1;
                PORTC = ~(display_counter);
            }
            else {
                display_counter = 0x01;
                PORTC = ~(display_counter);
            }

            PORTD = pgm_read_byte(&(sev_seg_nums[3]));

            if (display_counter & 1){
                PORTD = pgm_read_byte(&(sev_seg_nums[ones]));
            }

            if (display_counter & 2){
                PORTD = pgm_read_byte(&(sev_seg_nums[tens]));
            }

            if (display_counter & 4){
                PORTD = pgm_read_byte(&(sev_seg_nums[hundreds]));
            }

            if (display_counter & 8){
                PORTD = pgm_read_byte(&(sev_seg_nums[thousends]));
            }

            TCNT0 = 0;
        }

        // 1 sec
        if (TCNT1 >= 15625){
            ones++;
            tens += ones / 10;
            ones = ones % 10;

            hundreds += tens / 10;
            tens = tens % 10;

            thousends += hundreds / 10;
            hundreds = hundreds % 10;

            if (thousends == 9 & hundreds == 9 & tens == 9 & ones == 9) {
                ones = 0;
                tens = 0;
                hundreds = 0;
                thousends = 0;
            } 

            TCNT1 = 0;
        }
    }
}