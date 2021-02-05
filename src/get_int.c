#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

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

        // Remove newline char from end of string
        input[strcspn(input, "\n")] = 0;

        if (!check_digits(input)) {
            isInvalid = 1;
        }
    } while (isInvalid || (atoi(input) < min || atoi(input) > max));
    
    printf("\n");
    return atoi(input);
}

