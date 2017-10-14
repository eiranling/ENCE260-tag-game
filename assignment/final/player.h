#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>

typedef enum {NORTH, EAST, WEST, SOUTH} Direction;

typedef struct player_struct
{
    tinygl_point_t pos;
    bool is_runner;
    uint16_t speed;
    Direction current_direction;
} player_t;

static uint8_t player = 0; // set player for this unit will be according to who is host unit
static uint8_t other_player = 1;

bool player_caught(player_t* players);

void swap (player_t* players);

void move_player (player_t* players, Direction* new, uint8_t* player);

void create_players (player_t* players, uint8_t PLAYER);

#endif
