/*
 * mytimer.c
 *
 *  Created on: Jun 18, 2016
 *      Author: Christoph Gadinger
 */

#include "sysTimer.h"

sysTimer *headTimer = NULL;

/**
 * Initialisiere Hardwaretimer fuer eine 1ms Laufzeit
 */
void sysTimer_init(){
	cli();								// disable interrupts
	TCCR0A |= (1<<WGM01);				// CTC-Mode; 8Bit timer
	OCR0A = 249;						// timer_TOP = OCRA
	TIMSK0 |= (1<<OCIE0A);				// trigger interrupt on compare
	TCCR0B |= (1<<CS01) | (1<<CS00);	// prescaler set to 64; master_clk=16MHz
	sei();								// enable interrupts
}

/**
 *  Haengt einen Timer in die verkettete Structliste ein
 * 	Ist noch kein Timer vorhanden wird einer angelegt.
 */
void sysTimer_add(sysTimer* adresse, long value, char *name, void (*callback)(void)){

	// pruefe ob bereits ein Timer initialisiert wurde
	if(headTimer == NULL){
		headTimer = adresse;
	}

	// erstelle Referenz zum HeadTimer (da die adresse zum "mos_headTimer" nicht veraendert werden darf)
	// info: Veraenderungen der Inhalte des "refTimer" veraendern natuerlich auch die Inhalte der "mos_headTimer" (weil Referenz)
	sysTimer *refTimer = headTimer;

	// springe bis zum letzten Timer in der veschachtelten Liste
	while(refTimer->nextTimer != NULL){
		refTimer = refTimer->nextTimer;
	}

	// setze adresse zum neuen Timer
	refTimer->nextTimer = adresse;

	// springe in neuen Timer
	refTimer = refTimer->nextTimer;

	// Setze Namen des Timers
	strncpy(refTimer->name, name, SYSTIMER_NAMESIZE);

	// Setze Timer-Laufzeit und Ist-Zeit des Timers
	refTimer->start_millis = value;
	refTimer->akt_millis = value;

	// Setze Adresse auf naechsten Timer auf NULL
	refTimer->nextTimer = NULL;

	// Setze Pointer auf Callback-Function
	refTimer->callback = callback;

	// Setze Status des neuen Timers
	refTimer->status = SYSTIMER_STATUS_STOP;
}

/**
 *  Dekrementiere Timer
 */
static void sysTimer_decrement(){
	// erstelle Referenz zum HeadTimer
	sysTimer *refTimer = headTimer;
	while(refTimer != NULL){
		if(refTimer->status == SYSTIMER_STATUS_RUNNING){
			refTimer->akt_millis--;
		}
		refTimer = refTimer->nextTimer;
	}
}

/**
 *  Ueberpfuefe ob Timer abgelaufen, wenn ja fuere callback-function aus
 */
void sysTimer_process(){
	// erstelle Referenz zum HeadTimer
	sysTimer *refTimer = headTimer;

	while(refTimer != NULL){									// Solange die Timerreferenz gueltig ist
		if(refTimer->akt_millis <= 0){							// Wenn Timer abgelaufen
#ifdef TIMER_CALCREF_OUT
			uint8_t toggle = 0;
			toggle = (PORTB & (1 << PB0)) ? 0 : 1;
			if(toggle) PORTB |= (1 << PB0);
#endif
			refTimer->akt_millis = refTimer->start_millis;		// Setze Counter auf Startwert zurueck
			refTimer->callback();								// Rufe Callback-Function auf
#ifdef TIMER_CALCREF_OUT
			if(toggle) PORTB &= ~(1 << PB0);
#endif
		}
		refTimer = refTimer->nextTimer;							// Setze Referenzpointer auf naechsten Timer
	}
}

/**
 *  Interrupt-Routine: Dekrementiere Timer
 */
ISR(TIMER0_COMPA_vect){
	sysTimer_decrement();
}
