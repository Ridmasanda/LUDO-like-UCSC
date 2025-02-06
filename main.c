#include "Logic.h"

int main()
{
    srand(time(NULL));
    
    printf("The Red player has four (04) pieces named R1, R2, R3, and R4\n");
    printf("The Green player has four (04) pieces named G1, G2, G3, and G4\n");
    printf("The Yellow player has four (04) pieces named Y1, Y2, Y3, and Y4\n");
    printf("The Blue player has four (04) pieces named B1, B2, B3, and B4\n");

    Player yellowPlayer = {"Yellow", {"Y1", "Y2", "Y3", "Y4"}, {-1, -1, -1, -1}, 2, YELLOW_HOME_BEGIN, {false, false, false, false}, {0, 0, 0, 0}};
    Player bluePlayer = {"Blue", {"B1", "B2", "B3", "B4"}, {-1, -1, -1, -1}, 15, BLUE_HOME_BEGIN, {false, false, false, false}, {0, 0, 0, 0}};
    Player redPlayer = {"Red", {"R1", "R2", "R3", "R4"}, {-1, -1, -1, -1}, 28, RED_HOME_BEGIN, {false, false, false, false}, {0, 0, 0, 0}};
    Player greenPlayer = {"Green", {"G1", "G2", "G3", "G4"}, {-1, -1, -1, -1}, 41, GREEN_HOME_BEGIN, {false, false, false, false}, {0, 0, 0, 0}};

    Player players[4] = {redPlayer, greenPlayer, yellowPlayer, bluePlayer};

    int diceRoll, pieceToMove;
    char *startPlayerColor;
    int startPlayerId = -1;
    bool gameEnd = false;
    int round = 1;

    // Determining the first player with the highest dice roll
    while (1)
    {
        int maxRoll = 0;
        int countMax = 0;

        for (int i = 0; i < 4; i++)
        {
            diceRoll = rollDice();

            if (i == 0)
                printf(YELLOW "%s rolls %d.\n" RESET, players[i].color, diceRoll);
            if (i == 1)
                printf(BLUE "%s rolls %d.\n" RESET, players[i].color, diceRoll);
            if (i == 2)
                printf(RED "%s rolls %d.\n" RESET, players[i].color, diceRoll);
            if (i == 3)
                printf(GREEN "%s rolls %d.\n" RESET, players[i].color, diceRoll);

            if (diceRoll > maxRoll)
            {
                maxRoll = diceRoll;
                startPlayerColor = players[i].color;
                startPlayerId = i;
                countMax = 1;
            }
            else if (diceRoll == maxRoll)
            {
                countMax++;
            }
        }

        if (countMax == 1)
        {
            printf("%s player has the highest roll and will begin the game!\n\n", startPlayerColor);
            printf("The order of a single round is ");
            for (int i = 0; i < 4; i++)
            {
                int currentIndex = (startPlayerId + i) % 4;
                printf("%s, ", players[currentIndex].color);
            }
            break;
             printf(".\n");
        }
        else
        {
            printf("There is a tie with the highest roll. Rolling again...\n\n");
        }
    }

    // Main game loop
    while (!gameEnd)
    {
        printf("-------------------------------------------------------\n");
        printf("\n--- Start of Round %d ---\n", round);

        // Iterate over each player in the round
        for (int i = 0; i < 4; i++)
        {
            int playerIndex = (startPlayerId + i) % 4;
            Player *currentPlayer = &players[playerIndex];

            diceRoll = rollDice();

            printf("%s rolls %d.\n", currentPlayer->color, diceRoll);

            if (strcmp(currentPlayer->color, "Yellow") == 0)
            {
                pieceToMove = yellowPlayerStrategy(currentPlayer, players, diceRoll);
            }
            else if (strcmp(currentPlayer->color, "Red") == 0)
            {
                pieceToMove = redPlayerStrategy(currentPlayer, players, diceRoll);
            }
            else if (strcmp(currentPlayer->color, "Blue") == 0)
            {
                pieceToMove = bluePlayerStrategy(currentPlayer, diceRoll);
            }
            else if (strcmp(currentPlayer->color, "Green") == 0)
            {
                pieceToMove = greenPlayerStrategy(currentPlayer, diceRoll);
            }

            if (pieceToMove != -1)
            {
                int newPosition = computeNewPosition(currentPlayer, pieceToMove, diceRoll);

                if (newPosition != -1)
                {
                    printf("%s moves piece %s from ", currentPlayer->color, currentPlayer->pieces[pieceToMove]);
                    if (currentPlayer->loc[pieceToMove] == -1)
                    {
                        printf("Base");
                    }
                    else if (currentPlayer->inHomePath[pieceToMove])
                    {
                        printf("%sHomePath[%d]", currentPlayer->color, currentPlayer->homePathProgress[pieceToMove]);
                    }
                    else
                    {
                        printf("L%d", currentPlayer->loc[pieceToMove]);
                    }

                    currentPlayer->loc[pieceToMove] = newPosition;

                    printf(" to ");
                    if (currentPlayer->inHomePath[pieceToMove])
                    {
                        printf("%sHomePath[%d]", currentPlayer->color, currentPlayer->homePathProgress[pieceToMove]);
                    }
                    else
                    {
                        printf("L%d", newPosition);
                    }
                    printf(".\n");

                    if (!currentPlayer->inHomePath[pieceToMove])
                    {
                        capturePiece(currentPlayer, players, pieceToMove);
                    }

                    if (checkPlayerWon(currentPlayer))
                    {
                        printf("%s player wins the game!!!\n", currentPlayer->color);
                        printf("\n");
                        gameEnd = true;
                        break;
                    }
                }

                else
                {
                    printf("%s has no valid moves for piece %s.\n", currentPlayer->color, currentPlayer->pieces[pieceToMove]);
                }
            }
            else
            {
                printf("%s has no valid pieces to move.\n", currentPlayer->color);
            }
        }

        if (gameEnd)
            break;

        // Print player positions at the end of the round
        printf("\n--- End of Round %d ---\n", round);
        printf("-------------------------------------------------------\n\n");
        for (int j = 0; j < 4; j++)
        {
            initializePlayerStatus(players[j]);
        }

        round++;
    }
    return 0;
}