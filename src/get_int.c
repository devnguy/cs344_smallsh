#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "get_int.h"

const int MAX_LIMIT = 256;

/**
 * Checks if the parameter passed in as a string contains only numbers.
 * Ignores preceding '-' if present to allow negative numbers.
 * Returns true if each char is a digit.
 */
int check_digits(char* number) {
    int size = strlen(number);

    // Invalid if string is empty or number == "-"
    if (size == 0 || (size == 1 && number[0] == '-')) {
        return 0;
    }
    for (int i = 0; i < size; ++i) {
        if ((i != 0 || number[i] != '-') && !isdigit(number[i])) {
            return 0;
        }
    }
    return 1;
}

/**
 * Continues to prompt the user for input until they enter a string that 
 * can be properly converted to an int. Returns that string as an int.
 */
int get_int(char* prompt, int min, int max) {
    char input[MAX_LIMIT];
    int isInvalid = 0;

    do {
        if (isInvalid) {
            printf("Invalid input\n");
            isInvalid = 0;
        }
        printf("%s", prompt);

        fgets(input, MAX_LIMIT, stdin);

        // Scan input for newline char and remove
        input[strcspn(input, "\n")] = 0;

        if (!check_digits(input) || (atoi(input) < min || atoi(input) > max)) {
            isInvalid = 1;
        }
    } while (isInvalid);
    
    printf("\n");
    return atoi(input);
}

/**
 * Prompts the user to enter a string and removes the newline char.
 */
void get_str(char *prompt, char *str)
{
    char input[MAX_LIMIT];
    printf("%s", prompt);
    fgets(input, MAX_LIMIT, stdin);

    // Scan input for newline char and remove
    input[strcspn(input, "\n")] = 0;

    strcpy(str, input);
    printf("\n");
}

/**
 * Seeds random with the current time.
 */
void seed()
{
    srandom(time(NULL));
}

/**
 * Returns a peudorandomly generated int. seed() must be called before using
 * this function.
 */
int gen_rand_int(int min, int max)
{
    int num = (random() % (max - min + 1)) + min;
    return num;
}