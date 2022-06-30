/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher for MPS2
 * Note(s): possible defines set in "options for target - C/C++ - Define"
 *            __USE_LCD    - enable Output on GLCD
 *            __USE_TIMER0 - use Timer0  to generate timer interrupt
 *                         - use SysTick to generate timer interrupt (default)
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2015 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

/* Standard includes. */
#include <stdio.h>
#include <string.h>

typedef unsigned int            uint32_t; 
#include "task2.h"

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/




void vTaskStart( void *pvParameters )
{
/* write your MPU re-configuration code here



*/

  printf( "Attack test begin\n" );
	
  AttackTest();
  for(;;);
}



int main (void) {

	uint32_t a = 1234; // replace with your last 4-digital id 
    prvSetupHardware();
	xTaskCreate( vTaskStart, "Test2", 100, NULL, ( 1 | ( 0x80000000UL ) ), NULL );
    StartFreeRTOS(a);

	/* Will only get here if there was insufficient memory to create the idle
	task. */
	for(;;);

}




