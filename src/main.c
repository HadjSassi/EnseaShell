#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "constants.h"
#include "utils.h"
#include <string.h>
#include <unistd.h>

int main(void) {
    char inputBuffer[BUFSIZE];
    int lastExitCode = 0;
    strcpy(inputBuffer, EMPTY_STRING);

    display_message(WELCOME_MSG);

    display_message(PROMPTING_MSG);
    while (strcmp(inputBuffer, EXIT_KEY_WORD) != EQUALITY_VALUE) {
        if (!read_input(inputBuffer, BUFSIZE)) {
            display_message(FAREWELL_MSG);
            break;
        }
        if (strcmp(inputBuffer, EXIT_KEY_WORD) != EQUALITY_VALUE)
            execute_command(inputBuffer, &lastExitCode);
    }

    display_message(FAREWELL_MSG);

    return EXIT_SUCCESS;
}
