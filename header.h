#include <stdio.h>
#include<string.h>
#include <stdlib.h>


#ifndef BATTLESHIP_H
#define BATTLESHIP_H
#define BOARD_SIZE 10
#define MAX_BUFF 100
#define MAX_SHIPS 10
#define LETTER_TO_NUM(x)  (x -65)


typedef struct coor
{
    char c;
    int num;
    struct coor* nextMove;
} coor;

typedef struct ship
{
    int alive;
    int size;
    int hits;
    coor* pos;
} ship;

typedef struct place
{

    int ship;
    int hit;
    int miss;
    // coor coor;

} place;

typedef struct player
{

    place board[BOARD_SIZE][BOARD_SIZE];
    coor *moves;
    ship ships[MAX_SHIPS];
    //int shipsAmount;


} player;

 enum boolean {FALSE, TRUE};

 //enum letters { A =  0, B, C, D, E, F, G, H, I, J};

 void printBoardGame(player *PLAYER, FILE* fp);

 int placeShip(player* PLAYER, coor posS, coor posE);

 int LoadBoardFromFile(FILE* fpBoard, player* PLAYER);

int runSimulation(player *player_1, player *player_2, FILE *fpSimulation, FILE *fpResult);

int initializeBoard(player *PLAYER, char* filename);

int initializeShips(ship *ship, coor posS, coor posE);

void attackSquare(player* PLAYER, coor MOVE);

int addMove (player* PLAYER, coor move);

void printMoves (FILE* fp, player* PLAYER, coor* moves);

int checknear(player *PLAYER, coor posS, coor posE);

int playGame(player *PLAYER_1, player *PLAYER_2, FILE *fpResult);

int checkShIpDown(player *PLAYER, coor MOVE);

int shipDownOnPos(player *PLAYER, int i, int j);

int checkVictory(player* PLAYER);

void freeDynamicArrays(player* PLAYER);

void exitFunction(player* player_1, player* player_2, int num);

int checkShipSizes(player* PLAYER);

void bubbleSort(int* arr, int size);

void bubbleSortDown(int arr[], int n);

void swap(int *x, int *y);

void resetPlayer(player* PLAYER);

#endif