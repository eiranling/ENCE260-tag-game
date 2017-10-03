#include "system.h"
#include "tinygl.h"
#include <stdlib.h>

#define NUM_PLAYERS 2

/* Define polling rates in Hz  */
#define BUTTON_TASK_RATE ???

#define DISPLAY_TASK_RATE ???


typedef struct player_struct
{
    tinygl_point_t pos;
    bool is_runner;
} player_t;


void create_players (player_t *players) 
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
        } while (i > 0 && players[0].pos.x != x && players[0].pos.y != y) //make sure both players start in different spaces.
    
        players[i].pos.x = x;
        players[i].pos.y = y;
        players[i].is_runner = runner;
        
        runner = !runner; //make both players have different roles
        
        tinygl_draw_point (players[i].pos, 1); // 1 for on
    }
}


int main (void)
{
    system_init ();

    tinygl_init (DISPLAY_TASK_RATE);
    
    player_t players[NUM_PLAYERS];
    
    create_players();

    while (1)
    {
        // set up a task scheduler to poll navswitch, 
        // place specials, IR polling,
        // update location of runner, update location of chaser.
        // (rate of runner/chaser update will depend on active specials)


    }
}
