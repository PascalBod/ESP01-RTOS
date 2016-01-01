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
 * Implements framing for protocol messages.
 * How to use it:
 * - call frame1() on protocol message length
 * - send returned value
 * - call frame2() on protocol message length
 * - send returned value
 * - send protocol message
 */

#include "c_types.h"

/**
 * Returns first frame byte to be sent, for a given message length.
 */
uint8 frame1(uint16 usMessageLength) {
	return (uint8)(usMessageLength & 0x00FF);
}

uint8 frame2(uint16 usMessageLength) {
	return (uint8)((usMessageLength >> 8) & 0x00FF);
}
