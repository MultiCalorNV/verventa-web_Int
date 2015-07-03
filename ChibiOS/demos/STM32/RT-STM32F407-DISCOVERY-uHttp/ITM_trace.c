/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio.

    This file is part of ChibiOS.

    ChibiOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    ITM_trace.c
 * @brief   ITM Driver code.
 *
 * @addtogroup ITM
 * @{
 */

#include "hal.h"
#include "ITM_trace.h"

/*===========================================================================*/
/* External functions		                                                 */
/*===========================================================================*/
extern void Debug_ITMDebugOutputChar(char ch);

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/
static msg_t put(void *ip, uint8_t b) {

  (void)ip;
  
  Debug_ITMDebugOutputChar(b);

  return 1;
}

static msg_t get(void *ip) {

  (void)ip;

  return 4;
}

static size_t writes(void *ip, const uint8_t *bp, size_t n) {
  size_t cnt = n;

  while (cnt > 0) {
    put(ip, *bp);
    bp++;
    cnt--;
  }

  return n;
}

static size_t reads(void *ip, uint8_t *bp, size_t n) {

  (void)ip;
  (void)bp;
  (void)n;

  return 0;
}

static const struct ITMStreamVMT vmt = {writes, reads, put, get};

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   ITM stream object initialization.
 *
 * @param[out] pitm      pointer to the @p ITMStream object to be initialized
 */
void itmObjectInit(ITMStream *pitm) {
   pitm->vmt = &vmt;
}
/** @} */
