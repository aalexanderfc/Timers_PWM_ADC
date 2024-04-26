#include "LEDTimer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include <stdio.h>

volatile uint8_t ledPower = 0;  // Variable for LED power (0-255)
volatile uint16_t ledPeriod = 1000;  // Default period for LED blinking (1 second in ms)

void LEDTimer_Init() {
    // Configure Timer1 for CTC mode
    TCCR1A = 0;  // Normal port operation
    TCCR1B = (1 << WGM12) | (1 << CS11);  // CTC mode, Prescaler 8
    OCR1A = ((16000000 / 8) * ledPeriod / 1000) - 1;  // Set CTC compare value
    TIMSK1 = (1 << OCIE1A);  // Enable compare match interrupt

    // Configure Timer2 for Fast PWM mode
    DDRD |= (1 << PD3);  // Set PD3 as output (OC2B)
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);  // Non-inverting mode, Fast PWM
    TCCR2B = (1 << CS21);  // Prescaler set to 8
    OCR2B = ledPower;  // Set compare value for PWM
}

void LEDTimer_SetPower(uint8_t power) {
    ledPower = power;
    OCR2B = power;  // Update PWM compare value
}

void LEDTimer_SetPeriod(uint16_t period) {
    if (period < 200 || period > 5000) {
        return; // Validate period range
    }
    cli();  // Disable interrupts
    ledPeriod = period;
    OCR1A = ((16000000 / 8) * period / 1000) - 1;
    TCNT1 = 0;  // Reset timer counter to ensure immediate effect
    sei();  // Enable interrupts
}

ISR(TIMER1_COMPA_vect) {
    static uint8_t toggle = 0;
    toggle = !toggle;
    if (toggle) {
        OCR2B = ledPower;  // Set to specified power level
    } else {
        OCR2B = 0;  // Turn off the LED
    }
}

void parseCommand(char* command) {
    uint8_t power;
    uint16_t period;
    if (sscanf(command, "ledpowerfreq %hhu %u", &power, &period) == 2) {
        if (power > 255 || period < 200 || period > 5000) {
            USART_SendString("Error: Power must be 0-255 and period 200-5000ms.\n");
            return;
        }
        LEDTimer_SetPower(power);
        LEDTimer_SetPeriod(period);
        char response[100];
        snprintf(response, sizeof(response), "LED power set to %hhu and period set to %ums\n", power, period);
        USART_SendString(response);
    } else {
        USART_SendString("Invalid command format. Use: 'ledpowerfreq <power> <period>'.\n");
    }
}
