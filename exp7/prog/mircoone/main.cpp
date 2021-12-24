#define F_CPU 1000000UL                                         /* 1MHz */
#define USART_BAUDRATE 4800                                     /* bit or baud per second */
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1) /* BAUD_PRESCALE = 51 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <avr/eeprom.h>

#define LcdDataBus PORTA
#define LcdControlBus PORTB
#define keypadPort PORTC
#define keypadPin PINC

#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2

// username password variables
char true_id[] = "1111";
char true_password[] = "1111";

char user_id[4];
char user_password[4];

int credential_digit_counter;

bool id_entered = 0;
bool access_granted = 0;

//keypad variables
char keypad_shift[4] PROGMEM  = {0xFE, 0xFD, 0xFB, 0xF7};
char keypad_layout[16] PROGMEM  = {'7', '8', '9', '/', '4', '5', '6', '*', '1', '2', '3', '-', 'C', '0', '=', '+'};

//temperature variables
volatile int temp_counter = 0;

void run_stepper_motor(char selected_key);
void authenticate(char selected_key);

void UART_init() {
    // RX complete intrrupt and TX complete intrrupt enable
    UCSRB |= (1 << RXEN) | (1 << TXEN) | (1 << RXCIE) | (1 << TXCIE);

    // 8 bit data unit size
    UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);

    UBRRH = (BAUD_PRESCALE >> 8);
    UBRRL = BAUD_PRESCALE;
}

void UART_write_char(char data) {
    // wait while register is free
    while (!(UCSRA & (1 << UDRE))) {};
    UDR = data;
}

void lcd_write_char(char c) {
    LcdDataBus = c;
    LcdControlBus |= (1 << LCD_RS);
    LcdControlBus &= ~(1 << LCD_RW);
    LcdControlBus |= (1 << LCD_EN);
    _delay_us(1);
    LcdControlBus &= ~(1 << LCD_EN);
    _delay_ms(1);
}

void lcd_command(char c) {
    LcdDataBus = c;
    LcdControlBus &= ~(1 << LCD_RS);
    LcdControlBus &= ~(1 << LCD_RW);
    LcdControlBus |= (1 << LCD_EN);
    _delay_us(1);
    LcdControlBus &= ~(1 << LCD_EN);
    _delay_ms(3);
}

void lcd_write_string(char *c) {
    for (int i = 0; c[i] != 0; i++) {
        lcd_write_char(c[i]);
    }
}

void lcd_next_line() {
    lcd_command(0xC0);
}

void lcd_init() {
    DDRA = 0xFF;
    DDRB = 0x07;
    _delay_ms(20);
    lcd_command(0x38);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
    _delay_ms(2);
    lcd_command(0x80);
}

void lcd_cursor_at_first() {
    lcd_command(0x02);
}

void lcd_clear() {
    lcd_command(0x01);
    lcd_command(0x80);
}

void intrrupts_init() {
    GICR = 1 << INT0;
    MCUCR |= (1 << ISC01);
}

char get_selected_key() {
    int row=0, col=-1, position=-1;
    for (row=0 ; row<4 ; row++) {
        keypadPort = pgm_read_byte(&keypad_shift[row]);
        if(!(keypadPin & (1 << 4))) {col=0;}
        if(!(keypadPin & (1 << 5))) {col=1;}
        if(!(keypadPin & (1 << 6))) {col=2;}
        if(!(keypadPin & (1 << 7))) {col=3;}
        if(col != -1) {
            position = row * 4 + col;
            keypadPort = 0xF0;
            return pgm_read_byte(&keypad_layout[position]);
        }
        _delay_ms(20);
    }
    keypadPort = 0xF0;
}


int main(void) {
    intrrupts_init();
    lcd_init();
    UART_init();

    sei();

    DDRC = 0x0F;
    DDRD |= (1 << 3) | (1 << 4);
    keypadPort = 0xF0;

    credential_digit_counter = -1;

    while(!eeprom_is_ready()) {};
    // eeprom_write_byte(0, 0);
    id_entered = eeprom_read_byte(0) & (1 << 0);
    access_granted = eeprom_read_byte(0) & (1 << 1);

    if (!access_granted) {
        lcd_clear();
        lcd_write_string("enter your id:"); 
        lcd_next_line();
    } else {
        run_stepper_motor(NULL);
    }

    while (1) {}
}

void check_credentials() {
    access_granted = true;
    for(int i=0 ; i<strlen(true_password) ; i++) {
        access_granted &= user_password[i] == true_password[i];
    }

    for(int i=0 ; i<strlen(true_id) ; i++) {
        access_granted &= user_id[i] == true_id[i];
    }

    if (access_granted) {
        eeprom_write_byte(0, eeprom_read_byte(0) | (1 << 0));
        eeprom_write_byte(0, eeprom_read_byte(0) | (1 << 1));
    }
}

bool validate_credentials(char token) {
    if (token == '/') { return false; }
    if (token == '*') { return false; }
    if (token == '-') { return false; }
    if (token == '+') { return false; }
    if (token == '=') { return false; }
    if (token == 'C') { return false; }
    return true;
}

void run_stepper_motor(char selected_key) {
    lcd_clear();
    lcd_write_string("step no:");
    lcd_next_line();

    if (selected_key == NULL) { return; }
    if (validate_credentials(selected_key)) {
        lcd_write_char(selected_key);
        _delay_ms(200);
        UART_write_char(selected_key);
        lcd_clear();
        lcd_write_string("step no:");
        lcd_next_line();
    } else {
        lcd_clear();
        lcd_write_string("invalid input!");
        lcd_next_line();
        lcd_write_string("try again...");
        _delay_ms(1000);
        lcd_clear();
        lcd_write_string("step no:");
        lcd_next_line();
    }
}

void authenticate(char selected_key) {
    if (!id_entered) {
        if (credential_digit_counter == -1) { 
            credential_digit_counter = 0;
        }
        // char selected_key = get_selected_key();
        if (validate_credentials(selected_key)) {
            user_id[credential_digit_counter++] = selected_key;
            lcd_write_char(selected_key);
            if(credential_digit_counter >= strlen(true_id)) {
                _delay_ms(500);
                lcd_clear();
                lcd_write_string("enter password:"); 
                lcd_next_line();
                id_entered = true;
                credential_digit_counter = -1; 
            }
        }
    } else {
        if (credential_digit_counter == -1) { 
            credential_digit_counter = 0; 
        }
        // char selected_key = get_selected_key();
        if (validate_credentials(selected_key)) {
            user_password[credential_digit_counter++] = selected_key;
            lcd_write_char('*');
            if(credential_digit_counter >= strlen(true_password)) { 
                _delay_ms(500);
                check_credentials();
                if(access_granted) {
                    PORTD |= (1 << PD3);
                    lcd_clear();
                    lcd_write_string("Access approved!");
                    _delay_ms(1000);
                    PORTD &= ~(1 << PD3);
                    lcd_clear();
                    run_stepper_motor(NULL);
                } else {
                    PORTD |= (1 << PD4);
                    lcd_clear();
                    lcd_write_string("Access denied!");
                    _delay_ms(1000);
                    PORTD &= ~(1 << PD4);
                    id_entered = false;
                    lcd_clear();
                    lcd_write_string("enter your id:"); 
                    lcd_next_line();
                }
                credential_digit_counter = -1; 
            }
        }
    }
}


ISR(INT0_vect)
{
    char selected_key = get_selected_key();

    if (!access_granted) {
        authenticate(selected_key);
    } else {
        run_stepper_motor(selected_key);
    }
}


ISR(USART_RXC_vect) {
    if (access_granted) {
        if (temp_counter < 30) {
            lcd_write_char(UDR);
            temp_counter++;
        } else {
            _delay_ms(2000);
            lcd_clear();
            temp_counter = 0;
            run_stepper_motor(NULL);
        }
    }
}

