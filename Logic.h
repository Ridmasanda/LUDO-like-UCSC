#ifndef LOGIC_H
#define LOGIC_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "types.h"

int rollDice()
{
    return (rand() % 6) + 1;
}

void initializePlayerStatus(Player player)
{
    int boardPieceCount = 0;
    int basePieceCount = 0;
    printf("%s Pieces locations\n", player.color);
    printf("================================================================\n");

    for (int i = 0; i < 4; i++)
    {
        if (player.loc[i] == -1)
        {
            basePieceCount++;
        }
        else if (player.inHomePath[i])
        {
            printf(RED "%s ----> %sHomePath[%d] \n" RESET, player.pieces[i], player.color, player.homePathProgress[i]);
            boardPieceCount++;
        }
        else
        {
            printf("%s ----> cell L%d \n", player.pieces[i], player.loc[i]);
            boardPieceCount++;
        }
    }
    printf("\n%s player now has %d/4  pieces on the board and %d/4 pieces on the base.\n", player.color, boardPieceCount, basePieceCount);
    printf("================================================================\n");
}

int computeNewPosition(Player *player, int pieceIndex, int diceRoll)
{
    int currentPos = player->loc[pieceIndex];
    int newPos = currentPos;

    if (player->inHomePath[pieceIndex])
    {
        int updatedProgress = player->homePathProgress[pieceIndex] + diceRoll;
        if (updatedProgress > HOME_PATH)
        {
            return -1; // Cannot move, the move would exceed the home path
        }
        player->homePathProgress[pieceIndex] = updatedProgress;
        return player->homeStartCell; // We return homeStart as a flag that the piece is in home path
    }
    // Scenario 2: Piece is in the base
    else if (currentPos == -1)
    {
        // Moving the piece from the base to the starting cell
        return player->X;
    }
    // Scenario 3: Regular movement on the board
    else
    {
        newPos = (currentPos - 1 + diceRoll) % CELLS + 1;

        // Check if piece can enter the home path
        int distanceToHomeStart = (player->homeStartCell - currentPos + CELLS) % CELLS;
        if (diceRoll >= distanceToHomeStart)
        {
            player->inHomePath[pieceIndex] = true;
            player->homePathProgress[pieceIndex] = diceRoll - distanceToHomeStart + 1;
            return player->homeStartCell; // We return homeStart as a flag that the piece is in home path
        }
    }

    return newPos;
}

void capturePiece(Player *currentPlayer, Player players[], int pieceIndex)
{
    for (int i = 0; i < 4; i++)
    {
        // Check if the piece can be captured (same location, not in home path)
        if (strcmp(players[i].color, currentPlayer->color) == 0)
            continue;

        for (int j = 0; j < 4; j++)
        {
            if (players[i].loc[j] == currentPlayer->loc[pieceIndex] && !players[i].inHomePath[j] && !currentPlayer->inHomePath[pieceIndex])
            {
                printf("%s captures %s's piece %s!\n", currentPlayer->color, players[i].color, players[i].pieces[j]);
                // Move the captured piece back to the base
                players[i].loc[j] = -1;
                // Reset the captured piece's status regarding the home path
                players[i].inHomePath[j] = false;
                players[i].homePathProgress[j] = 0;
            }
        }
    }
}

int yellowPlayerStrategy(Player *yellowPlayer, Player players[], int diceRoll)
{
    for (int i = 0; i < 4; i++)
    {
        if (yellowPlayer->loc[i] == -1 && diceRoll == 6)
        {
            return i;
        }
    }

    int nearestToHome = -1;
    int smallestDistance = CELLS;

    for (int i = 0; i < 4; i++)
    {
        if (yellowPlayer->loc[i] != -1) // If the piece is not in the base
        {
            int newPosition = computeNewPosition(yellowPlayer, i, diceRoll);
            if (newPosition == -1)
                continue;

            int distanceHome;
            // If the piece is in the home path, calculate the distance to the end of the path
            if (yellowPlayer->inHomePath[i])
            {
                distanceHome = HOME_PATH - yellowPlayer->homePathProgress[i];
            }
            else
            {
                distanceHome = (yellowPlayer->homeStartCell - newPosition + CELLS) % CELLS;
            }

            if (distanceHome < smallestDistance)
            {
                nearestToHome = i;
                smallestDistance = distanceHome;
            }
        }
    }

    return nearestToHome;
}

int bluePlayerStrategy(Player *bluePlayer, int diceRoll)
{
    for (int i = 0; i < 4; i++)
    {
        int pieceToMove = (i + diceRoll) % 4; // Cyclically select a piece index based on the dice roll

        // Check if the selected piece is in the base
        if (bluePlayer->loc[pieceToMove] == -1)
        {
            if (diceRoll == 6)
                return pieceToMove;
            continue;
        }

        int newPosition = computeNewPosition(bluePlayer, pieceToMove, diceRoll);
        if (newPosition != -1)
        {
            return pieceToMove;
        }
    }

    return -1;
}

int redPlayerStrategy(Player *redPlayer, Player players[], int diceRoll)
{
    int targetIndex = -1;      // Index of the piece that can capture an opponent's piece
    int alternativeIndex = -1; // Index of the fallback piece to move if no capture is possible

    // Loop through all four pieces of the aggressive player
    for (int i = 0; i < 4; i++)
    {
        // Check if the piece is in the base (-1 indicates in the base)
        if (redPlayer->loc[i] == -1)
        {
            // If the dice roll is 6, the piece can be moved out of the base
            if (diceRoll == 6)
                return i;
            continue;
        }

        // Calculate the new position of the piece after the dice roll
        int newPosition = computeNewPosition(redPlayer, i, diceRoll);
        if (newPosition == -1)
            continue;

        // Loop through all competitors to check for possible captures
        for (int j = 0; j < 4; j++)
        {
            // Skip the check for the aggressive player's own pieces
            if (strcmp(players[j].color, redPlayer->color) == 0)
                continue;

            // Loop through all pieces of the current competitor
            for (int k = 0; k < 4; k++)
            {
                // Check if the new position of the aggressive player's piece matches a competitor's piece
                // and if neither piece is in their respective home paths
                if (newPosition == players[j].loc[k] && !players[j].inHomePath[k] && !redPlayer->inHomePath[i])
                {
                    targetIndex = i;
                    return targetIndex;
                }
            }
        }

        // If no capture is possible, consider this piece as a alternative option
        if (alternativeIndex == -1)
        {
            alternativeIndex = i;
        }
    }

    return alternativeIndex;
}

int greenPlayerStrategy(Player *greenPlayer, int diceRoll)
{
    int blockCandidateIndex = -1; // Index of the piece that can potentially create or maintain a block
    int validMoveIndex = -1;      // Index of the first valid movable piece found

    for (int i = 0; i < 4; i++) // Iterate over all four pieces of the green player
    {
        if (greenPlayer->loc[i] == -1)
        {
            if (diceRoll == 6)
                return i;
            continue;
        }

        int newPosition = computeNewPosition(greenPlayer, i, diceRoll);
        if (newPosition == -1)
            continue;

        // Check if this move creates or maintains a block
        for (int j = 0; j < 4; j++)
        {
            if (i != j && greenPlayer->loc[j] == newPosition)
            {
                blockCandidateIndex = i;
                break;
            }
        }

        if (blockCandidateIndex != -1)
            break;

        if (validMoveIndex == -1)
        {
            validMoveIndex = i;
        }
    }

    return (blockCandidateIndex != -1) ? blockCandidateIndex : validMoveIndex;
}

bool checkPlayerWon(Player *currentPlayer)
{
    for (int i = 0; i < 4; i++)
    {
        if (!currentPlayer->inHomePath[i] || currentPlayer->homePathProgress[i] < HOME_PATH)
        {
            return false;
        }
    }
    return true;
}

#endif