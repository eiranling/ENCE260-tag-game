#include <stdlib.h>
#include "system.h"
#include "tinygl.h"
#include "navswitch.h"
#include "ir_uart.h"


enum direction {NORTH, EAST, WEST, SOUTH};

enum special {SPEED_UP, SLOW_DOWN};

typedef struct player_struct
{
    tinygl_point_t pos;
    bool is_runner;
} player_t;

/* Creates both players and places them in random locations
 * on the matrix, ensuring they do not start in the same spot
 * @params the list of players to be populated
 */ 
void create_players (player_t* players);


/* Polls the navswitch and sets the direction for the 
 * player to move in 
 * @param the list of players
 * @param the current direction to be updated
 */ 
void get_move (player_t* players, direction* current);


/* moves the player of this machiene in the direction stated
 * @param the direction to move in
 * @param the list of players
 * @returns the new point co-ords of the player for this machiene
 */ 
tinygl_point_t move_player (direction* new, player_t* players);


/* Checks to see if the players have ended up at the same co-ords
 * this will indicate if the runner has been caught by the chaser
 * @param the list of players
 * @return true if players are at same co-ords
 */ 
bool player_caught (player_t* players);

/* Swaps over the status of runner so the runner becomes the chaser
 * and the chaser becomes the runner
 * @param the list of active players
 */ 
void swap (player_t* players);

/* 
 * 
 */
 void create_special (void); 

/*
 * 
 */
 void apply_special (player_t* players, special power); 
