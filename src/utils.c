//
// Created by keranis on 27/11/24.
//

#include "utils.h"
#include <stdio.h>
#include "constants.h"
#include "io.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

float calculateTime(struct timespec *startTime, struct timespec *endTime) {
    long elapsedTimeMs = (endTime->tv_sec - startTime->tv_sec) * 1000
                         + (endTime->tv_nsec - startTime->tv_nsec) / 1000000;
    return (float)elapsedTimeMs;
}

int execute_one_command(const char *inputBuffer, int *lastExitCode) {
    struct timespec startTime, endTime;
    clock_gettime(CLOCK_MONOTONIC, &startTime);
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
        clock_gettime(CLOCK_MONOTONIC, &endTime);

        float elapsedTimeMs = calculateTime(&startTime, &endTime);

        if (WIFEXITED(status)) {
            *lastExitCode = WEXITSTATUS(status);
            display_message("[exit:");
            char exitCode[10];
            sprintf(exitCode, "%d", *lastExitCode);
            display_message(exitCode);
        } else if (WIFSIGNALED(status)) {
            *lastExitCode = -WTERMSIG(status);
            display_message("[sign:");
            char signalNum[10];
            sprintf(signalNum, "%d", -(*lastExitCode));
            display_message(signalNum);
        }

        char elapsedTimeStr[20];
        sprintf(elapsedTimeStr, "|%ldms", elapsedTimeMs);
        display_message(elapsedTimeStr);
        display_message("] ");
    }
    return EXIT_SUCCESS;
}


