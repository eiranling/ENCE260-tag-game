/**
    @file player.c
	@authors Susan Collishaw and Eiran Ling
    @date 18 Oct 2017
	@brief The player module for the tag game. 
*/
#include <stdlib.h>
#include "led.h"
#include "tinygl.h"
#include "player.h"

// Define global variables that have been declared as 'extern' in player.h
uint8_t player = 0;
uint8_t other_player = 1;

/* Checks to see if the players have ended up at the same co-ords
 * this will indicate if the runner has been caught by the chaser
 * @param the list of players
 * @return true if players are at same co-ords
*/
bool player_caught (player_t* players) 
{
    if ((players[player].pos.x == players[other_player].pos.x) && (players[player].pos.y == players[other_player].pos.y)) {
        return true;
    } else {
        return false;
    }
}

/* Swaps over the status of runner so the runner becomes the chaser
 * and the chaser becomes the runner
 * @param the list of active players
*/
void swap (player_t* players) 
{
    
    if (players[player].is_runner) {
        players[player].is_runner = 0;
        players[other_player].is_runner = 1;
        led_set(LED1, 0);
            
    } else {
        players[player].is_runner = 1;
        players[other_player].is_runner = 0;
        led_set(LED1, 1);
    }
}

/* moves the player of this machiene in the direction stated 
 * @param the list of players
 * @param the direction to move in
 * @param player the index of the player to be moved
*/
void move_player (player_t* players, Direction* new, uint8_t* player)
{
    if (*new == SOUTH) {
        players[*player].pos.y++;
        if (players[*player].pos.y > TINYGL_HEIGHT -1) 
        {
            players[*player].pos.y = TINYGL_HEIGHT -1;
        }
    } else if (*new == EAST) {
        players[*player].pos.x++;
        if (players[*player].pos.x > TINYGL_WIDTH -1) 
        {
            players[*player].pos.x = TINYGL_WIDTH -1;
        }
    } else if (*new == NORTH) {
       players[*player].pos.y--;
        if (players[*player].pos.y < 0) 
        {
           players[*player].pos.y = 0;
        }
    } else if (*new == WEST) {
        players[*player].pos.x--;
        if (players[*player].pos.x < 0)
        {
            players[*player].pos.x = 0;
        }
    }
}

/* Creates both players and places them in random locations
 * on the matrix, ensuring they do not start in the same spot
 * turns on the blue led on the runners side
 * @params the list of players to be populated
 * @params the current player on this machiene
*/
void create_players (player_t* players, uint8_t PLAYER) 
{
    uint8_t i;
    bool runner = 1;

    for (i = 0; i < NUM_PLAYERS; i++)
    {
        uint8_t x;
        uint8_t y;
        
        x = TINYGL_WIDTH * i;
        y = TINYGL_HEIGHT * i;
		if (x == TINYGL_WIDTH || y == TINYGL_HEIGHT) {
			x--;
			y--;
		}
    
        players[i].pos.x = x;
        players[i].pos.y = y;
        players[i].is_runner = runner;
        players[i].speed = STANDARD_SPEED;
        
        runner = !runner; //make sure only one chaser
        
        tinygl_draw_point (players[i].pos, 1);
        if (players[PLAYER].is_runner == 1) {
            led_set(LED1, 1);
        } else {
            led_set(LED1, 0);
        }
    }
}

