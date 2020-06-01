;
; AssemblerApplication1.asm
;
; Created: 21.04.2020 13:55:56
; Author : Mariia Mostova
;

.include "m2560def.inc"

.DSEG

.CSEG
	.org	0
	jmp		reset
	.org	0x0040
	jmp		TIM3_COMPA

reset:	
	ldi		r16, high(RAMEND)  
	out		SPH, r16 
	ldi		r16, low(RAMEND)   
	out		SPL, r16  

	ldi		r16, 0x00
	ldi		r17, 0xFF
	out		PORTA, r17
	out		DDRA, r16
	out		PORTF, r16
	out		DDRF, r17
	out		PORTC, r16
	out		DDRC, r17
	out		PORTE, r16
	out		DDRE, r17

	cli 

	ldi		r20, 0x00
	sts		TCCR3A, r20
	ldi		r20, (1 << WGM31) | (1 << CS31) | (1 << CS30)
	sts		TCCR3B, r20
	ldi		r27, high(0x200A) 
	sts		OCR3AH, r27
	ldi		r27, low(0x200A) 
	sts		OCR3AL, r27
	ldi		r20, 1 << OCIE3A
	sts		TIMSK3, r20

	sei

	rjmp main

TIM3_COMPA:
	ldi		r22, 0x00
	sbic	PINB, 0
	sbic	PINB, 1
	rjmp	a0TimerFirst

	reti

a0TimerFirst:
	ldi		r28, 0b0000_0101
	lsr		r21
	lsl		r23
	lsl		r23
	add		r22, r21
	add		r22, r23
	reti

main:
;first
	ldi		r22, 0x00
	ldi		r21, 0b1000_0000
;second
	ldi		r23, 0b0000_0001
	ldi		r25, 0b0000_0010	
;first
	sbis    PINA, 0 
    rjmp	a0_beep
;second

	rjmp	main

a0_beep:
	sbis    PINA, 2 
	rjmp	a2_pressed

a2_pressed:
	out		PORTC, r22
	sbic    PINC, 4
	rcall	setPinb6 
	rjmp	a2_pressed

setPinb6:
	sbic    PINC, 6
	ldi		r23, 0b0000_0010
	ret

