/*
 * sysTimer.h
 *
 *  Created on: May 11, 2017
 *      Author: Christoph Gadinger
 */

#ifndef LIB_SYSTIMER_H_
#define LIB_SYSTIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/**
 *  Prozess Status Definitionen
 */
#define SYSTIMER_STATUS_STOP 		1
#define SYSTIMER_STATUS_RUNNING 	2
#define SYSTIMER_STATUS_HOLD 		3

#define SYSTIMER_NAMESIZE			20

/**
 *  Declaration des Timer Structs
 */
typedef struct sysTimer{
	char name[SYSTIMER_NAMESIZE];
	char status;
	long start_millis;
	long akt_millis;
	void (*volatile callback)(void);
	struct sysTimer* nextTimer;
} sysTimer;

/**
 * 	Das Kopfelement der Timerliste (nur dekleration)
 */
extern sysTimer *headTimer;

// Funktionsprototypen
void sysTimer_init();
void sysTimer_add(sysTimer* address, long value, char * name, void (*callback)(void));
void sysTimer_decrement();
void sysTimer_process();

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* LIB_SYSTIMER_H_ */
