#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "header.h"
#include "colors.h"

int LoadBoardFromFile(FILE *fpBoard, player *PLAYER) /* Reads the ships file content and send it to initialize. */
{
    int counter = 0, Err;
    char buff[MAX_BUFF], c, c2;
    coor posS, posE;

    rewind(fpBoard);

    while (1)
    {

        fgets(buff, MAX_BUFF, fpBoard);

        if (buff[0] == '#')
            continue;

        if (sscanf(buff, "%c%d%c%c%d", &posS.c, &posS.num, &c, &posE.c, &posE.num) == 5)
        {
            if (counter > 9) /* Returns error number 4*/
                return 4;
            Err = placeShip(PLAYER, posS, posE);
            if (Err != 0)
                return Err;
            Err = initializeShips(&PLAYER->ships[counter], posS, posE);
            if (Err != 0)
                return Err;
            counter++;
        }

        /* skips on empty lines*/
        do
        {
            c2 = getc(fpBoard);

            if (c2 != '\n')
                ungetc(c2, fpBoard);

        } while (c2 == '\n');

        if (feof(fpBoard))
            break;
    }

    if (counter < 10) /* returns error number 5 */
        return 5;

    Err = checkShipSizes(PLAYER);

    return Err;
}

int initializeShips(ship *ship, coor posS, coor posE) /* Initialize a ship content to the Player ships array. */
{
    int Err;
    ship->alive = TRUE;
    ship->hits = 0;

    if (posS.c == posE.c)
    {
        /* Allocating memmory for ship pos according to size*/
        ship->size = posE.num - posS.num + 1;
        if (ship->size > 4 || ship->size <= 0) /* Returns error number 3 */
            return 3;
        ship->pos = (coor *)malloc(ship->size * sizeof(coor));
        if (ship->pos == NULL) /* Returns error number 2 */
            return 2;

        /* Load positions to ship's pos array*/
        for (int i = 0; i < ship->size; i++)
        {
            ship->pos[i].c = posS.c;
            ship->pos[i].num = posS.num + i;
        }
    }
    if (posS.num == posE.num)
    {
        ship->size = posE.c - posS.c + 1;
        if (ship->size > 4 || ship->size <= 0) /* Returns error number 3 */
            return 3;
        ship->pos = (coor *)malloc(ship->size * sizeof(coor));
        if (ship->pos == NULL) /* Returns error number 2 */
            return 2;
        /* Load positions to ship's pos array*/
        for (int i = 0; i < ship->size; i++)
        {
            ship->pos[i].c = posS.c + i;
            ship->pos[i].num = posS.num;
        }
    }

    return 0;
}

int placeShip(player *PLAYER, coor posS, coor posE) /* Place a ship on the board. */
{
    int Err;
    Err = checknear(PLAYER, posS, posE);

    if (posS.c == posE.c)
    {
        for (int i = posS.num - 1; i < posE.num; i++)
            PLAYER->board[i][LETTER_TO_NUM(posS.c)].ship = TRUE;
    }

    else if (posS.num == posE.num)
    {
        for (int i = LETTER_TO_NUM(posS.c); i <= LETTER_TO_NUM(posE.c); i++)
            PLAYER->board[posS.num - 1][i].ship = TRUE;
    }
    return Err;
}

void printBoardGame(player *PLAYER, FILE *fp) /* Prints the board game. */
{
    if (fp == NULL)
        fp = stdout;
    white();
    fprintf(fp, "%2c|", ' ');
    reset();

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        purple();
        fprintf(fp, " %1c ", 'A' + i);

        white();
        fprintf(fp, "|");
    }

    fprintf(fp, "\n");

    for (int i = 0; i < 10; i++)
    {
        white();
        fprintf(fp, "--+---+---+---+---+---+---+---+---+---+---|\n");
        purple();
        fprintf(fp, "%2d", i + 1);
        for (int k = 0; k < BOARD_SIZE; k++)
        {
            if (PLAYER->board[i][k].hit == TRUE)
            {
                switch (shipDownOnPos(PLAYER, i, k))
                {
                case TRUE:
                    white();
                    fprintf(fp, "|");
                    yellow();
                    fprintf(fp, " %1c ", '&');
                    break;

                case FALSE:
                    white();
                    fprintf(fp, "|");
                    green();
                    fprintf(fp, " %1c ", 'O');
                    break;
                }
            }

            else if (PLAYER->board[i][k].miss == TRUE)
            {
                white();
                fprintf(fp, "|");
                red();
                fprintf(fp, " %1c ", 'X');
            }

            else
            {
                white();
                fprintf(fp, "| %1c ", ' ');
            }
        }
        white();
        fprintf(fp, "|\n");
    }
    white();
    fprintf(fp, "-------------------------------------------\n");
    reset();
}

int initializeBoard(player *PLAYER, char *filename) /*Resets board and send it to LoadBoardFromFile function. */
{
    int Err, check;
    FILE *fpBoard;

    do
    {
        check = 0;
        fpBoard = fopen(filename, "r");

        /* If couldn't open file from argc asks users to enter file name*/

        if (fpBoard == NULL)
        {
            check = 1;
            red();
            printf("\n\nCouldn't load file: %s\n", filename);
            reset();
            printf("\nEnter the correct file name:>\n");
            scanf("%s", filename);
        }
    } while (check == 1);

    /* Initialized all values on board to false*/
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int k = 0; k < BOARD_SIZE; k++)
        {
            PLAYER->board[i][k].ship = FALSE;
            PLAYER->board[i][k].hit = FALSE;
            PLAYER->board[i][k].miss = FALSE;
        }
    }

    /*Loads players ships from file*/
    Err = LoadBoardFromFile(fpBoard, PLAYER);

    fclose(fpBoard);
    PLAYER->moves = NULL;
    return Err;
}

void attackSquare(player *PLAYER, coor MOVE) /* Place the attack on board. */
{

    if (PLAYER->board[MOVE.num - 1][LETTER_TO_NUM(MOVE.c)].ship == TRUE)
        PLAYER->board[MOVE.num - 1][LETTER_TO_NUM(MOVE.c)].hit = TRUE;
    else
        PLAYER->board[MOVE.num - 1][LETTER_TO_NUM(MOVE.c)].miss = TRUE;
}

int addMove(player *PLAYER, coor move) /* Assings given move to player's moves linked list*/
{

    coor *pMove;
    /* Allocating memory for move*/
    pMove = (coor *)malloc(sizeof(coor));
    if (pMove == NULL)
        return 2;

    /*Assigns move*/
    pMove->c = move.c;
    pMove->num = move.num;
    /*Links move in palyers' moves linked list*/
    pMove->nextMove = PLAYER->moves;
    PLAYER->moves = pMove;

    return 0;
}

void printMoves(FILE *fp, player *PLAYER, coor *moves) /*Prints player's moves linked list recursively */
{
    if (moves == NULL)
        return;

    if (fp == NULL)
        fp = stdout;

    printMoves(fp, PLAYER, moves->nextMove);

    fprintf(fp, "%c%d ", moves->c, moves->num);

    if (PLAYER->board[moves->num - 1][(int)LETTER_TO_NUM(moves->c)].hit == TRUE)
        fprintf(fp, " HIT!\n");
    else
        fprintf(fp, " MISS\n");
}

int runSimulation(player *player_1, player *player_2, FILE *fpSimulation, FILE *fpResult) /*Loads simulation file and plays the game according to instructions in file  */
{
    int won = 0, Err = 0;
    char buff[MAX_BUFF], playerBuff[MAX_BUFF], c1, c2;
    coor move, *pMove;

    while (1)
    {
        /* Gets line from file*/
        fgets(buff, MAX_BUFF, fpSimulation);
        /* Removing trailing new line */
        buff[strcspn(buff, "\n")] = '\0';
        /* if line starts with # ignores it */
        if (buff[0] == '#')
            continue;
        /* if line containtns player's move */
        if (sscanf(buff, "%s%c%c%d%c", playerBuff, &c1, &move.c, &move.num) == 4)
        {
            /* checks for valid move*/
            if (move.c > 'J' || move.c < 'A' || move.num > 10 || move.num < 0)
            {
                fprintf(fpResult, "Lost turn\n");
                continue;
            }
            /* check's which player move*/
            if (strcmp(playerBuff, "PLAYER_1") == 0 && won == 0)
            {
                /* Checks that player attacks free square*/
                if (player_2->board[move.num - 1][LETTER_TO_NUM(move.c)].miss == TRUE || player_2->board[move.num - 1][LETTER_TO_NUM(move.c)].hit == TRUE)
                {
                    printf("Square already attacked - lost turn\n");
                    fprintf(fpResult, "PLAYER_2 %c%d Lost Turn\n", move.c, move.num);
                    continue;
                }

                fprintf(fpResult, "%s ", buff);

                attackSquare(player_2, move);

                Err = addMove(player_1, move);
                if (Err != 0)
                    return Err;
                /* Checks for hit or miss */
                if (player_2->board[move.num - 1][LETTER_TO_NUM(move.c)].hit == TRUE)
                {
                    fprintf(fpResult, "HIT!\n");
                    if (checkShIpDown(player_2, move) == TRUE)
                    {
                        fprintf(fpResult, "Congratulation! ship is down\n");
                        if (checkVictory(player_2) == TRUE)
                        {
                            fprintf(fpResult, "Player 1 won!\n");
                            won = 1;
                        }
                    }
                }
                else
                    fprintf(fpResult, "Miss\n");
            }
            /* check's which player move*/
            else if (strcmp(playerBuff, "PLAYER_2") == 0 && won == 0)
            {
                /* Checks that player attacks free square*/
                if (player_1->board[move.num - 1][move.c - 65].miss == TRUE || player_1->board[move.num - 1][move.c - 65].hit == TRUE)
                {
                    printf("Square already attacked - lost turn\n");
                    fprintf(fpResult, "PLAYER_2 %c%d Lost Turn\n", move.c, move.num);
                    continue;
                }
                fprintf(fpResult, "%s ", buff);

                attackSquare(player_1, move);
                Err = addMove(player_2, move);
                if (Err != 0)
                    return Err;

                /* Checks for hit or miss */
                if (player_1->board[move.num - 1][LETTER_TO_NUM(move.c)].hit == TRUE)
                {
                    fprintf(fpResult, "HIT!\n");
                    if (checkShIpDown(player_1, move) == TRUE)
                    {
                        fprintf(fpResult, "Congratulation! ship is down\n");
                        if (checkVictory(player_1) == TRUE)
                        {
                            fprintf(fpResult, "Player 2 won!\n");
                            won = 1;
                        }
                    }
                }
                else
                    fprintf(fpResult, "Miss\n");
            }
        }

        if (strcmp(buff, "PRINT_BOARD") == 0)
        {
            fprintf(fpResult, "\nPlayer 1 Board:\n\n");
            printBoardGame(player_1, fpResult);
            fprintf(stdout, "\nPlayer 1 Board:\n\n");
            printBoardGame(player_1, stdout);

            fprintf(fpResult, "\nPlayer 2 Board:\n\n");
            printBoardGame(player_2, fpResult);
            fprintf(stdout, "\nPlayer 2 Board:\n\n");
            printBoardGame(player_2, stdout);
        }

        if (strcmp(buff, "PRINT_ MOVEMENTS") == 0)
        {
            fprintf(fpResult, "\nPlayer 1 Moves:\n\n");
            printMoves(fpResult, player_2, player_1->moves);

            fprintf(fpResult, "\nPlayer 2 Moves:\n\n");
            printMoves(fpResult, player_1, player_2->moves);
            fprintf(fpResult, "\n");
        }

        /*Skips on empty new lines*/
        do
        {
            c2 = getc(fpSimulation);

            if (c2 != '\n')
                ungetc(c2, fpSimulation);

        } while (c2 == '\n');

        if (feof(fpSimulation))
            break;
    }

    if (won == 0)
    {
        yellow();
        printf("\n\nPress 'Y'/ 'y' to continue playing the game or any other key to quit\n");
        reset();
        c1 = getchar();

        if (c1 == 'y' || c1 == 'Y')
            Err = playGame(player_1, player_2, fpResult);
    }
    return Err;
}

int checknear(player *PLAYER, coor posS, coor posE) /* Checks if if ship is placed on top or near to another ship.*/
{
    if (posS.c == posE.c)
    {
        for (int i = posS.num - 2; i <= posE.num; i++)
        {
            if (i < 0 || i > 9)
                continue;
            for (int j = LETTER_TO_NUM(posS.c) - 1; j <= LETTER_TO_NUM(posE.c) + 1; j++)
            {
                if (j < 0 || j > 9)
                    continue;

                if (PLAYER->board[i][j].ship == TRUE)
                {
                    /*If ships are bordering or on top each other returns error 1*/
                    return 1;
                }
            }
        }
    }
    if (posS.num == posE.num)
    {
        for (int i = LETTER_TO_NUM(posS.c) - 1; i <= LETTER_TO_NUM(posE.c) + 1; i++)
        {
            if (i < 0 || i > 9)
                continue;
            for (int j = posS.num - 2; j <= posE.num; j++)
            {
                if (j < 0 || j > 9)
                    continue;

                if (PLAYER->board[j][i].ship == TRUE)
                {
                    /*If ships are bordering or on top each other returns error 1*/
                    return 1;
                }
            }
        }
    }
    return 0;
}

int playGame(player *PLAYER_1, player *PLAYER_2, FILE *fpResult) /* Function that playes the game in turns between two players */
{

    int counter = 0, Err = 0;
    coor MOVE;
    yellow();
    printf("\n\n\t\t~Battleship Game~\n\n");
    fprintf(fpResult, "\t\t~Battleship Game~\n\n");
    while (1)
    {
        switch (counter % 2)
        {
        case 0:
            blue();
            printf("\n\t\tPlayer 1 Turn!\n");
            printBoardGame(PLAYER_2, NULL);
            white();
            printf("Enter a square to attack:> (Y0 for exit)\n");

            fflush(stdin);
            if (scanf("%c%d", &MOVE.c, &MOVE.num) == 2)
            {
                if (MOVE.c == 'Y' && MOVE.num == 0)
                    return 0;

                if (MOVE.c > 'J' || MOVE.c < 'A' || MOVE.num > 10 || MOVE.num < 0)
                {
                    red();
                    printf("\nInvalid move - lost turn\n");
                    fprintf(fpResult, "PLAYER_1 %c%d Lost Turn\n", MOVE.c, MOVE.num);
                    counter++;
                    break;
                }
                if (PLAYER_2->board[MOVE.num - 1][MOVE.c - 65].miss == TRUE || PLAYER_2->board[MOVE.num - 1][MOVE.c - 65].hit == TRUE)
                {
                    red();
                    printf("\nSquare already attacked - lost turn\n");
                    fprintf(fpResult, "PLAYER_1 %c%d Lost Turn\n", MOVE.c, MOVE.num);
                    counter++;
                    break;
                }
            }
            else
            {
                red();
                printf("\nInvalid move - lost turn\n");
                fprintf(fpResult, "PLAYER_1 %c%d Lost Turn\n", MOVE.c, MOVE.num);
                counter++;
                break;
            }

            attackSquare(PLAYER_2, MOVE);
            Err = addMove(PLAYER_1, MOVE);
            if (Err != 0)
                return Err;

            if (PLAYER_2->board[MOVE.num - 1][MOVE.c - 65].hit == TRUE)
            {
                green();
                fprintf(stdout, "HIT - You get another turn!\n");
                fprintf(fpResult, "PLAYER_1 %c%d HIT!\n", MOVE.c, MOVE.num);
                if (checkShIpDown(PLAYER_2, MOVE) == TRUE)
                {
                    yellow();
                    fprintf(fpResult, "Congratulations! Ship is down.\n");
                    printf("Congratulations! Ship is down.\n");
                    if (checkVictory(PLAYER_2) == TRUE)
                    {
                        yellow();
                        fprintf(fpResult, "\n\n\t\tPlayer 1 won!\n");
                        fprintf(stdout, "\n\n\t\tPlayer 1 won!\n");
                        reset();
                        return 0;
                    }
                }
                break;
            }
            else
            {
                red();
                fprintf(stdout, "Miss\n");
                fprintf(fpResult, "PLAYER_1 %c%d miss\n", MOVE.c, MOVE.num);
                counter++;
                break;
            }

        case 1:
            red();
            printf("\n\t\tPlayer 2 Turn!\n");
            printBoardGame(PLAYER_1, NULL);
            white();
            printf("Enter a square to attack:> (Y0 for exit)\n");

            fflush(stdin);
            if (scanf("%c%d", &MOVE.c, &MOVE.num) == 2)
            {
                if (MOVE.c == 'Y' && MOVE.num == 0)
                    return 0;

                if (MOVE.c > 'J' || MOVE.c < 'A' || MOVE.num > 10 || MOVE.num < 0)
                {
                    red();
                    printf("\nUnvalid move - lost turn\n");
                    counter++;
                    break;
                }
                if (PLAYER_1->board[MOVE.num - 1][MOVE.c - 65].miss == TRUE || PLAYER_1->board[MOVE.num - 1][MOVE.c - 65].hit == TRUE)
                {
                    red();
                    printf("\nSquare already attacked - lost turn\n");
                    fprintf(fpResult, "PLAYER_2 %c%d Lost Turn\n", MOVE.c, MOVE.num);
                    counter++;
                    break;
                }
            }
            else
            {
                red();
                printf("\nInvalid move - lost turn\n");
                fprintf(fpResult, "PLAYER_1 %c%d Lost Turn\n", MOVE.c, MOVE.num);
                counter++;
                break;
            }
            attackSquare(PLAYER_1, MOVE);
            Err = addMove(PLAYER_2, MOVE);
            if (Err != 0)
                return Err;

            if ((PLAYER_1->board[MOVE.num - 1][MOVE.c - 65].hit == TRUE) && (PLAYER_1->board[MOVE.num - 1][MOVE.c - 65].hit = TRUE))
            {
                green();
                fprintf(stdout, "HIT - You get another turn!\n");
                fprintf(fpResult, "PLAYER_2 %c%d HIT!\n", MOVE.c, MOVE.num);
                if (checkShIpDown(PLAYER_1, MOVE) == TRUE)
                {
                    yellow();
                    fprintf(fpResult, "Congratulations! Ship is down.\n");
                    printf("Congratulations! Ship is down.\n");
                    if (checkVictory(PLAYER_1) == TRUE)
                    {
                        yellow();
                        fprintf(fpResult, "\n\n\t\tPlayer 2 won!");
                        fprintf(stdout, "\n\n\t\tPlayer 2 won!\n");
                        reset();
                        return 0;
                    }
                }
                break;
            }
            else
            {
                red();
                fprintf(stdout, "Miss\n");
                fprintf(fpResult, "PLAYER_2 %c%d miss\n", MOVE.c, MOVE.num);
                counter++;
                break;
            }
        }
    }
}

int checkShIpDown(player *PLAYER, coor MOVE) /* Checks if ship got hit in all positions */
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < PLAYER->ships[i].size; j++)
        {
            if (PLAYER->ships[i].pos[j].num == MOVE.num && PLAYER->ships[i].pos[j].c == MOVE.c)
            {
                PLAYER->ships[i].hits++;
                if (PLAYER->ships[i].hits == PLAYER->ships[i].size)
                {
                    PLAYER->ships[i].alive = FALSE;
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

int shipDownOnPos(player *PLAYER, int i, int j) /* Makes the print board game function print destroyed ship  */
{
    for (int l = 0; l < 10; l++)
    {
        if (PLAYER->ships[l].alive == FALSE)
        {
            for (int m = 0; m < PLAYER->ships[l].size; m++)
            {
                if ((PLAYER->ships[l].pos[m].num - 1 == i) && (LETTER_TO_NUM(PLAYER->ships[l].pos[m].c) == j))
                {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

int checkVictory(player *PLAYER) /* Cheack if player won */
{
    for (int i = 0; i < MAX_SHIPS; i++)
        if (PLAYER->ships[i].alive == TRUE)
            return FALSE;
    return TRUE;
}

void freeDynamicArrays(player *PLAYER) /* Frees all dynamical allocated pointers*/
{
    coor *MOVE = PLAYER->moves;
    for (int i = 0; i < MAX_SHIPS; i++)
    {
        if (PLAYER->ships[i].pos != NULL)
            free(PLAYER->ships[i].pos);
    }

    while (PLAYER->moves != NULL)
    {
        MOVE = PLAYER->moves;
        PLAYER->moves = MOVE->nextMove;
        free(MOVE);
    }
}

int checkShipSizes(player *PLAYER) /* Check if ship sizes are accoring to instructions */
{

    int arrSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    int arrShips[MAX_SHIPS];

    for (int i = 0; i < MAX_SHIPS; i++)
        arrShips[i] = PLAYER->ships[i].size;

    bubbleSortDown(arrShips, MAX_SHIPS);

    for (int i = 0; i < MAX_SHIPS; i++)
    {
        if (arrSizes[i] != arrShips[i])
            return 6;
    }
    return 0;
}

void swap(int *x, int *y) /* swap function fo bubblesort */
{
    int t = *x;
    *x = *y;
    *y = t;
}

void bubbleSortDown(int arr[], int n) /* bubble sort int array */
{
    int i, count = n;
    while (count > 1)
    {
        for (i = 0; i < n - 1; i++)
            if (arr[i] < arr[i + 1])
                swap(arr + i, arr + i + 1);
        --count;
    }
}

void exitFunction(player *player_1, player *player_2, int num) /* Switches between errors */
{

    switch (num)
    {
    case 1:
        red();
        printf("\n\nShips are Placed in Bordering positions!\n");
        reset();
        freeDynamicArrays(player_1);
        freeDynamicArrays(player_2);
        yellow();
        printf("\nFree complited\n");
        reset();
        exit(-1);

    case 2:
        red();
        printf("\n\nMemory allocation failed!\n");
        reset();
        freeDynamicArrays(player_1);
        freeDynamicArrays(player_2);
        yellow();
        printf("\nFree complited\n");
        reset();
        exit(-1);

    case 3:
        red();
        printf("\nShip of bigger/smaller size then required!\n");
        reset();
        freeDynamicArrays(player_1);
        freeDynamicArrays(player_2);
        yellow();
        printf("\nFree complited\n");
        reset();
        exit(-1);

    case 4:
        red();
        printf("\nToo Many ships then required!\n");
        reset();
        freeDynamicArrays(player_1);
        freeDynamicArrays(player_2);
        printf("\nFree complited\n");
        exit(-1);

    case 5:
        red();
        printf("\nLess ships then required !\n");
        reset();
        freeDynamicArrays(player_1);
        freeDynamicArrays(player_2);
        yellow();
        printf("\nFree complited\n");
        reset();
        exit(-1);

    case 6:
        red();
        printf("\nSizes of ships dont match with the instructions.\n");
        freeDynamicArrays(player_1);
        freeDynamicArrays(player_2);
        yellow();
        printf("\nFree complited\n");
        reset();
        exit(-1);
        break;
    case 7:
        red();
        printf("\nCouldn't open RESULT.txt file.\n");
        reset();
        freeDynamicArrays(player_1);
        freeDynamicArrays(player_2);
        yellow();
        printf("\nFree complited\n");
        reset();
        exit(-1);
        break;

    case 0:
        return;
        break;
    }
}

void resetPlayer(player *PLAYER) /* Sets all pointers in player struct to NULL*/
{
    for (int i = 0; i < MAX_SHIPS; i++)
    {
        PLAYER->ships[i].pos = NULL;
        PLAYER->ships[i].pos = NULL;
    }

    PLAYER->moves = NULL;
    PLAYER->moves = NULL;
}