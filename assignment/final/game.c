#include <stdlib.h>
#include "system.h"
#include "tinygl.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "game.h"

#define NUM_PLAYERS 2 //max players
#define TIME_LIMIT 60000 // 1 min in miliseconds
#define PLAYER 0 // set player for this unit
#define SPEED  // set standard speed
#define UP_SPEED  // set power up speed
#define DOWN_SPEED  //set power down speed

/* Define polling rates in Hz  */
#define NAVSWITCH_TASK_RATE ???

#define DISPLAY_TASK_RATE ???



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
void create_players (player_t* players) 
{
    uint8_t i;
    uint8_t runner = 1;

    for (i = 0; i < NUM_PLAYERS; i++)
    {
        uint8_t x;
        uint8_t y;
        
        do { //randomly draw co-ords within our matrix
            x = rand () % TINYGL_WIDTH;
            y = rand () % TINYGL_HEIGHT;
        } while (i > 0 && !player_caught(&players)) //make sure both players start in different spaces.
    
        players[i].pos.x = x;
        players[i].pos.y = y;
        players[i].is_runner = runner;
        
        runner = !runner; //make both players have different roles
        
        tinygl_draw_point (players[i].pos, 1); // 1 for on
    }
}



/* Polls the navswitch and sets the direction for the player
 * to move in
 * @param the list of players
 * @param the current direction to be updated
 */ 
void get_move (player_t* players, direction* current)
{
    navswitch_update();
    
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) 
    { 
        current = NORTH;
    } else if (navswitch_push_event_p (NAVSWITCH_SOUTH))  
    {
        current = SOUTH;
    } else if (navswitch_push_event_p (NAVSWITCH_EAST)) 
    { 
        current = EAST;
    } else if (navswitch_push_event_p (NAVSWITCH_WEST))
    {
        current = WEST;
    }

}

/* moves the player of this machiene in the direction stated 
 * @param the direction to move in
 * @param the list of players
 * @returns the new point co-ords of the player for this machiene
 */ 
tinygl_point_t move_player (direction* new, player_t* players)
{
    if (new == NORTH) {
        player[PLAYER].pos.y++;
        if (player[PLAYER].pos.y == TINYGL_HEIGHT) 
        {
            player[PLAYER].pos.y = 0;
        }
    } else if (new == EAST) {
        player[PLAYER].pos.x++;
        if (player[PLAYER].pos.x == TINYGL_WIDTH) 
        {
            player[PLAYER].pos.x = 0;
        }
    } else if (new == SOUTH) {
        player[PLAYER].pos.y--;
        if (player[PLAYER].pos.y == TINYGL_HEIGHT) 
        {
            player[PLAYER].pos.y = 0;
        }
    } else if new == WEST) {
        player[PLAYER].pos.x--;
        if (player[PLAYER].pos.x == TINYGL_WIDTH) 
        {
            player[PLAYER].pos.x = 0;
        }
    }
    return player[PLAYER].pos;
}


/* Checks to see if the players have ended up at the same co-ords
 * this will indicate if the runner has been caught by the chaser
 * @param the list of players
 * @return true if players are at same co-ords
 */ 
bool player_caught (player_t* players) 
{
    uint8_t caught = 0;
    if (player[0].pos == player[1].pos)
    { 
        caught = 1;
    }
    return caught;
}

/* Swaps over the status of runner so the runner becomes the chaser
 * and the chaser becomes the runner
 * @param the list of active players
 */ 
void swap (player_t* players) 
{
    if (player[0].is_runner) {
        player[0].is_runner = 0;
        player[1].is_runner = 1;
    } else {
        player[0].is_runner = 1;
        player[1].is_runner = 0;
    }
}


/* 
 * 
 */
 void create_special (void)
 {
     continue;
 } 

/*
 * 
 */
 void apply_special (player_t* players, special power)
 {
     continue;
 }


int main (void)
{
    system_init ();

    tinygl_init (DISPLAY_TASK_RATE);
    
    navswitch_init();
    
    
    player_t players[NUM_PLAYERS];
    
    create_players(&players);
    
    direction current_direction;

    while (1)
    {
        // set up a task scheduler to poll navswitch, 
        // place specials, IR polling,
        // update location of runner, update location of chaser.
        // (rate of runner/chaser update will depend on active specials)
        
            get_move(&players, &current_direction);
        
            tinygl_draw_point (player[PLAYER].pos, 0); // temp turn off point to stop ghosting
            move_player(current_direction);
            tinygl_draw_point (player[PLAYER].pos, 1);
            if (player_caught(&players)) {
                swap();
            }
    }
}
