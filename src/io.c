//
// Created by mahdi on 27/11/2024.
//

#include "io.h"
#include "constants.h"

#include <string.h>
#include <unistd.h>


void display_message(const char *message) {
    write(STDOUT_FILENO, message, strlen(message));
}

void display_prompt() {
    display_message(PROMPTING_MSG);
}

int read_input(char *buffer, size_t size) {
    ssize_t bytesRead = read(STDIN_FILENO, buffer, size - 1);
    if (bytesRead > 0) {
        buffer[bytesRead - 1] = '\0';
        return 1;
    }
    return 0;
}