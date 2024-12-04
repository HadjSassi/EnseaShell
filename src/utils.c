#include "utils.h"
#include <stdio.h>
#include "constants.h"
#include "io.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>

float calculateTime(struct timespec *startTime, struct timespec *endTime) {
    long elapsedTimeSec = endTime->tv_sec - startTime->tv_sec;
    long elapsedTimeNs = endTime->tv_nsec - startTime->tv_nsec;

    if (elapsedTimeNs < INITIAL_SECOND) {
        elapsedTimeSec--;
        elapsedTimeNs += MILLISECOND_TO_SECOND;
    }

    long totalTimeNs = (elapsedTimeSec * MILLISECOND_TO_SECOND) + elapsedTimeNs;

    return totalTimeNs / ONE_SECOND_OF_MILLISECOND;
}

void redirectInput(const char *inputFile) {
    int inputFd = open(inputFile, O_RDONLY);
    if (inputFd == ELEMENT_NOT_FOUND) {
        perror(FAILED_TO_OPEN_FILE);
        _exit(EXIT_FAILURE);
    }
    if (dup2(inputFd, STDIN_FILENO) == ELEMENT_NOT_FOUND) {
        perror(FAILED_TO_REDIRECT);
        close(inputFd);
        _exit(EXIT_FAILURE);
    }
    close(inputFd);
}

void redirectOutput(const char *outputFile) {
    int outputFd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, PERMESSION);
    if (outputFd == ELEMENT_NOT_FOUND) {
        perror(FAILED_TO_OPEN_FILE);
        _exit(EXIT_FAILURE);
    }
    if (dup2(outputFd, STDOUT_FILENO) == ELEMENT_NOT_FOUND) {
        perror(FAILED_TO_REDIRECT);
        close(outputFd);
        _exit(EXIT_FAILURE);
    }
    close(outputFd);
}

void parseArguments(char *inputCopy, char **args, int *inputRedirected, int *outputRedirected, char **inputFile, char **outputFile) {
    char *token = strtok(inputCopy, SEPERATOR);
    int i = INITIAL_VALUE;

    while (token != NULL) {
        if (strcmp(token, REDIRECT_INPUT) == EQUALITY_VALUE) {
            *inputRedirected = TRUE;
            token = strtok(NULL, SEPERATOR);
            *inputFile = token;
        } else if (strcmp(token, REDIRECT_OUTPUT) == EQUALITY_VALUE) {
            *outputRedirected = TRUE;
            token = strtok(NULL, SEPERATOR);
            *outputFile = token;
        } else {
            args[i++] = token;
        }
        token = strtok(NULL, SEPERATOR);
    }
    args[i] = NULL;
}

void handleChildProcess(char **args, int inputRedirected, const char *inputFile, int outputRedirected, const char *outputFile) {
    if (inputRedirected) {
        redirectInput(inputFile);
    }

    if (outputRedirected) {
        redirectOutput(outputFile);
    }

    if (execvp(args[0], args) == ELEMENT_NOT_FOUND) {
        perror(EXECUTION_ERROR);
        _exit(EXIT_FAILURE);
    }
}

void handleParentProcess(pid_t pid, int *lastExitCode, struct timespec *startTime, struct timespec *endTime) {
    int status;
    waitpid(pid, &status, 0);
    clock_gettime(CLOCK_MONOTONIC, endTime);

    float elapsedTimeMs = calculateTime(startTime, endTime);

    if (WIFEXITED(status)) {
        *lastExitCode = WEXITSTATUS(status);
        display_message(EXIT_TEXT);
        char exitCode[SIZE_BUFFER];
        sprintf(exitCode, "%d", *lastExitCode);
        display_message(exitCode);
    } else if (WIFSIGNALED(status)) {
        *lastExitCode = -WTERMSIG(status);
        display_message(SIGNAL_TEXT);
        char signalNum[SIZE_BUFFER];
        sprintf(signalNum, "%d", -(*lastExitCode));
        display_message(signalNum);
    }

    char elapsedTimeStr[SIZE_BUFFER];
    sprintf(elapsedTimeStr, "|%.3fms", elapsedTimeMs);
    display_message(elapsedTimeStr);
    display_message(CLOSING_BRACKETS);
}

int execute_command(const char *inputBuffer, int *lastExitCode) {
    struct timespec startTime, endTime;
    clock_gettime(CLOCK_MONOTONIC, &startTime);

    int inputRedirected = FALSE, outputRedirected = FALSE;
    char *inputFile = NULL, *outputFile = NULL;
    char *args[BUFSIZE];

    char *inputCopy = strdup(inputBuffer);

    parseArguments(inputCopy, args, &inputRedirected, &outputRedirected, &inputFile, &outputFile);

    pid_t pid = fork();

    if (pid == CHILD_SELF_PID) {
        handleChildProcess(args, inputRedirected, inputFile, outputRedirected, outputFile);
    } else if (pid > CHILD_SELF_PID) {
        handleParentProcess(pid, lastExitCode, &startTime, &endTime);
    }

    free(inputCopy);
    return EXIT_SUCCESS;
}
