#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "constants.h"
#include "utils.h"
#include <string.h>
#include <unistd.h>

int main(void) {
    char inputBuffer[BUFSIZE];
    int lastExitCode = 0 ;
    strcpy(inputBuffer, EMPTY_STRING);

    display_message(WELCOME_MSG);

    while (strcmp(inputBuffer, EXIT_KEY_WORD) != EQUALITY_VALUE) {
        display_message(PROMPTING_MSG);

        if (lastExitCode >= 0) {
            display_message("[exit:");
            char exitCode[10];
            sprintf(exitCode, "%d", lastExitCode);
            display_message(exitCode);
            display_message("] ");
        } else {
            display_message("[sign:");
            char signalNum[10];
            sprintf(signalNum, "%d", -lastExitCode);
            display_message(signalNum);
            display_message("] ");
        }

        if (!read_input(inputBuffer, BUFSIZE)) {
            display_message(FAREWELL_MSG);
            break;
        }

        execute_one_command(inputBuffer, &lastExitCode);
    }

    display_message(FAREWELL_MSG);

    return EXIT_SUCCESS;
}
