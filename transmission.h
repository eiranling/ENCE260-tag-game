/**
    @file transmission.h
	@authors Susan Collishaw and Eiran Ling
    @date 18 Oct 2017
	@brief The transmisison module for the tag game. 

    @defgroup transmission The transmission module for the tag game

    This module handles most of the IR transmission and reception for the
	tag game.
*/
#ifndef TRANS_H
#define TRANS_H

#include "system.h"
#include "player.h"
#include "ir_uart.h"

/* Reads the IR buffer and assigns the value in the buffer to a char pointer.
 * 
 * @param the char pointer to write the buffered char to.
 */
void receive_IR (char* recv);

/* Transmits a char based on the Direction enum
 * @param A pointer to the direction to transmit.
 */
void transmit_IR_dir (Direction* dir);

/* 
Transmits and 'A' to signal that the game has ended.
*/
void transmit_start(void);
#endif

