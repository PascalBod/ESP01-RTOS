/*
 *  Copyright (C) 2015 Pascal Bodin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * Implements code used to create application-level protocol
 * messages.
 *
 * In current example, we have only one message: COUNTER
 *
 */

#include "c_types.h"

#include "protMessage.h"

/**
 * Builds COUNTER protocol message.
 *
 * ucCounter: value to be sent in COUNTER message.
 * pucBuffer: pointer to a pre-allocated buffer.
 * ucBufferL: size of pre-allocated buffer.
 *
 * returned value:
 *   -1 if buffer size is not enough
 *   n > 0 if buffer size is OK. COUNTER message is in buffer, its
 *         size is n.
 */
uint8 makeCounter(uint8 ucCounter, uint8 *pucBuffer, uint8 ucBufferL) {

	if (ucBufferL < PROT_MSG_COUNTER_L) {
		return -1;
	}

	pucBuffer[0] = PROT_MSG_COUNTER;
	pucBuffer[1] = ucCounter;
	return 2;

}
