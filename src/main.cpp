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

void BorrarPantalla() {
printf("\033[H\033[J"); }

#ifndef _WIN32
static struct termios gOldTermios;

void ModoRaw() {
struct termios t;
tcgetattr(STDIN_FILENO, &gOldTermios);
t = gOldTermios;
t.c_lflag &= ~(ICANON | ECHO);
t.c_cc[VMIN] = 1;
t.c_cc[VTIME] = 0;
tcsetattr(STDIN_FILENO, TCSANOW, &t); }

void RestaurarTerm() {
tcsetattr(STDIN_FILENO, TCSANOW, &gOldTermios); }

char LeerUnix() {
char c = 0;
read(STDIN_FILENO, &c, 1);
return c; }
#endif

char LeerTeclado() {
#ifdef _WIN32
return (char)_getch();
#else
return LeerUnix();
#endif
}

const char* ColorBorde(int ticks) {
if (ticks <= 20) return "\033[34m";
if (ticks <= 40) return "\033[33m";
return "\033[31m"; }

void ResetColor() {
printf("\033[0m"); }

void InicializarSalas(Room* r) {
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
strncpy(r[5].item.name, "", MAX_NAME_LEN - 1); }

void InicializarPlayer(Player* p) {
p->x = 2; p->y = 2;
p->lives = 3;
p->currentRoom = 0;
p->hasItem = false;
p->keysCarried = 0;
strncpy(p->inventoryItem, "", MAX_NAME_LEN - 1); }

void PonerEnemigos(Room* r, Enemy* eArray) {
eArray[0].x = r->width - 3; eArray[0].y = r->height - 3;
eArray[0].type = 1;
eArray[0].active = (r->id != 6);

eArray[1].x = 1; eArray[1].y = r->height - 2;
eArray[1].type = 2;
eArray[1].active = (r->id != 6); }

void MoverEnemigoAlAzar(int idx, const Player* p, const Room* r, Enemy* eArray) {
int nx = (p->x + r->width / 2) % (r->width - 2) + 1;
int ny = (p->y + r->height / 2) % (r->height - 2) + 1;
if (nx == p->x && ny == p->y) { nx = (nx + 2) % (r->width - 2) + 1; }
eArray[idx].x = nx;
eArray[idx].y = ny; }

void CrearMapa(const Room* r, const Player* p, const Enemy* eArray) {
int h = r->height; int w = r->width;
for (int row = 0; row < h; row++) {
for (int col = 0; col < w; col++) {
if (row == 0 || row == h - 1) { gGrid[row][col] = '#'; }
else if (col == 0 || col == w - 1) { gGrid[row][col] = '#'; }
else { gGrid[row][col] = '.'; } } }

if (r->exitNorth >= 0) gGrid[0][w / 2] = 'N';
if (r->exitSouth >= 0) gGrid[h - 1][w / 2] = 'S';
if (r->exitEast >= 0) gGrid[h / 2][w - 1] = 'E';
if (r->exitWest >= 0) gGrid[h / 2][0] = 'W';

if (r->id == 5) { gGrid[h / 2][w - 2] = 'O'; }

const Item* it = &r->item;
if (!it->collected && it->x >= 0) { gGrid[it->y][it->x] = '$'; }

for (int ei = 0; ei < ENEMY_COUNT; ei++) {
const Enemy* e = &eArray[ei];
if (e->active) {
char sym = (e->type == 1) ? 'X' : 'Z';
gGrid[e->y][e->x] = sym; } }

gGrid[p->y][p->x] = '@'; }

void Dibujar(const Room* r, const Player* p, int ticks) {
BorrarPantalla();
int h = r->height; int w = r->width;

printf("%s", ColorBorde(ticks));
printf("=== Room %d | Lives: %d | Keys Deposited: %d/5 | Ticks: %d ===\n", r->id, p->lives, gKeysDeposited, ticks);
ResetColor();

for (int row = 0; row < h; row++) {
for (int col = 0; col < w; col++) {
char c = gGrid[row][col];
if (c == '#') {
printf("%s#", ColorBorde(ticks)); ResetColor(); } 
else if (c == '@') { printf("\033[32m@\033[0m"); } 
else if (c == 'X') { printf("\033[35mX\033[0m"); } 
else if (c == 'Z') { printf("\033[36mZ\033[0m"); } 
else if (c == '$') { printf("\033[33m$\033[0m"); } 
else if (c == 'O') { printf("\033[34mO\033[0m"); } 
else { printf("%c", c); } }
printf("\n"); }

printf("%s", ColorBorde(ticks));
printf("[W][A][S][D] Move | [G] Deposit ALL keys at 'O'\n");
ResetColor();

printf("Inventory: Items Collected (%d/5) -> ", gKeysDeposited + p->keysCarried);
if (p->hasItem) { printf("[%s] (and %d keys carried)", p->inventoryItem, p->keysCarried); } 
else if (p->keysCarried > 0) { printf("(%d keys carried)", p->keysCarried); } 
else { printf("(empty)"); }
printf("\n"); }

void MoverEnemigos(const Room* r, const Player* p, Enemy* eArray) {
if (r->id == 6) return;
for (int ei = 0; ei < ENEMY_COUNT; ei++) {
Enemy *e = &eArray[ei];
if (!e->active) { continue; }

int dx = 0, dy = 0;
if (e->x < p->x) dx = 1;
else if (e->x > p->x) dx = -1;

if (e->y < p->y) dy = 1;
else if (e->y > p->y) dy = -1;

if (e->type == 1) {
if (gTicks % 2 == 0) {
int nx = e->x + dx;
if (nx > 0 && nx < r->width - 1 && gGrid[e->y][nx] != '#') e->x = nx; } } 
else {
int ny = e->y + dy;
if (ny > 0 && ny < r->height - 1 && gGrid[ny][e->x] != '#') e->y = ny; } } }

void RevisarGolpe(Player* p, const Room* r, Enemy* eArray) {
for (int ei = 0; ei < ENEMY_COUNT; ei++) {
Enemy *e = &eArray[ei];
if (e->active && e->x == p->x && e->y == p->y) {
p->lives--;
printf("\a"); fflush(stdout);
MoverEnemigoAlAzar(ei, p, r, eArray); } } }

void AgarrarItem(Player* p, Room* r) {
Item *it = &r->item;
if (!it->collected && it->x == p->x && it->y == p->y) {
strncpy(p->inventoryItem, it->name, MAX_NAME_LEN - 1);
p->hasItem = true; p->keysCarried++;
it->collected = true;
printf("\a"); fflush(stdout); } }

void CambiarSala(char key, Player* p, Room* roomsArray, Enemy* eArray) {
Room *r = &roomsArray[p->currentRoom];
int newRoom = -1;

if (key == 'w' && p->y == 0 && r->exitNorth >= 0) { newRoom = r->exitNorth; } 
else if (key == 's' && p->y == r->height - 1 && r->exitSouth >= 0) { newRoom = r->exitSouth; } 
else if (key == 'd' && p->x == r->width - 1 && r->exitEast >= 0) {
if (r->exitEast == 5 && gKeysDeposited < 5) { return; }
newRoom = r->exitEast; } 
else if (key == 'a' && p->x == 0 && r->exitWest >= 0) { newRoom = r->exitWest; }

if (newRoom >= 0) {
p->currentRoom = newRoom;
Room *nr = &roomsArray[newRoom];
if (key == 'w') { p->x = nr->width / 2; p->y = nr->height - 2; }
else if (key == 's') { p->x = nr->width / 2; p->y = 1; }
else if (key == 'd') { p->x = 1; p->y = nr->height / 2; }
else if (key == 'a') { p->x = nr->width - 2; p->y = nr->height / 2; }
PonerEnemigos(nr, eArray); } }

void MoverJugador(char key, Player* p, Room* roomsArray, Enemy* eArray) {
Room *r = &roomsArray[p->currentRoom];
int nx = p->x; int ny = p->y;

if (key == 'w') ny--;
else if (key == 's') ny++;
else if (key == 'a') nx--;
else if (key == 'd') nx++;
else if (key == 'g') {
if (r->id == 5 && (p->x == r->width - 2 && p->y == r->height / 2)) {
gKeysDeposited += p->keysCarried;
p->keysCarried = 0; p->hasItem = false;
strncpy(p->inventoryItem, "", MAX_NAME_LEN - 1);
printf("\a"); fflush(stdout); }
return; }
else return;

char tile = gGrid[ny][nx];

if (tile == 'N' || tile == 'S' || tile == 'E' || tile == 'W') {
p->x = nx; p->y = ny;
CambiarSala(key, p, roomsArray, eArray);
return; }

if (ny > 0 && ny < r->height - 1 && nx > 0 && nx < r->width - 1) {
if (gGrid[ny][nx] != '#') {
p->x = nx; p->y = ny; } } }

void PantallaVictoria() {
BorrarPantalla();
printf("\033[33m\n VICTORY: YOU WON \n\n\033[0m"); }

void PantallaDerrota() {
BorrarPantalla();
printf("\033[31m\n GAME OVER \n\n\033[0m"); }

int main() {
#ifndef _WIN32
ModoRaw();
#endif

InicializarSalas(gRooms);
InicializarPlayer(&gPlayer);
PonerEnemigos(&gRooms[0], gEnemies);

for (; gRunning; ) {
Room* currentRoomPtr = &gRooms[gPlayer.currentRoom];

CrearMapa(currentRoomPtr, &gPlayer, gEnemies);
Dibujar(currentRoomPtr, &gPlayer, gTicks);

if (gPlayer.currentRoom == 5 && gKeysDeposited == 5) {
PantallaVictoria(); gRunning = false;
break; }

if (gPlayer.lives <= 0) {
PantallaDerrota(); gRunning = false;
break; }

char key = LeerTeclado();

if (key == 'q') {
gRunning = false;
break; }

if (key != 'w' && key != 'a' && key != 's' && key != 'd' && key != 'g') continue;

MoverJugador(key, &gPlayer, gRooms, gEnemies);
gTicks++;

AgarrarItem(&gPlayer, currentRoomPtr);
MoverEnemigos(currentRoomPtr, &gPlayer, gEnemies);
RevisarGolpe(&gPlayer, currentRoomPtr, gEnemies);

if (gPlayer.lives <= 0) {
CrearMapa(currentRoomPtr, &gPlayer, gEnemies);
Dibujar(currentRoomPtr, &gPlayer, gTicks);
PantallaDerrota(); gRunning = false; } }

#ifndef _WIN32
RestaurarTerm();
#endif

return 0; 
}
