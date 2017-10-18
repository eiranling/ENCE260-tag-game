/**
    @file player.h
	@authors Susan Collishaw and Eiran Ling
    @date 18 Oct 2017
	@brief The player module for the tag game. 

    @defgroup player The player module for the tag game

    This module handles most of the player movement and interaction
    around the game board. It also has two global variables that are
    externally accessed from both game.c and player.c
*/
#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>

#define NUM_PLAYERS 2
#define STANDARD_SPEED 200

// Direction enum
typedef enum {NORTH, EAST, WEST, SOUTH} Direction;

// define two externally accessed global variables
extern uint8_t player;
extern uint8_t other_player;

// Okayer structure
typedef struct player_struct
{
    // Player position
    tinygl_point_t pos;

    // Whether the player is 'it'
    bool is_runner;

    // Speed of the player
    uint16_t speed;

    // Direction that the player is moving in.
    Direction current_direction;
} player_t;

/* Checks to see if the players have ended up at the same co-ords
 * this will indicate if the runner has been caught by the chaser
 * @param the list of players
 * @return true if players are at same co-ords
*/
bool player_caught(player_t* players);

/* Swaps over the status of runner so the runner becomes the chaser
 * and the chaser becomes the runner
 * @param the list of active players
*/
void swap (player_t* players);

/* moves the player of this machiene in the direction stated 
 * @param the list of players
 * @param the direction to move in
 * @param player the index of the player to be moved
*/
void move_player (player_t* players, Direction* new, uint8_t* player);

/* Creates both players and places them in random locations
 * on the matrix, ensuring they do not start in the same spot
 * turns on the blue led on the runners side
 * @params the list of players to be populated
 * @params the current player on this machiene
*/
void create_players (player_t* players, uint8_t PLAYER);

#endif
