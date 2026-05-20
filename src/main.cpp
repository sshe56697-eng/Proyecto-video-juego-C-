#include <iostream>
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include "game_structures.h"

static Room gRooms[MAX_ROOMS];
static Enemy gEnemies[ENEMY_COUNT];
static Player gPlayer;
static int gTicks = 0;
static bool gRunning = true;
static int gKeysDeposited = 0;
static char gGrid[MAX_GRID_H][MAX_GRID_W];

void PlatformClear() {
printf("\033[H\033[J");
}

#ifndef _WIN32
static struct termios gOldTermios;

void TermRaw() {
struct termios t;
tcgetattr(STDIN_FILENO, &gOldTermios);
t = gOldTermios;
t.c_lflag &= ~(ICANON | ECHO);
t.c_cc[VMIN] = 1;
t.c_cc[VTIME] = 0;
tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void TermRestore() {
tcsetattr(STDIN_FILENO, TCSANOW, &gOldTermios);
}

char GetchUnix() {
char c = 0;
read(STDIN_FILENO, &c, 1);
return c;
}
#endif

char ReadKey() {
#ifdef _WIN32
return (char)_getch();
#else
return GetchUnix();
#endif
}

const char* BorderColor(int ticks) {
if (ticks <= 20) return "\033[34m";
if (ticks <= 40) return "\033[33m";
return "\033[31m";
}

void ResetColor() {
printf("\033[0m");
}

void InitRooms(Room* r) {
r[0].id = 1; r[0].height = SMALL_H; r[0].width = SMALL_W;
r[0].exitNorth = -1; r[0].exitSouth = 1; r[0].exitEast = -1; r[0].exitWest = -1;
r[0].item.x = 5; r[0].item.y = 4; r[0].item.collected = false;
strncpy(r[0].item.name, "Ancient Key", MAX_NAME_LEN - 1);

r[1].id = 2; r[1].height = SMALL_H; r[1].width = SMALL_W;
r[1].exitNorth = 0; r[1].exitSouth = 2; r[1].exitEast = -1; r[1].exitWest = -1;
r[1].item.x = 3; r[1].item.y = 7; r[1].item.collected = false;
strncpy(r[1].item.name, "Glowing Orb", MAX_NAME_LEN - 1);

r[2].id = 3; r[2].height = SMALL_H; r[2].width = SMALL_W;
r[2].exitNorth = 1; r[2].exitSouth = -1; r[2].exitEast = 3; r[2].exitWest = -1;
r[2].item.x = 10; r[2].item.y = 5; r[2].item.collected = false;
strncpy(r[2].item.name, "Shadow Rune", MAX_NAME_LEN - 1);

r[3].id = 4; r[3].height = SMALL_H; r[3].width = SMALL_W;
r[3].exitNorth = -1; r[3].exitSouth = -1; r[3].exitEast = 4; r[3].exitWest = 2;
r[3].item.x = 7; r[3].item.y = 2; r[3].item.collected = false;
strncpy(r[3].item.name, "Iron Sigil", MAX_NAME_LEN - 1);

r[4].id = 5; r[4].height = SMALL_H; r[4].width = SMALL_W;
r[4].exitNorth = -1; r[4].exitSouth = -1; r[4].exitEast = 5; r[4].exitWest = 3;
r[4].item.x = 2; r[4].item.y = 8; r[4].item.collected = false;
strncpy(r[4].item.name, "Mystic Crest", MAX_NAME_LEN - 1);

r[5].id = 6; r[5].height = LARGE_H; r[5].width = LARGE_W;
r[5].exitNorth = -1; r[5].exitSouth = -1; r[5].exitEast = -1; r[5].exitWest = 4;
r[5].item.x = -1; r[5].item.y = -1; r[5].item.collected = true;
strncpy(r[5].item.name, "", MAX_NAME_LEN - 1);
}

void InitPlayer(Player* p) {
p->x = 2;
p->y = 2;
p->lives = 3;
p->currentRoom = 0;
p->hasItem = false;
strncpy(p->inventoryItem, "", MAX_NAME_LEN - 1);
}

void PlaceEnemiesInRoom(Room* r, Enemy* eArray) {
eArray[0].x = r->width - 3;
eArray[0].y = r->height - 3;
eArray[0].type = 1;
eArray[0].active = true;

eArray[1].x = 1;
eArray[1].y = r->height - 2;
eArray[1].type = 2;
eArray[1].active = true;
}

void RepositionEnemy(int idx, const Player* p, const Room* r, Enemy* eArray) {
int nx = (p->x + r->width / 2) % (r->width - 2) + 1;
int ny = (p->y + r->height / 2) % (r->height - 2) + 1;
if (nx == p->x && ny == p->y) nx = (nx + 2) % (r->width - 2) + 1;
eArray[idx].x = nx;
eArray[idx].y = ny;
}

void BuildGrid(const Room* r, const Player* p, const Enemy* eArray) {
int h = r->height;
int w = r->width;
int row = 0;
while (row < h) {
int col = 0;
while (col < w) {
if (row == 0 || row == h - 1) gGrid[row][col] = '#';
else if (col == 0 || col == w - 1) gGrid[row][col] = '#';
else gGrid[row][col] = '.';
col++;
}
row++;
}

if (r->exitNorth >= 0) gGrid[0][w / 2] = 'N';
if (r->exitSouth >= 0) gGrid[h - 1][w / 2] = 'S';
if (r->exitEast >= 0) gGrid[h / 2][w - 1] = 'E';
if (r->exitWest >= 0) gGrid[h / 2][0] = 'W';

if (r->id == 5) {
gGrid[h / 2][w - 2] = 'O';
}

const Item* it = &r->item;
if (!it->collected && it->x >= 0) {
gGrid[it->y][it->x] = '$';
}

int ei = 0;
while (ei < ENEMY_COUNT) {
const Enemy* e = &eArray[ei];
if (e->active) {
char sym = (e->type == 1) ? 'X' : 'Z';
gGrid[e->y][e->x] = sym;
}
ei++;
}

gGrid[p->y][p->x] = '@';
}

void Render(const Room* r, const Player* p, int ticks) {
PlatformClear();
int h = r->height;
int w = r->width;

printf("%s", BorderColor(ticks));
printf("=== Room %d | Lives: %d | Keys: %d/5 | Ticks: %d ===\n", r->id, p->lives, gKeysDeposited, ticks);
ResetColor();

int row = 0;
while (row < h) {
int col = 0;
while (col < w) {
char c = gGrid[row][col];
if (c == '#') {
printf("%s#", BorderColor(ticks));
ResetColor();
} else if (c == '@') {
printf("\033[32m@\033[0m");
} else if (c == 'X') {
printf("\033[35mX\033[0m");
} else if (c == 'Z') {
printf("\033[36mZ\033[0m");
} else if (c == '$') {
printf("\033[33m$\033[0m");
} else if (c == 'O') {
printf("\033[34mO\033[0m");
} else {
printf("%c", c);
}
col++;
}
printf("\n");
row++;
}

printf("%s", BorderColor(ticks));
printf("[W][A][S][D] Move | [G] Drop item at 'O'\n");
ResetColor();

printf("Inventory: ");
if (p->hasItem) {
printf("[%s]", p->inventoryItem);
} else {
printf("(empty)");
}
printf("\n");
}

void MoveEnemies(const Room* r, const Player* p, Enemy* eArray) {
int ei = 0;
while (ei < ENEMY_COUNT) {
Enemy *e = &eArray[ei];
if (!e->active) { ei++; continue; }

int dx = 0, dy = 0;
if (e->x < p->x) dx = 1;
else if (e->x > p->x) dx = -1;

if (e->y < p->y) dy = 1;
else if (e->y > p->y) dy = -1;

if (e->type == 1) {
int nx = e->x + dx;
if (nx > 0 && nx < r->width - 1) e->x = nx;
} else {
int ny = e->y + dy;
if (ny > 0 && ny < r->height - 1) e->y = ny;
}
ei++;
}
}

void CheckEnemyCollision(Player* p, const Room* r, Enemy* eArray) {
int ei = 0;
while (ei < ENEMY_COUNT) {
Enemy *e = &eArray[ei];
if (e->active && e->x == p->x && e->y == p->y) {
p->lives--;
printf("\a");
fflush(stdout);
RepositionEnemy(ei, p, r, eArray);
}
ei++;
}
}

void CheckItemPickup(Player* p, Room* r) {
Item *it = &r->item;
if (!it->collected && it->x == p->x && it->y == p->y) {
if (!p->hasItem) {
strncpy(p->inventoryItem, it->name, MAX_NAME_LEN - 1);
p->hasItem = true;
it->collected = true;
printf("\a");
fflush(stdout);
}
}
}

void CheckExit(char key, Player* p, Room* roomsArray, Enemy* eArray) {
Room *r = &roomsArray[p->currentRoom];
int newRoom = -1;

if (key == 'w' && p->y == 0 && r->exitNorth >= 0) {
newRoom = r->exitNorth;
} else if (key == 's' && p->y == r->height - 1 && r->exitSouth >= 0) {
newRoom = r->exitSouth;
} else if (key == 'd' && p->x == r->width - 1 && r->exitEast >= 0) {
if (r->exitEast == 5 && gKeysDeposited < 5) {
return;
}
newRoom = r->exitEast;
} else if (key == 'a' && p->x == 0 && r->exitWest >= 0) {
newRoom = r->exitWest;
}

if (newRoom >= 0) {
p->currentRoom = newRoom;
Room *nr = &roomsArray[newRoom];
p->x = nr->width / 2;
p->y = nr->height / 2;
PlaceEnemiesInRoom(nr, eArray);
}
}

void ProcessMove(char key, Player* p, Room* roomsArray, Enemy* eArray) {
Room *r = &roomsArray[p->currentRoom];
int nx = p->x;
int ny = p->y;

if (key == 'w') ny--;
else if (key == 's') ny++;
else if (key == 'a') nx--;
else if (key == 'd') nx++;
else if (key == 'g') {
if (r->id == 5 && p->hasItem && p->x == r->width - 2 && p->y == r->height / 2) {
p->hasItem = false;
strncpy(p->inventoryItem, "", MAX_NAME_LEN - 1);
gKeysDeposited++;
printf("\a");
fflush(stdout);
}
return;
}
else return;

char tile = gGrid[ny][nx];

if (tile == 'N' || tile == 'S' || tile == 'E' || tile == 'W') {
CheckExit(key, p, roomsArray, eArray);
return;
}

if (ny > 0 && ny < r->height - 1 && nx > 0 && nx < r->width - 1) {
p->x = nx;
p->y = ny;
}
}

void ShowVictory() {
PlatformClear();
printf("\033[33m\n=== VICTORY ===\n\n\033[0m");
}

void ShowDefeat() {
PlatformClear();
printf("\033[31m\n=== GAME OVER ===\n\n\033[0m");
}

int main() {
#ifndef _WIN32
TermRaw();
#endif

InitRooms(gRooms);
InitPlayer(&gPlayer);
PlaceEnemiesInRoom(&gRooms[0], gEnemies);

while (gRunning) {
Room* currentRoomPtr = &gRooms[gPlayer.currentRoom];

BuildGrid(currentRoomPtr, &gPlayer, gEnemies);
Render(currentRoomPtr, &gPlayer, gTicks);

if (gPlayer.currentRoom == 5 && gKeysDeposited == 5) {
ShowVictory();
gRunning = false;
break;
}

if (gPlayer.lives <= 0) {
ShowDefeat();
gRunning = false;
break;
}

char key = ReadKey();

if (key == 'q') {
gRunning = false;
break;
}

if (key != 'w' && key != 'a' && key != 's' && key != 'd' && key != 'g') continue;

ProcessMove(key, &gPlayer, gRooms, gEnemies);
gTicks++;

CheckItemPickup(&gPlayer, currentRoomPtr);
MoveEnemies(currentRoomPtr, &gPlayer, gEnemies);
CheckEnemyCollision(&gPlayer, currentRoomPtr, gEnemies);

if (gPlayer.lives <= 0) {
BuildGrid(currentRoomPtr, &gPlayer, gEnemies);
Render(currentRoomPtr, &gPlayer, gTicks);
ShowDefeat();
gRunning = false;
}
}

#ifndef _WIN32
TermRestore();
#endif

return 0;
}