#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"
#include "colors.h"

/* Main function */
int main(int argc, char **argv)
{

    if (argc != 4)
    {
        red();
        printf("\n\nMissing/too many files\n");
        reset();
        exit(-1);
    }

    int selection, Err, check;
    char c;
    player player_1, player_2;
    FILE *fpBoard_1, *fpBoard_2, *fpSimulation, *fpResult;

    resetPlayer(&player_1);
    resetPlayer(&player_2);

    Err = initializeBoard(&player_1, argv[1]);
    exitFunction(&player_1, &player_2, Err);
    Err = initializeBoard(&player_2, argv[2]);
    exitFunction(&player_1, &player_2, Err);

    while (1)
    {
        blue();
        printf("\n\t\t _           _   _   _           _     _ \n");
        printf("\t\t| |         | | | | | |         | |   (_)\n");
        printf("\t\t| |__   __ _| |_| |_| | ___  ___| |__  _ _ __\n");
        printf("\t\t| '_ \\ / _` | __| __| |/ _ \\/ __| '_ \\| | '_ \\\n ");
        printf("\t\t| |_) | (_| | |_| |_| |  __/\\__ \\ | | | | |_) |\n");
        printf("\t\t|_.__/ \\__,_|\\__|\\__|_|\\___||___/_| |_|_| .__/\n ");
        printf("                                                       | |\n");
        printf("                                                        |_|\n");
        white();
        printf("1 -Run Simulation \t\t2 -Play Game\n3 -Exit game\n");
        yellow();
        printf("\nSelect==>");
        reset();
        scanf("%d", &selection);
        c = getchar();
        switch (selection)
        {
        case 1:

            do
            {
                check = 0;
                fpSimulation = fopen(argv[3], "r");

                if (fpSimulation == NULL)
                {
                    check = 1;
                    red();
                    printf("\n\nCouldn't load file: %s\n", argv[3]);
                    reset();
                    printf("Enter the correct file name:>\n");
                    scanf("%s", argv[3]);
                }
            } while (check == 1);

            fpResult = fopen("RESULT.txt", "w");
            if (fpResult == NULL)
            {
                Err = 7;
                exitFunction(&player_1, &player_2, Err);
            }
            /* Runs simiulation from file, on return frees dynamical arrays and intializes board again */
            Err = runSimulation(&player_1, &player_2, fpSimulation, fpResult);
            exitFunction(&player_1, &player_2, Err);
            freeDynamicArrays(&player_1);
            freeDynamicArrays(&player_2);
            Err = initializeBoard(&player_1, argv[1]);
            exitFunction(&player_1, &player_2, Err);
            Err = initializeBoard(&player_2, argv[2]);
            exitFunction(&player_1, &player_2, Err);
            fclose(fpSimulation);
            fclose(fpResult);
            yellow();
            printf("\n\n Game saved succesfully to file: RESULT.TXT");
            reset();
            break;
        case 2:
            fpResult = fopen("RESULT.txt", "w");
            if (fpResult == NULL)
            {
                Err = 7;
                exitFunction(&player_1, &player_2, Err);
            }
            /*  Plays game betwenn to players , on return frees dynamical arrays and intializes board again */
            Err = playGame(&player_1, &player_2, fpResult);
            exitFunction(&player_1, &player_2, Err);
            freeDynamicArrays(&player_1);
            freeDynamicArrays(&player_2);
            Err = initializeBoard(&player_1, argv[1]);
            exitFunction(&player_1, &player_2, Err);
            Err = initializeBoard(&player_2, argv[2]);
            exitFunction(&player_1, &player_2, Err);
            fclose(fpResult);
            yellow();
            printf("\n\n Game saved succesfully to file: RESULT.TXT");
            reset();
            break;
        case 3:
            freeDynamicArrays(&player_1);
            freeDynamicArrays(&player_2);
            return 0;
        }
    }
}
