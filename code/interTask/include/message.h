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

#ifndef INCLUDE_MESSAGE_H_
#define INCLUDE_MESSAGE_H_

/**
 * A message is made of an identifier (or message type), and of some data,
 * which structure depends on message identifier. Data part is declared as
 * a union, which contains different data structures.
 */

/**
 * Message identifiers.
 */
typedef enum {
	MSG_TOBECOUNTED = 0,
	MSG_WAITMSG,
} MESSAGE_ID;

/**
 * Message sent by task 1.
 */
typedef struct {
	uint8 ucValue;
} Msg_ToBeCounted_t;

/**
 * Data parts of messages.
 */
typedef union {
	Msg_ToBeCounted_t xToBeCounted;
} Msg_data_u;

/**
 * Message structure.
 */
typedef struct {
	MESSAGE_ID ucId;
	Msg_data_u xData;
} ITMessage_t;

#endif /* INCLUDE_MESSAGE_H_ */
