/*
 * FreeRTOS V202012.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


/*
 * This file demonstrates the use of FreeRTOS-MPU.  It creates tasks in both
 * User mode and Privileged mode, and using both the xTaskCreate() and
 * xTaskCreateRestricted() API functions.  The purpose of each created task is
 * documented in the comments above the task function prototype (in this file),
 * with the task behaviour demonstrated and documented within the task function
 * itself.
 *
 * In addition a queue is used to demonstrate passing data between
 * protected/restricted tasks as well as passing data between an interrupt and
 * a protected/restricted task.  A software timer is also used.
 */

/* Standard includes. */
#include "string.h"
#include <stdio.h>
#include "task3a.h"

void vTask3( void * pvParameters ) {	
	( void ) pvParameters; /* Unused parameters. */
	
	/* write your malicious code here
	
 
   
	
	*/


	printf("Attack successful!\n");
	
	
	for( ; ; )
	{
	}
}

int main( void )
{
  uint32_t id;
  id = 1234; //input your last 4-digital id

	prvSetupHardware();
	StartFreeRTOS(id, vTask3);

	/* Will only get here if there was insufficient memory to create the idle
	task. */
	for( ;; );
}
/*-----------------------------------------------------------*/








