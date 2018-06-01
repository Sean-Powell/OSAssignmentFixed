//
// Created by sean on 07/05/18.
//

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>
#include "linenoise/linenoise.h"
#include "variables.h"
#include "redirection.h"
#include "defenitions.h"
#include "Signals.h"

bool append = false;
bool inFile = false;
bool outRedirect = false;
bool inRedirect = false;
bool exitLoop = false;

char tempLine[255];
int returnedValue;

int externalCommands(char *_args[MAX_ARGS]);

int internalCommands(char *_args[MAX_ARGS]);


FILE *openFile(char *_target) {
    FILE *file;
    if (!append) {
        file = fopen(_target, "w");
        fclose(file);
    }
    file = fopen(_target, "a+");
    return file;
}

int internalVars(char **_args) {
    int counter = 0;
    for (int i = 0; i < strlen(_args[0]); i++) {
        if (_args[0][i] == '$') {
            counter++;
        }
    }

    if (counter == 2) {
        //todo set var to a var
        char var1name[MAX_VAR_SIZE];
        char var2name[MAX_VAR_SIZE];
        int i = 0, k = 0;
        while (_args[0][i] != '=') {
            if (k < strlen(_args[0])) {
                k++;
            }
            var1name[i] = _args[0][i];
            i++;
        }
        var1name[i] = '\0';
        i++;
        for (int j = 0; i < strlen(_args[0]); j++) {
            var2name[j] = _args[0][i];
            i++;
        }
        var2name[i - strlen(var1name) - 1] = '\0';

        int returned = setVarToVar(var1name, var2name);
        if (returned == 0) {
            printf("Var %s was set to %s\n", var1name, var2name);
        } else if (returned == -1) {
            printf("ERROR: var %s could not be found\n", var1name);
        } else {
            printf("ERROR: var %s could not be found\n", var2name);
        }
    } else {
        char name[MAX_VAR_SIZE];
        char data[MAX_VAR_SIZE];

        char *temp = _args[0];
        int i = 0, k = 0;

        while (temp[i] != '=') {
            if (k < sizeof(temp)) {
                k++;
            }
            name[i] = temp[i];
            i++;
        }
        name[i] = '\0';
        i++;

        for (int j = 0; i < strlen(temp); j++) {
            data[j] = temp[i];
            i++;
        }
        data[i - strlen(name) - 1] = '\0';
        VAR *vars = getVars();
        int varSize = getVarsSize();
        bool found = false;
        for (int j = 0; j < varSize && !found; j++) {
            if (strstr(vars[j].name, name)) {
                found = true;
                editVariable(name, data);
            }
        }
        if (!found) {
            returnedValue = createVariable(name, data);
            if(returnedValue == 0){
                printf("Variable created successfully\n");
            }else{
                printf("An error occurred during the creation of the variable %s\n", name);
            }
        }
    }
    return 0;
}

int internalPrint(char *_args[MAX_ARGS]) { //todo find the segfault in the program
    int i = 1;
    if (strstr(_args[i], "\"")) {
        bool firstTime = true;
        bool found = false;
        for (i = 1; i < MAX_ARGS && _args[i] != NULL && !found; i++) {
            char *temp = _args[i];
            if (strstr(_args[i], "\"")) {
                if (firstTime != true) {
                    for (int j = 0; j < sizeof(temp); j++) {
                        if (temp[j] == 34) {
                            j = sizeof(temp) + 1;
                            found = true;
                        } else {
                            printf("%c", temp[j]);
                        }
                    }
                } else {
                    int x = 0;;
                    if (firstTime) {
                        firstTime = false;
                        x = 1;
                    }
                    //no idea why a 2 has to be here.
                    for (int j = x; j < sizeof(temp) - x - 2; j++) {
                        printf("%c", temp[j]);
                    }

                    printf(" ");
                }
            } else {
                    printf("%s", temp);
                    printf(" ");
            }
        }
        printf("\n");
    } else {
        while (_args[i] != NULL) {
            if (strstr(_args[i], "$")) {
                char *data = getVarData(_args[i]);
                if (data != NULL) {
                    printf("%s ", data);
                } else {
                    printf("\n ERROR: var %s could not be found\n", _args[i]);
                    return -1;
                }
            } else {
                printf("%s ", _args[i]);
            }
            i++;
        }
        printf("\n");
        return 0;
    }
}

int changeWorkingDir(char *_dir) {
    //char newDir[512];
    //snprintf(newDir, sizeof(newDir), dir);
    int returned = chdir(_dir);
    char newCWD[512];
    getcwd(newCWD, sizeof(newCWD));
    editVariable("$CWD", newCWD);
    if (returned == -1) {
        printf("The working directory was unable to be changed");
        return -1;
    } else {
        return 0;
    }
}

int scriptFile(char *path) {
    //Todo find and fix the bug that causes a leak from first previous lines arguments to current lines
    printf("Path is: %s\n", path);
    FILE *file = NULL;
    file = fopen(path, "r");
    printf("file path set\n");
    char lineSplit[MAX_ARGS][MAX_VAR_SIZE];

    char *args[MAX_ARGS];
    printf("Line split made\n");
    if (file) {
        printf("File exists\n");
        bool endOfLine;
        int counter = 0;
        char token[255];
        int loopCounter = 0;
        while (fgets(tempLine, sizeof(tempLine), file) != NULL) {
            endOfLine = false;
            counter = 0;
            printf("Loop %d\n", loopCounter);
            loopCounter++;
            printf("Line is: %s\n", tempLine);
            int index = 0, indexTemp = 0, nextTokenBeginsAt = 0;
            while (!endOfLine) {
                if (tempLine[indexTemp] != '\0' && tempLine[indexTemp] != '\n') {
                    if (tempLine[indexTemp] == 32) {
                        nextTokenBeginsAt = indexTemp + 1;
                        token[index] = '\0';
                        for (int i = 0; i < index; i++) {
                            lineSplit[counter][i] = token[i];
                        }
                        //indexTemp++;
                        lineSplit[counter][indexTemp + 1] = 32;
                        for (int i = 0; i < sizeof(token); i++) {
                            if (token[i] == '\0') {
                                i = sizeof(token);
                            } else {
                                token[i] = '\0';
                            }
                        }
                        counter++;
                        indexTemp++;
                        index = 0;
                    } else {
                        token[index] = tempLine[indexTemp];
                        index++;
                        indexTemp++;
                    }
                } else {
                    int j = 0;
                    for (int i = nextTokenBeginsAt; i < indexTemp; i++, j++) {
                        lineSplit[counter][j] = token[j];
                    }
                    printf("End of line found at index: %d\n", indexTemp);
                    endOfLine = true;
                    counter++;
                }
            }

            for (int i = 0; i < counter; i++) {
                args[i] = malloc(sizeof(lineSplit[i]));
                strcpy(args[i], lineSplit[i]);
            }

            printf("counter: %d\n", counter);

            for (int i = 0; i < counter; i++) {
                printf("Args %d: %s\n", i, args[i]);
            }

            internalCommands(args);

            for (int i = 0; i < MAX_ARGS; i++) {
                args[i] = realloc(args[i], 0);
            }

            for (int i = 0; i < counter; i++) {
                for (int j = 0; j < strlen(lineSplit[i]); j++) {
                    lineSplit[i][j] = '\0';
                }
            }
        }
    } else {
        if (errno == 2) {
            printf("The file located at %s could not be located\n", path);
        } else {
            printf("Opening file failed with error num: %d\n", errno);
        }
    }

    return 0;
}

int internalCommands(char *_args[MAX_ARGS]) {
    if (strstr(_args[0], "=")) {
        internalVars(_args);
    } else if (strstr(_args[0], "print")) {
        internalPrint(_args);
    } else if (strstr(_args[0], "exit")) {
        exitLoop = true;
        return -1;
    } else if (strstr(_args[0], "chdir")) {
        changeWorkingDir(_args[1]);
    } else if (strstr(_args[0], "source")) {
        scriptFile(_args[1]);
    } else if (strstr(_args[0], "all")) {
        int size = getVarsSize();
        VAR *vars = getVars();

        for (int i = 0; i < size; i++) {
            printf("Variable %s, data: %s\n", vars[i].name, vars[i].data);
        }
    } else {
        externalCommands(_args);
    }

    return 0;
}

int externalCommands(char *_args[MAX_ARGS]) {
    char *path = getVarData("$PATH");;
    char *paths[MAX_ARGS]; // unlikely to be more that 255 paths
    char *token;
    token = strtok(path, ":");
    int tokenIndex, returnValue, status;
    for (tokenIndex = 0; token != NULL && tokenIndex < (MAX_ARGS - 1); tokenIndex++) {
        paths[tokenIndex] = malloc((strlen(token) + 1) * sizeof(char));
        strcpy(paths[tokenIndex], token);
        token = strtok(NULL, ":");
    }

    pid_t pid = fork();
    printf("parent pid is %d, and child pid is: %d\n", getppid(), pid);
    pushToStack(pid);
    if (pid == 0) {//child process
        printf("Result of running %s:\n", _args[0]);
        int i;
        for (i = 0; i < tokenIndex && i != -1; i++) {
            char *command = malloc((strlen(_args[0]) + strlen(paths[i]) + 1) * sizeof(char));
            strcat(command, paths[i]);
            strcat(command, "/");
            strcat(command, _args[0]);
            returnValue = execv(command, _args);
            if (returnValue != -1) {
                i = -1;
            }
            free(command);
        }
        if (i != -1) {
            printf("The command could not be run\n");
        }
    } else {//otherwise waits for the child
        wait(&status);
    }
}

struct sigaction sigHandler;

int main() {
    printf("new version");
    char *line, *token = NULL, *args[MAX_ARGS], *prompt = "eggshell v1.0->", buffer[MAX_BUFFFER];
    int tokenIndex;

    memset(&sigHandler, 0, sizeof(sigHandler));
    sigHandler.sa_handler = signalManager;
    sigaction(SIGINT, &sigHandler, NULL);
    sigaction(SIGCONT, &sigHandler, NULL);
    sigaction(SIGTSTP, &sigHandler, NULL);

    createVariable("$PATH", getenv("PATH"));
    createVariable("$USER", getenv("USER"));
    createVariable("$CWD", getenv("PWD"));
    createVariable("$HOME", getenv("HOME"));
    char shell[1024];
    realpath("main.c", shell);
    createVariable("$SHELL", shell);
    createVariable("$TERMINAL", ttyname(STDIN_FILENO));
    createVariable("$EXITCODE", "0");

    while (!exitLoop) {
        while ((line = linenoise(prompt)) != NULL) {
            if(strlen(line) > 0) {
                printf("Checking for redirection\n");
                int returnValue = checkForRedirection(line);
                printf("Redirection returned: %d\n", returnValue);
                //TODO handle inRedirect
                token = strtok(line, " ");
                for (tokenIndex = 0; token != NULL && tokenIndex < (MAX_ARGS - 1); tokenIndex++) {
                    printf("Token %d: %s\n", tokenIndex, token);
                    args[tokenIndex] = token;
                    token = strtok(NULL, " ");
                }
                args[tokenIndex] = NULL;
                printf("All tokens found:\n");
                switch (returnValue) {
                    case 1:
                        outRedirect = true;
                        for (int i = 0; i < tokenIndex; i++) {
                            if (strstr(args[i], ">")) {
                                startOutputRedirection(args[i+1], append);
                            }
                        }
                        break;
                    case 2:
                        outRedirect = true;
                        append = true;
                        for (int i = 0; i < tokenIndex; i++) {
                            if (strstr(args[i], ">>")) {
                                startOutputRedirection(args[i+1], append);
                            }
                        }

                        break;
                    case 0:
                        //there is no redirection needed.
                        break;
                    case -1:
                        inRedirect = true;
                        inFile = true;
                        break;
                    case -2:
                        inRedirect = true;
                        break;
                    default:
                        printf("The command has an error in it");
                        break;
                }
                returnValue = internalCommands(args);
                if (returnValue == -1) {
                    break;
                } else {
                    //TODO fix exit codes they are currently broken
                    size_t length = (size_t) snprintf(NULL, 0, "%d", returnValue);
                    char *src = malloc(sizeof(char) * (length + 1));
                    snprintf(src, length + 1, "%d", returnValue);
                    editVariable("EXITCODE", src);
                }

                // Free allocated memory
                free(line);

                //reset variables for redirection
                append = false;
                if(outRedirect){
                    stopOutputRedirection();
                }
                outRedirect = false;
                inRedirect = false;
                inFile = false;
            }
        }
    }
}