#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//defines
#define WIDTH 300
#define HEIGHT 400
#define BLOCKSIZE 16

#define W ((WIDTH/BLOCKSIZE)-5)
#define H (HEIGHT/BLOCKSIZE)

#define E 1
#define B 2

#define forij for(int j=0;j<5;j++)for(int i=0;i<5;i++)

//globals
static char piece0[5][5] = {{E,E,E,E,E},
                            {E,E,E,B,E},
                            {E,B,B,B,E},
                            {E,E,E,E,E},
                            {E,E,E,E,E}};
static char piece1[5][5] = {{E,E,E,E,E},
                            {E,E,B,E,E},
                            {E,B,B,B,E},
                            {E,E,E,E,E},
                            {E,E,E,E,E}};
static char piece2[5][5] = {{E,E,E,E,E},
                            {E,E,E,E,E},
                            {E,B,B,E,E},
                            {E,E,B,B,E},
                            {E,E,E,E,E}};
static char piece3[5][5] = {{E,E,E,E,E},
                            {E,E,E,E,E},
                            {E,E,B,B,E},
                            {E,B,B,E,E},
                            {E,E,E,E,E}};
static char piece4[5][5] = {{E,E,E,E,E},
                            {E,B,E,E,E},
                            {E,B,B,B,E},
                            {E,E,E,E,E},
                            {E,E,E,E,E}};
static char piece5[5][5] = {{E,E,E,E,E},
                            {E,E,E,E,E},
                            {E,E,B,B,E},
                            {E,E,B,B,E},
                            {E,E,E,E,E}};
static char piece6[5][5] = {{E,E,E,E,E},
                            {E,E,E,E,E},
                            {B,B,B,B,B},
                            {E,E,E,E,E},
                            {E,E,E,E,E}};

static int colors[] = { 0xFF0000,
                        0x00FF00,
                        0x0000FF,
                        0xFFFF00,
                        0xFF00FF,
                        0x00FFFF,
                        0xFFFFFF };

char currPiece[5][5];
char nextPiece[5][5];
int currColor,nextColor;
int pieceX,pieceY;
int map[W][H];
long score;
int endCounter;
int levelCounter;
int timerInterval;
bool pieceCreated;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_TimerID timerID;

void selectPiece(char *piece) {
    int random = (rand() % 70) / 10;
    switch (random) {
    case 0:
        memcpy(piece, &(piece0[0][0]), sizeof(char) * 5 * 5);
        nextColor = colors[0];
        break;

    case 1:
        memcpy(piece, &(piece1[0][0]), sizeof(char) * 5 * 5);
        nextColor = colors[1];
        break;

    case 2:
        memcpy(piece, &(piece2[0][0]), sizeof(char) * 5 * 5);
        nextColor = colors[2];
        break;

    case 3:
        memcpy(piece, &(piece3[0][0]), sizeof(char) * 5 * 5);
        nextColor = colors[3];
        break;

    case 4:
        memcpy(piece, &(piece4[0][0]), sizeof(char) * 5 * 5);
        nextColor = colors[4];
        break;

    case 5:
        memcpy(piece, &(piece5[0][0]), sizeof(char) * 5 * 5);
        nextColor = colors[5];
        break;

    case 6:
        memcpy(piece, &(piece6[0][0]), sizeof(char) * 5 * 5);
        nextColor = colors[6];
        break;
    }
}

void initStack() {
    for (int j = 0; j < H - 1; j++) {
        for (int i = 0; i < W; i++) {
            if (i == 0 || i == W - 1)
                map[i][j] = 0x55555555;
            else
                map[i][j] = 0x00000000;
        }
    }
    for (int i = 0; i < W; i++) {
        map[i][H - 1] = 0x55555555;
    }

    score = 0;
    timerInterval = 500;
    endCounter = 0;
    pieceCreated = false;
}

void initPiece() {
    //selectPiece(&(currPiece[0][0]));
    memcpy(&(currPiece[0][0]), &(nextPiece[0][0]), sizeof(char) * 5 * 5);
    currColor = nextColor;


    selectPiece(&(nextPiece[0][0]));
    //nextColor = (rand() << 8) | 0xFF;

    pieceX = W / 3;
    pieceY = 0;
    pieceCreated = true;
}

bool checkCollision(int xinc,int yinc)
{
    forij
    {
        if(currPiece[i][j] == B && map[i+pieceX+xinc][j+pieceY+yinc] != 0)
            return true;
    }
    return false;
}

void pushPieceOntoStack()
{
    forij
    {
        if(currPiece[i][j] == B)
        {
            map[i+pieceX][j+pieceY] = currColor;
        }
    }
}

void rotatePiece()
{
    int temp[5][5];
    forij
    {
        temp[i][j] = currPiece[i][j];
    }
    forij
    {
        currPiece[i][j] = temp[4-j][i];
    }
    //if rotated piece has collision, revert
    if(checkCollision(0,0))
    {
        forij
        {
            currPiece[i][j] = temp[i][j];
        }
    }
}

void castShadow()
{
    int temp = pieceY;
    while(!checkCollision(0,1))
        pieceY++;

    //shadowPiece
    forij
    {
        if(currPiece[i][j] == B)
        {
            SDL_Rect rect;
            rect.x = (i+pieceX)*BLOCKSIZE;
            rect.y = (j+pieceY)*BLOCKSIZE;
            rect.w = rect.h = BLOCKSIZE;
            SDL_SetRenderDrawColor(renderer, 0x33, 0x33, 0x33, 0x33);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    //reset piece pos
    pieceY = temp;
}

void movePieceLeft()
{
    if(!checkCollision(-1,0))
        pieceX--;
}

void movePieceRight()
{
    if(!checkCollision(1,0))
        pieceX++;
}

void rowCheck()
{
    int dstRow = H-2;
    int srcRow = H-2;

    while(srcRow>=0)
    {
        bool isRowFull = true;
        //check row
        for(int i=1;i<W-1;i++)
        {
            if(map[i][srcRow]==0)
            {
                isRowFull = false;
                break;
            }
        }

        //shift row
        if(!isRowFull)
        {
            if(dstRow!=srcRow)
            {
                for(int i=1;i<W-1;i++)
                {
                    map[i][dstRow] = map[i][srcRow];
                }
            }
            srcRow--;
            dstRow--;
        }else
        {
            //we have a full row, so add points
            score+=7;
            printf("Score: %d\n",score);
            levelCounter++;
            if(levelCounter==10)
            {
                timerInterval-=50;
                levelCounter = 0;
            }

            srcRow--;
        }

    }

    //fill remaining rows with 0
    while(dstRow>=0)
    {
        for(int i=1;i<W-1;i++)
        {
            map[i][dstRow] = 0;
        }
        dstRow--;
    }
}

void dropPiece()
{
    if(!checkCollision(0,1))
    {
        pieceY++;
        if(pieceCreated)
            pieceCreated = false;
    }else
    {
        pushPieceOntoStack();
        initPiece();
        rowCheck();
        if(pieceCreated)
            endCounter++;
        //if stack is filled, quit
        if(endCounter>2)
        {
            SDL_Event quitEvent;
            quitEvent.type = SDL_QUIT;
            SDL_PushEvent(&quitEvent);
        }
    }
}

void dropDownPiece()
{
    while(!checkCollision(0,1))
        pieceY++;

    pushPieceOntoStack();
    initPiece();
    rowCheck();
}

void drawPiece()
{
    //currPiece
    forij
    {
        if(currPiece[i][j] == B)
        {
            SDL_Rect rect;
            rect.x = (i+pieceX)*BLOCKSIZE;
            rect.y = (j+pieceY)*BLOCKSIZE;
            rect.w = rect.h = BLOCKSIZE;
            SDL_SetRenderDrawColor(renderer, (currColor >> 24) & 0xff, (currColor >> 16) & 0xff, (currColor >> 8) & 0xff, (currColor) & 0xff);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    //nextPiece
    forij
    {
        if(nextPiece[i][j] == B)
        {
            SDL_Rect rect;
            rect.x = (i+W)*BLOCKSIZE;
            rect.y = (j)*BLOCKSIZE;
            rect.w = rect.h = BLOCKSIZE;
            SDL_SetRenderDrawColor(renderer, (nextColor >> 24) & 0xff, (nextColor >> 16) & 0xff, (nextColor >> 8) & 0xff, (nextColor) & 0xff);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void drawStack()
{
    for(int j=0;j<H;j++)
    {
        for(int i=0;i<W;i++)
        {
            SDL_Rect rect;
            rect.x = (i)*BLOCKSIZE;
            rect.y = (j)*BLOCKSIZE;
            rect.w = rect.h = BLOCKSIZE;
            SDL_SetRenderDrawColor(renderer, (map[i][j] >> 24) & 0xff, (map[i][j] >> 16) & 0xff, (map[i][j] >> 8) & 0xff, (map[i][j]) & 0xff);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void drawAll()
{
    SDL_SetRenderDrawColor(renderer, 0x99, 0x99, 0x99, 0x99);

    drawStack();
    castShadow();
    drawPiece();

    SDL_RenderPresent(renderer);
}

Uint32 timerCallback(Uint32 interval, void* data)
{
    dropPiece();

    drawAll();

    return interval;
}

void initGame()
{
    srand(time( NULL ));
    //select next piece first
    selectPiece(&(nextPiece[0][0]));
    nextColor = (rand()<<8) | 0xFF;

    initPiece();

    initStack();

    timerID = SDL_AddTimer(timerInterval, timerCallback, NULL);
    timerCallback(0, NULL);
}

void initSDL()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    window = SDL_CreateWindow("SDLTRIS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED ,WIDTH, HEIGHT, 0);

    renderer = SDL_CreateRenderer(window, -1, 0);
}

int loopGame()
{
    SDL_Event event;
    while (true) {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            SDL_Quit();
            return 0;

        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_UP:
                rotatePiece();
                drawAll();
                break;
            case SDLK_DOWN:
                dropDownPiece();
                drawAll();
                break;
            case SDLK_LEFT:
                movePieceLeft();
                drawAll();
                break;
            case SDLK_RIGHT:
                movePieceRight();
                drawAll();
                break;
            case SDLK_ESCAPE:
                SDL_Quit();
                return 0;
            }
            break;
        }

        SDL_Delay(100);
    }
    return 0;
}

int main(int argc, char *argv[])
{

    initSDL();
    initGame();
    loopGame();

    return 0;
}
