/******************************************************************************
** Program name: Movie Files
** Author:       Huy Nguyen
** Date:         02/01/21
** Description:  This program processes a file containing movie data chosen by
**               the user. The movie titles will be added to .txt files based
**               on the year the movie was released
******************************************************************************/

#define _GNU_SOURCE

#include "menu.h"
#include "get_int.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int main(int argc, char *argv[])
{
    seed();
    menu();
    return EXIT_SUCCESS;
}
