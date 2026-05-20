#ifndef GAME_STRUCTURES_H
#define GAME_STRUCTURES_H

#define MAX_ROOMS 6
#define ENEMY_COUNT 2
#define MAX_NAME_LEN 32
#define SMALL_H 10
#define SMALL_W 15
#define LARGE_H 25
#define LARGE_W 35
#define MAX_GRID_H 30
#define MAX_GRID_W 40

struct Item {
int x;
int y;
bool collected;
char name[MAX_NAME_LEN];
};

struct Room {
int id;
int height;
int width;
int exitNorth;
int exitSouth;
int exitEast;
int exitWest;
Item item;
};

struct Player {
int x;
int y;
int lives;
int currentRoom;
bool hasItem;
char inventoryItem[MAX_NAME_LEN];
};

struct Enemy {
int x;
int y;
int type;
bool active;
};

#endif