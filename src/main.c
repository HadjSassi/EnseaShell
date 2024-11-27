#include "io.h"
#include "constants.h"
#include <string.h>

#define BUFSIZE 1024

int main(void) {
    char inputBuffer[BUFSIZE];

    strcpy(inputBuffer, EMPTY_STRING);

    display_message(WELCOME_MSG);

    while (strcmp(inputBuffer, EXIT_KEY_WORD) != 0) {
        display_message(PROMPTING_MSG);

        if (!read_input(inputBuffer, BUFSIZE)) {
            break;
        }
    }
    return 0;
}
