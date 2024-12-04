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

void parsePipeArguments(char *inputCopy, char ***commands) {
    char *token = strtok(inputCopy, PIPE_SEPARATOR);
    display_message("am here \n");
    int cmdIndex = INITIAL_VALUE;
    while (token != NULL) {
        commands[cmdIndex++] = token;
        token = strtok(NULL, PIPE_SEPARATOR);
    }
    commands[cmdIndex] = NULL;
}

int createPipe() {
    int pipeFDs[2];
    if (pipe(pipeFDs) == ELEMENT_NOT_FOUND) {
        perror(FAILED_TO_CREATE_PIPE);
        _exit(EXIT_FAILURE);
    }
    return pipeFDs[1];
}

void executeWithPipe(char **args, int inputFD, int outputFD) {
    if (inputFD != ELEMENT_NOT_FOUND) {
        if (dup2(inputFD, STDIN_FILENO) == ELEMENT_NOT_FOUND) {
            perror(FAILED_TO_REDIRECT);
            _exit(EXIT_FAILURE);
        }
    }

    if (outputFD != ELEMENT_NOT_FOUND) {
        if (dup2(outputFD, STDOUT_FILENO) == ELEMENT_NOT_FOUND) {
            perror(FAILED_TO_REDIRECT);
            _exit(EXIT_FAILURE);
        }
    }

    if (execvp(args[0], args) == ELEMENT_NOT_FOUND) {
        perror(EXECUTION_ERROR);
        _exit(EXIT_FAILURE);
    }
}

void handlePipeExecution(char **commands, int numCommands) {
    int pipeFDs[2];
    int inputFD = ELEMENT_NOT_FOUND;

    for (int i = INITIAL_VALUE; i < numCommands; i++) {
        char *args[BUFSIZE];
        parseArguments(commands[i], args, NULL, NULL, NULL, NULL);

        if (i < numCommands - 1) {
            if (pipe(pipeFDs) == ELEMENT_NOT_FOUND) {
                perror(FAILED_TO_CREATE_PIPE);
                _exit(EXIT_FAILURE);
            }
        }

        pid_t pid = fork();

        if (pid == CHILD_SELF_PID) {
            if (i > INITIAL_VALUE) {
                if (dup2(inputFD, STDIN_FILENO) == ELEMENT_NOT_FOUND) {
                    perror(FAILED_TO_REDIRECT);
                    _exit(EXIT_FAILURE);
                }
            }

            if (i < numCommands - 1) {
                if (dup2(pipeFDs[1], STDOUT_FILENO) == ELEMENT_NOT_FOUND) {
                    perror(FAILED_TO_REDIRECT);
                    _exit(EXIT_FAILURE);
                }
            }

            close(pipeFDs[0]);
            close(pipeFDs[1]);

            executeWithPipe(args, inputFD, pipeFDs[1]);
        } else if (pid > CHILD_SELF_PID) {
            if (inputFD != ELEMENT_NOT_FOUND) {
                close(inputFD);
            }
            if (i < numCommands - 1) {
                close(pipeFDs[1]);
            }

            inputFD = pipeFDs[0];

            waitpid(pid, NULL, 0);
        }
    }
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

    if (strchr(inputBuffer, PIPE_SEPARATOR) != NULL) {
        char *commands[BUFSIZE];
        parsePipeArguments(inputCopy, commands);
        int numCommands = 0;

        while (commands[numCommands] != NULL) {
            numCommands++;
        }

        handlePipeExecution(commands, numCommands);
    } else {
        parseArguments(inputCopy, args, &inputRedirected, &outputRedirected, &inputFile, &outputFile);
        pid_t pid = fork();
        if (pid == CHILD_SELF_PID) {
            handleChildProcess(args, inputRedirected, inputFile, outputRedirected, outputFile);
        } else if (pid > CHILD_SELF_PID) {
            handleParentProcess(pid, lastExitCode, &startTime, &endTime);
        }
    }

    free(inputCopy);
    return EXIT_SUCCESS;
}

