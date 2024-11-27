#include "io.h"
#include "constants.h"
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFSIZE 1024

int main(void) {
    char inputBuffer[BUFSIZE];

    strcpy(inputBuffer, EMPTY_STRING);

    display_message(WELCOME_MSG);

    while (strcmp(inputBuffer, EXIT_KEY_WORD) != 0) {
        display_message(PROMPTING_MSG);

        if (!read_input(inputBuffer, BUFSIZE)) {
            display_message(FAREWELL_MSG);
            break;
        }

        if (strcmp(inputBuffer, EXIT_KEY_WORD) != 0 && fork() == CHILD_SELF_PID) {
            char *args[] = {inputBuffer, NULL};
            if (execvp(args[0], args) == COMMAND_NOT_FOUND) {
                display_message("Command not found.\n");
                _exit(COMMAND_NOT_FOUND);
            }
        } else {
            wait(NULL);
        }
    }

    display_message(FAREWELL_MSG);

    return 0;
}
