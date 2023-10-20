typedef struct {
    int x;
    int y;
    int oldX;
    int oldY;
    int dx;
    int dy;
    int height;
    int width;
} PLAYER;

typedef struct {
    int x;
    int y;
    int dy;
    int height;
    int width;
    unsigned short color;
} TRAP;

typedef struct {
    int x;
    int y;
    int height;
    int width;
    unsigned short color;
    int active;
} CHEESE;

typedef struct {
    int y;
    int x;
    int height;
    int width;
    unsigned short color;
    int active;
} WALL;

#define TRAPCOUNT 5
#define CHEESECOUNT 3
#define WALLCOUNT 3

extern PLAYER player;
extern CHEESE cheese[CHEESECOUNT];
extern TRAP traps[TRAPCOUNT];
extern WALL walls[WALLCOUNT];
extern int score;
extern int total;
extern int lives;

enum { BLACKID=0, WHITEID, PINKID, GRAYID, GRAID, GRID, BLACKRID, YELLOWID };

void initGame();
void initPlayer();
void initCheese();
void initTraps();
void initWalls();
void updateGame();
void updatePlayer();
void updateCheese(CHEESE* cheese);
void updateTraps(TRAP* trap);
void updateWalls();
void drawGame();
void drawPlayer();
void drawCheese(CHEESE* cheese);
void drawTraps(TRAP* trap);
void drawWalls(WALL * wall);
int wallCollision(WALL* wall);

void pauseAnimation(int frame);