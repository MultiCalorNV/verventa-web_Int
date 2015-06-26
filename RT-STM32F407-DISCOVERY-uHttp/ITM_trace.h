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
 * @file    ITM_trace.h
 * @brief   ITM Driver macros and structures.
 *
 * @addtogroup ITM
 * @{
 */

#ifndef _ITM_TRACE_H
#define _ITM_TRACE_H

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
	* @brief   @p itmStream specific data.
 */
#define _itm_stream_data                                                 \
  _base_sequential_stream_data

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   @p itmStream virtual methods table.
 */
struct ITMStreamVMT {
  _base_sequential_stream_methods
};

/**
 * @extends BaseAsynchronousChannel
 *
 * @brief   Full duplex itm driver class.
 * @details This class extends @p BaseAsynchronousChannel by adding physical
 *          I/O queues.
 */
typedef struct {
  /** @brief Virtual Methods Table.*/
  const struct ITMStreamVMT *vmt;
  _itm_stream_data
}ITMStream;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void	itmObjectInit(ITMStream *pitm);
#ifdef __cplusplus
}
#endif

#endif /* _ITM_TRACE_H */

/** @} */
