#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#ifdef F_CPU
#define F_CPU 16000000UL
#endif
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

// Externally defined variable
extern volatile uint16_t adcValue;

// Function prototypes
void ADC_Init();
void USART_Init(unsigned int ubrr);
bool USART_Available();
char USART_Receive();
void USART_Transmit(unsigned char data);
void USART_SendString(const char* str);

#endif // TIMER_H
