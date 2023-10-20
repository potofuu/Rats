#include "gba.h"
#include "mode4.h"
#include "print.h"
#include "background.h"
#include "start.h"
#include "game.h"
#include "dance2.h"
#include "dance1.h"
#include "sounds.h"
#include <stdio.h>
// Add makefile

void initialize();
void goToStart();
void start();
void goToGame();
void goToPause();
void pause();
void goToWin();
void win();
void goToLose();
void lose();
void game();

enum { START, GAME, PAUSE, LOSE, WIN };
int state;
int frames;
int total;
char buffer[41];
char buffer2[100];

unsigned short oldButtons;
unsigned short buttons;

// int rSeed;

int main() {
    initialize();

    while (1) {

        oldButtons = buttons;
        buttons = REG_BUTTONS;

        //Game state machine
        switch (state)
        {
        case START:
            start();
            break;
        case GAME:
            game();
            break;
        case PAUSE:
            pause();
            break;
        case WIN:
            win();
            break;
        case LOSE:
            lose();
            break;
        default:
            break;
        }

    }
}

void initialize() {
    mgba_open();
    //enable mode 4
    REG_DISPCTL = MODE(4) | BG_ENABLE(2) | DISP_BACKBUFFER;

    //enabling sound
    REG_SOUNDCNT_X = SND_ENABLED;

    REG_SOUNDCNT_L = DMG_VOL_LEFT(5) |
                   DMG_VOL_RIGHT(5) |
                   DMG_SND1_LEFT |
                   DMG_SND1_RIGHT |
                   DMG_SND2_LEFT |
                   DMG_SND2_RIGHT |
                   DMG_SND3_LEFT |
                   DMG_SND3_RIGHT |
                   DMG_SND4_LEFT |
                   DMG_SND4_RIGHT;
    REG_SOUNDCNT_H = DMG_MASTER_VOL(2);

    //track total, frames, buttons. Total should carry over from resets
    total = 0;
    frames = 0;
    buttons = REG_BUTTONS;
    oldButtons = 0;

    goToStart();
}

void goToStart() {
    //Load in the palette for the start screen
    DMANow(3, startPal, BG_PALETTE, 256);
    //draw start screen
    drawFullscreenImage4(startBitmap);
    // drawString4(50, 50, "Press Start", BLACK);
    
    waitForVBlank();
    flipPage();
    //move to start state
    state = START;

    // rSeed = 0;
}

void start() {
    // rSeed++;

    waitForVBlank();
    //start game upon start press
    if (BUTTON_PRESSED(BUTTON_START)) {
        // srand(rSeed);
        goToGame();
        initGame();
    }
}

void goToGame() {
    //sound effect on game start
    REG_SND2CNT = DMG_ENV_VOL(4) | DMG_STEP_TIME(2);
    REG_SND2FREQ = NOTE_C3 | SND_RESET;
    //move to game state
    state = GAME;
}

void game() {
    updateGame();
    drawGame();


    //draw and update score
    drawString4(180, 1, "score: ", BLACKID);
    sprintf(buffer, "%d", score);
    drawString4(220, 1, buffer, PINKID);


    //draw and update total, score is reset but total stays
    drawString4(0, 1, "Total Cheese : ", BLACKID);
    sprintf(buffer2, "%d", total);
    drawString4(85, 1, buffer2, PINKID);

    waitForVBlank();
    flipPage();

    //check for pause
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToPause();
    }

    //win if all the cheese is eaten
    if (score == 3) {
        goToWin();
    }
    //lose upon contact
    if (lives == 0) {
        goToLose();
    }
}

//pause screen, return to game state after clicking start again
void goToPause() {
    fillScreen4(BLACKID);
    drawString4(85, 80, "Game Paused", PINKID);

    waitForVBlank();
    flipPage();
    state = PAUSE;
}

//pause animation
void pause() {
    // frames++;
    // if (frames > 50) {
    //     frames = 0;
    // }
    // pauseAnimation(frames);

    // waitForVBlank();
    // flipPage();
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToGame();
    }
}

//transition to win screen
void goToWin() {
    fillScreen4(BLACKID);
    drawString4(90, 80, "You Win!", YELLOWID);

    REG_SND2CNT = DMG_ENV_VOL(4) | DMG_STEP_TIME(2);
    REG_SND2FREQ = NOTE_C4 | SND_RESET;

    waitForVBlank();
    flipPage();
    state = WIN;
}

//win screen, return to start of game upon button press
void win() {
    waitForVBlank();
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToStart();
    }
}

//transition to lose screen
void goToLose() {
    fillScreen4(BLACKID);
    drawString4(85, 80, "You Lose...", WHITEID);
    REG_SND1CNT = DMG_ENV_VOL(4) | DMG_SWEEP_STEPTIME(2) | DMG_STEP_TIME(5);
    REG_SND1FREQ = NOTE_A2 | SND_RESET;

    waitForVBlank();
    flipPage();
    state = LOSE;
}

//upon button press, return to game start
void lose() {
    waitForVBlank();
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToStart();
    }   
}