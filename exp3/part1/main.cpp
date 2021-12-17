#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/pgmspace.h>

unsigned char sev_seg_nums[10] PROGMEM = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

void timer1_init()
{
    // prescaler = clk/64
    TCCR1B |= (1 << CS11);
    TCCR1B |= (1 << CS10);

    // initialization of timer 1 register
    TCNT1 = 0;
}


int main(void)
{
    DDRD = 0xff;
    
    int i=0;
    PORTD = pgm_read_byte(&(sev_seg_nums[i]));  
    
    timer1_init();

    while(1)
    {   
        // 1 sec
        if (TCNT1 >= 15625)
        {
            i++;
            PORTD = pgm_read_byte(&(sev_seg_nums[i % 10]));   
            TCNT1 = 0;             
        }    
    }
}