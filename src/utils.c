//
// Created by keranis on 27/11/24.
//


#include "utils.h"

#include <stdlib.h>

#include "constants.h"
#include "io.h"
#include <unistd.h>
#include <sys/wait.h>

int execute_one_command(const char *inputBuffer, int *lastExitCode) {
    pid_t pid = fork();

    if (pid == CHILD_SELF_PID) {
        char *args[] = { (char *)inputBuffer, NULL };
        if (execvp(args[0], args) == ELEMENT_NOT_FOUND) {
            display_message(COMMAND_NOT_FOUND);
            _exit(ELEMENT_NOT_FOUND);
        }
    } else if (pid > CHILD_SELF_PID) {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            *lastExitCode = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            *lastExitCode = -WTERMSIG(status);
        } else {
            *lastExitCode = 0;
        }
    }
    return EXIT_SUCCESS;
}