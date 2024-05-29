#include <mega128a.h>
#include <delay.h>
#include <stdio.h>

#define F_CPU 16000000UL  // 16 MHz
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#define DELAY 2000


void uart_init(void) {
    
    UBRR0H = 0;
    UBRR0L = 103;
    UCSR0A = 0x0; 
    UCSR0B = 0x18;
    UCSR0C = 0x06;  
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}

unsigned char uart_receive(void) {
    while (!(UCSR0A & (1<<RXC0)));
    return UDR0;
}

void init_pwm(void) {
    
     // PWM 설정
    //TCCR3A |= (1 << WGM30) | (1 << COM3B1);
    //TCCR3B |= (1 << WGM32) | (1 << CS31) | (1 << CS30); // 64분주
    //OCR3AH = 0; //High byte of OCR3A
    //OCR3AL = 255; // Low byte of OCR3A 
    
    // Mode 14, 8-bit high-speed PWM mode Timer counter number 3
    TCCR3A |= (1 << WGM30);
    TCCR3B |= (1 << WGM32);

    // Non-inverting mode TOP: 0xFF Comparison match value: OCR3B register
    TCCR3A |= (1 << COM3B1);
    TCCR3B |= (1 << CS31) | (1 << CS30); // Divide rate 64

    OCR3AH = 0; //High byte of OCR3A
    OCR3AL = 255; // Low byte of OCR3A
}



void stop_motors() {
    PORTB &= ~((1<<0)|(1<<1)|(1<<2)|(1<<3));
}

void control_motor(char command) {
    DDRF = 0x0F; // PORTF의 하위 4비트를 출력으로 설정합니다.
    DDRB |= (1<<5) | (1<<6); // PORTB의 5번, 6번 핀을 출력으로 설정합니다.
    
    PORTB &= ~((1<<5)|(1<<6)); // 먼저 모터를 정지시키기 위해 ENA와 ENB를 비활성화합니다.
     switch (command) {
        case 'F': // 전진
            PORTF = 0x09; // 1001, 전진  
            break;
        case 'B': // 후진
            PORTF = 0x06; // 0110, 후진
            break;
        case 'L': // 좌회전
            PORTF = 0x05; // 0101, 좌회전            
            break;
        case 'R': // 우회전
            PORTF = 0x0A; // 1010, 우회전            
            break;
        case 'S': // 정지
            PORTF = 0x00; // 모든 모터 정지
            break;
        default:
            printf("Unknown command\n");
            break;
     }
     
     if (command != 'S') {
        PORTB |= (1<<5) | (1<<6); // ENA와 ENB를 활성화하여 모터 동작
    }
}

void main(void) {
    uart_init();
    //init_pwm();
    DDRB = 0xFF; // 모터 제어 핀을 출력으로 설정

    while (1) {
        char command = uart_receive();  // UART로부터 명령 수신
        control_motor(command);        // 수신된 명령에 따라 모터 제어
    }
}
