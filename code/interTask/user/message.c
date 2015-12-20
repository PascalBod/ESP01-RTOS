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
 * Message identifiers.
 */
typedef enum {
	MSG_COUNTED = 0,
} MESSAGE_ID;

/**
 * Message sent by task 1.
 */
typedef struct {
	uint8 ucValue;
} Msg_Counted_t;

/**
 * Data part of a message.
 */
typedef union {
	Msg_Counted_t xCounted;
} Msg_data_u;

/**
 * Message structure.
 */
struct ITMessage {
	MESSAGE_ID ucId;
	Msg_data_u xData;
};
