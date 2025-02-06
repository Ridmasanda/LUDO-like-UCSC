#include <stdbool.h>

#ifndef TYPES_H
#define TYPES_H

#define YELLOW "\x1b[31m"
#define BLUE "\x1b[32m"
#define RED "\x1b[33m"
#define GREEN "\x1b[34m"
#define RESET "\x1b[0m"

#define CELLS 52
#define HOME_PATH 6
#define STANDARD_PATH_SIZE 52

// Define home path positions
#define YELLOW_HOME_BEGIN 1
#define BLUE_HOME_BEGIN 14
#define RED_HOME_BEGIN 27
#define GREEN_HOME_BEGIN 40

typedef struct Player
{
    char color[15];
    char pieces[4][3];
    int loc[4];
    int X;
    int homeStartCell;
    bool inHomePath[4];
    int homePathProgress[4];
} Player;

#endif
