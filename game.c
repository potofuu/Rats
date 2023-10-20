#include "gba.h"
#include "game.h"
#include "mode4.h"
#include "rat.h"
#include "cheese.h"
#include "print.h"
#include "dance2.h"
#include "dance1.h"
#include "downrat.h"
#include "trap.h"
#include "sounds.h"
#include <stdlib.h>
#include <stdio.h>

PLAYER player;
CHEESE cheese[CHEESECOUNT];
TRAP traps[TRAPCOUNT];
WALL walls[WALLCOUNT];

int lives;
int score;
int total;

//initialize the game objects, player, cheese, traps, and walls

void initGame() {
    lives = 1;
    score = 0;
    initPlayer();
    initCheese();
    initTraps();
    initWalls();

    //load in shared palette
    DMANow(3, dance2Pal, BG_PALETTE, 256);
}

//start the player at bottom right
void initPlayer() {
    player.y = 120;
    player.x = 200;
    player.oldY = player.y;
    player.oldX = player.x;
    player.dx = 0;
    player.dy = 0;
    player.height = 40;
    player.width = 40;
}

//initialize cheese, alternating in each row
void initCheese() {
    for (int i = 0; i < CHEESECOUNT; i++) {
        cheese[i].y = 20 + (45 * i);
        cheese[i].height = 20;
        cheese[i].width = 20;
        cheese[i].active = 1;
        cheese[i].color = YELLOWID;
        if (i % 2 == 0) {
            cheese[i].x = 20;
        } else {
            cheese[i].x = 200;
        }
    }
}

//initialize the traps
void initTraps() {
    for (int i = 0; i < TRAPCOUNT; i++) {
        traps[i].height = 10;
        traps[i].width = 15;
        traps[i].dy = 2;
    }
    traps[0].x = 80;
    traps[1].x = 130;
    traps[2].x = 75;
    traps[3].x = 150;
    traps[4].x = 150;
    traps[0].y = 25;
    traps[1].y = 25;
    traps[2].y = 120;
    traps[3].y = 120;
    traps[4].y = 70;
}

//initialize the walls, should start with all 3 active
void initWalls() {
    for (int i = 0; i < WALLCOUNT; i++) {
        walls[i].active = 1;
        walls[i].color = GRAID;
        walls[i].height = 10;
        walls[i].width = 240;
        walls[i].x = 0;
        walls[i].y = 10 + i * 45;
    }
}

//run game update functions
void updateGame() {
    updatePlayer();
    updateWalls();
    for (int i = 0; i < CHEESECOUNT; i++) {
        updateCheese(&cheese[i]);
    }
    for (int i = 0; i < TRAPCOUNT; i++) {
        updateTraps(&traps[i]);
    }
}

//check for wall collision as well as move inputs
void updatePlayer() {
    if (wallCollision(&walls[WALLCOUNT - score - 1])) {
        lives--;
    }
    if (BUTTON_HELD(BUTTON_LEFT) && (player.x - 1 > 0)) {
        player.dx = -2;
        player.dy = 0;
    } else if (BUTTON_HELD(BUTTON_RIGHT) && (player.x + 50 < 240)) {
        player.dx = 2;
        player.dy = 0;
    } else if (BUTTON_HELD(BUTTON_DOWN) && (player.y + 40 < 160)) {
        player.dy = 2;
        player.dx = 0;
    } else if (BUTTON_HELD(BUTTON_UP) && (player.y - 1 > 0)) {
        player.dy = -2;
        player.dx = 0;
    } else {
        player.dx = 0;
        player.dy = 0;
    }
    player.oldX = player.x;
    player.x += player.dx;
    player.oldY = player.y;
    player.y += player.dy;
}

//check if cheese gets eaten
void updateCheese(CHEESE* cheese) {
    if (cheese->active && collision(cheese->x, cheese->y, cheese->width, cheese->height, player.x, player.y, player.width, player.height)) {
        cheese->active = 0;
        score++;
        total++;
        REG_SND2CNT = DMG_ENV_VOL(4) | DMG_STEP_TIME(2);
        REG_SND2FREQ = NOTE_G6 | SND_RESET;
    }
}

//move the traps
void updateTraps(TRAP* trap) {
    if (collision(trap->x, trap->y, trap->width, trap->height - 10, player.x, player.y, player.width - 5, player.height - 5)) {
        lives--;
    }
    if (traps[0].y > 65) {
        trap->dy = -1;
    } else if (traps[0].y < 0) {
        trap->dy = 1;
    }
    trap->y += trap->dy;
}

//check if walls need to be disabled, based on how much cheese is eaten
void updateWalls() {
    if (score == 1) {
        walls[2].color = WHITEID;
        walls[2].active = 0;
    } else if (score == 2) {
        walls[1].color = WHITEID;
        walls[1].active = 0;
    }
}

//draw game function calls, iteratively move through cheese, wall and trap arrays
void drawGame() {
    fillScreen4(WHITEID);
    drawPlayer();
    for (int i = 0; i < CHEESECOUNT; i++) {
        drawCheese(&cheese[i]);
    }
    for (int i = 0; i < WALLCOUNT; i++) {
        drawWalls(&walls[i]);
    }
    for (int i = 0; i < TRAPCOUNT; i++) {
        drawTrap(&traps[i]);
    }
}

//draw out player, switch to flipped upon cheese eat
void drawPlayer() {
    if (score % 2 == 0) {
        drawImage4(player.x, player.y, player.width, player.height, ratBitmap);
    } else {
        drawImage4(player.x, player.y, player.width, player.height, downratBitmap);
    }

}

//draw the cheese
void drawCheese(CHEESE* cheese) {
    if (cheese->active) {
        drawImage4(cheese->x, cheese->y, cheese->width, cheese->height, cheeseBitmap);
    }
}

//draw the traps
void drawTrap(TRAP* trap) {
    drawImage4(trap->x, trap->y, trap->width, trap->height, trapBitmap);
}

//draw the walls
void drawWalls(WALL* wall) {
    if (wall->active) {
        drawRectangle4(wall->x, wall->y, wall->width, wall->height, wall->color);
    }
}

//pause animation drawing
void pauseAnimation(int frame) {
    // drawImage4(100, 70, 100, 100, dance1Bitmap);
}

//helper function to check if player touches a wall
int wallCollision(WALL* wall) {
    return collision(wall->x, wall->y, wall->width, wall->height - 5, player.x, player.y, player.width, player.height - 5);
}