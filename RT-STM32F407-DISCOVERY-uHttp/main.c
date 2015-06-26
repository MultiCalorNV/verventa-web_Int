/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "ITM_trace.h"
#include "chprintf.h"
#include "ArrayList.h"
#include "queue.h"

/*	LwIP include	*/
#include "lwipthread.h"
#include "../../../os/ext/lwip-1.4.1/src/include/lwip/stats.h"

/*	Web include		*/
#include "uhttpd.h"

#include "rtc.h"

/*	Static variables -------------------------------------------------*/
static int Debug_ITMDebug = 0;
ITMStream itm_port;

#define TASK_IP_HTTP_PRIORITY    88

//****************************************************************************

void Debug_ITMDebugEnable(void){
	volatile unsigned int *ITM_TER      = (volatile unsigned int *)0xE0000E00;
	volatile unsigned int *SCB_DHCSR 		= (volatile unsigned int *)0xE000EDF0;
	volatile unsigned int *DBGMCU_CR 		= (volatile unsigned int *)0xE0042004;

	*DBGMCU_CR |= 0x27; // DBGMCU_CR

if ((*SCB_DHCSR & 1) && (*ITM_TER & 1)) // Enabled?
    Debug_ITMDebug = 1;
}

//****************************************************************************

void Debug_ITMDebugOutputChar(char ch){
	static volatile unsigned int *ITM_STIM0 = (volatile unsigned int *)0xE0000000; // ITM Port 0
	static volatile unsigned int *SCB_DEMCR = (volatile unsigned int *)0xE000EDFC;

	if (Debug_ITMDebug && (*SCB_DEMCR & 0x01000000))
	{
		while(*ITM_STIM0 == 0);
  	*((volatile char *)ITM_STIM0) = ch;
	}
}

//****************************************************************************

void Debug_ITMDebugOutputString(char *Buffer){
	if (Debug_ITMDebug)
		while(*Buffer)
			Debug_ITMDebugOutputChar(*Buffer++);
}

//******************************************************************************

/*
 * Producer Thread
 */
//static THD_WORKING_AREA(wa_producer, 128);
/*static THD_FUNCTION(producer_thread, arg) {

  (void)arg;
  char e = 5;
  chRegSetThreadName("producer");
  while (TRUE) {
	En_queue(e++);
	chThdSleepMilliseconds(1000);
  }
}*/

/*
 * Consumer Thread
 */
//static THD_WORKING_AREA(wa_consumer, 128);
/*static THD_FUNCTION(consumer_thread, arg) {

  (void)arg;
  char e;
  chRegSetThreadName("consumer");
  while (TRUE) {
    e = De_queue();
	chprintf((BaseSequentialStream *)&itm_port, "Element: %d\n", e);
	chThdSleepMilliseconds(300);
  }
}*/

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palClearPad(GPIOG, GPIOG_PIN6);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOG, GPIOG_PIN6);
    chThdSleepMilliseconds(500);
    stats_display();
  }
}

/*
 * Application entry point.
 */
int main(void) {
  /* Enable TRACE debug -----------------------------------------------*/
  Debug_ITMDebugEnable();
  Debug_ITMDebugOutputString("SWV Enabled\n");
  
  itmObjectInit(&itm_port);

  chprintf((BaseSequentialStream *)&itm_port, "%s", "ChibiOS V3.0\n");

  /*End****************************************************************/
	
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  //RTCDateTime timespec;
  //uint32_t msec = 500;
  //struct tm t = { 0 };
  //t.tm_mon = 5;
  //t.tm_year = 2015 - 1900;
  //t.tm_isdst = -1;
  //t.tm_mday = 160;
  //t.tm_hour = 14;
  //t.tm_min = 21;
  //t.tm_sec = 40;

  //time_t now;
  //const struct tm *ltm;

  //time_t when = mktime(&t);
  //const struct tm *norm = localtime(&when);

  //chprintf((BaseSequentialStream *)&itm_port, "Today is : %s", asctime(norm));

  //rtcConvertStructTmToDateTime(norm, msec, &timespec);

  //chprintf((BaseSequentialStream *)&itm_port, "Today is : %d %d %d %d\n", timespec.year, timespec.month, timespec.day, timespec.dayofweek);

  //rtcSetTime(&RTCD1, &timespec);
  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD6, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  
  /*
   * Creates the Producer thread.
   */
  //chThdCreateStatic(wa_producer, sizeof(wa_producer), NORMALPRIO, producer_thread, NULL);
  
  /*
   * Creates the Consumer thread.
   */
  //chThdCreateStatic(wa_consumer, sizeof(wa_consumer), NORMALPRIO, consumer_thread, NULL);

  /*
   * Creates the LWIP threads (it changes priority internally).
   */
  chThdCreateStatic(wa_lwip_thread, sizeof(wa_lwip_thread), NORMALPRIO+1,
  					lwip_thread, NULL);

  /*
   * Create the HTTP server task
   */
  chThdCreateStatic(task_http_stack, sizeof(task_http_stack), TASK_IP_HTTP_PRIORITY,
    		 	 	 	HttpTask, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
	  if (palReadPad(GPIOA, GPIOA_BUTTON) == 1)
	  TestThread(&SD6);
	  //time(&now);
	  //ltm = localtime(&now);
	  //chprintf((BaseSequentialStream *)&itm_port, "Today is : %s", asctime(ltm));
	  chThdSleepMilliseconds(1000);
  }
}
