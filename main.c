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

int Debug(char *_toPrint, ...) {
#ifdef DEBUG
    printf("%s", _toPrint);
#endif
}

int internalVars(char **_args) {
    int counter = 0;
    for (int i = 0; i < strlen(_args[0]); i++) {
        if (_args[0][i] == '$') {
            counter++;
        }
    }

    printf("Number of vars: %d\n", counter);
    if (counter == 2) {
        //todo set var to a var
        char var1name[MAX_VAR_SIZE];
        char var2name[MAX_VAR_SIZE];
        int i = 0, k = 0;
        while (_args[0][i] != '=') {
            if (k < strlen(_args[0])) {
                printf("DEBUG: %c != %c\n", _args[0][i], '=');
                k++;
            }
            //strncpy(&name[i], &temp[i], 1);
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

        int returned = setVartoVar(var1name, var2name);
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
                printf("DEBUG: %c != %c\n", temp[i], '=');
                k++;
            }
            //strncpy(&name[i], &temp[i], 1);
            name[i] = temp[i];
            i++;
        }
        name[i] = '\0';
        printf("DEBUG: name ends at %d\n", i);

        i++;

        for (int j = 0; i < strlen(temp); j++) {
            data[j] = temp[i];
            i++;
        }
        data[i - strlen(name) - 1] = '\0';
        printf("DEBUG: data ends at %d\n", i);
        printf("DEBUG: creating variable name: %s, data: %s\n", name, data);
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
            createVariable(name, data);
        }
    }
    return 0;
}

int internalPrint(char *_args[MAX_ARGS]) { //todo find the segfault in the program
    FILE *file = NULL;
    int i = 1;
    if (outRedirect) {
        file = openFile(getTarget());
    }

    printf("DEBUG: FILE CREATED\n");
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
                            if (outRedirect) {
                                VAR var;
                                var.data = malloc(sizeof(char));
                                var.data[0] = temp[j];
                                VAR *vars;
                                vars = malloc(sizeof(VAR));
                                vars[0] = var;
                                outputRedirection("%", vars, 1, append, file);
                                free(var.data);
                                free(vars);
                            } else {
                                printf("%c", temp[j]);
                            }
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
                        if (outRedirect) {
                            VAR var;
                            var.data = malloc(sizeof(char));
                            var.data[0] = temp[j];
                            VAR *vars;
                            vars = malloc(sizeof(VAR));
                            vars[0] = var;
                            outputRedirection("%", vars, 1, append, file);
                            free(var.data);
                            free(vars);
                        } else {
                            printf("%c", temp[j]);
                        }
                    }
                    if (outRedirect) {
                        outputRedirection(" ", NULL, 0, append, file);
                    } else {
                        printf(" ");
                    }
                }
            } else {
                if (outRedirect) {
                    VAR var;
                    var.data = malloc(strlen(temp) * sizeof(char));
                    strcpy(var.data, temp);
                    VAR *vars;
                    vars = malloc(sizeof(VAR));
                    vars[0] = var;
                    outputRedirection("% ", vars, 1, append, file);
                    free(var.data);
                    free(vars);
                } else {
                    printf("%s", temp);
                    printf(" ");
                }
            }
        }
        if (outRedirect) {
            outputRedirection("\n", NULL, 0, append, file);
        } else {
            printf("\n");
        }
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
        if (outRedirect) {
            FILE *file = openFile(getTarget());
            outputRedirection("The working directory was unable to be changed", NULL, 0, append, file);
            fclose(file);
        } else {
            printf("The working directory was unable to be changed");
        }
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
}

int internalCommands(char *_args[MAX_ARGS]) {
    printf("Internal Commands check:\n");
    if (strstr(_args[0], "=")) {
        printf("= located:\n");
        internalVars(_args);
    } else if (strstr(_args[0], "print")) {
        printf("print:\n");
        internalPrint(_args);
    } else if (strstr(_args[0], "exit")) {
        printf("exit:\n");
        exitLoop = true;
        return -1;
    } else if (strstr(_args[0], "chdir")) {
        printf("chdir:\n");
        changeWorkingDir(_args[1]);
    } else if (strstr(_args[0], "source")) {
        printf("Source:\n");
        scriptFile(_args[1]);
    } else if (strstr(_args[0], "all")) {
        printf("All:\n");
        int size = getVarsSize();
        VAR *vars = getVars();
        if (outRedirect) {
            FILE *file = openFile(getTarget());
            printf("%p\n", file);
            printf("Starting redirect:\n");
            VAR *varsForRedirect;
            for (int i = 0; i < size; i++) {
                printf("Loop %d:\n", i);
                VAR forRedirect1;
                forRedirect1.data = malloc(strlen(vars[i].name) * sizeof(char));
                strcpy(forRedirect1.data, vars[i].name);
                VAR forRedirect2;
                forRedirect2.data = malloc(strlen(vars[i].data) * sizeof(char));
                strcpy(forRedirect2.data, vars[i].data);
                printf("VAR variables created\n");
                varsForRedirect = malloc(sizeof(VAR) * 2);
                printf("Var array created\n");
                varsForRedirect[0] = forRedirect1;
                varsForRedirect[1] = forRedirect2;
                printf("Vars added to the array\n");
                outputRedirection("Variable %, data %\n", varsForRedirect, 2, append, file);
                free(varsForRedirect);
                free(forRedirect2.data);
                free(forRedirect1.data);
            }
            fclose(file);
        } else {
            for (int i = 0; i < size; i++) {
                printf("Variable %s, data: %s\n", vars[i].name, vars[i].data);
            }
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
    pid_t tempPid;
    while((tempPid = popFromStack()) != pid);
    pushToStack(tempPid);
}

struct sigaction sigHandler;

int main() {
    char *line, *token = NULL, *args[MAX_ARGS], *prompt = "eggshell v1.0->", buffer[MAX_BUFFFER];
    int tokenIndex;
    memset(&sigHandler, 0, sizeof(sigHandler));
    sigHandler.sa_handler = signalManager;
    sigaction(SIGINT, &sigHandler, NULL);
    sigaction(SIGCONT, &sigHandler, NULL);
    sigaction(SIGTSTP, &sigHandler, NULL);

    //TODO make all internal command check for redirection first
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
                            setTarget(args[i + 1]);
                            printf("Target is: %s\n", args[i + 1]);
                        } else {
                            printf("%s is not the target\n", args[i]);
                        }
                    }
                    break;
                case 2:
                    outRedirect = true;
                    append = true;
                    for (int i = 0; i < tokenIndex; i++) {
                        if (strstr(args[i], ">>")) {
                            printf("Target is: %s\n", args[i + 1]);
                            setTarget(args[i + 1]);
                        } else {
                            printf("%s is not the target\n", args[i]);
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
            printf("Booleans set: inFile: %d, inRedirect: %d, append: %d, outRedirect: %d\n", inFile, inRedirect,
                   append,
                   outRedirect);
            returnValue = internalCommands(args);
            if (returnValue == -1) {
                break;
            } else {
                //TODO fix exit codes they are currently broken
                Debug("DEBUG: changing exitcode\n");
                size_t length = (size_t) snprintf(NULL, 0, "%d", returnValue);
                char *src = malloc(sizeof(char) * (length + 1));
                snprintf(src, length + 1, "%d", returnValue);
                editVariable("EXITCODE", src);
            }

            printf("freeing line\n");
            // Free allocated memory
            free(line);

            //reset variables for redirection
            printf("Resetting variables back to false:\n");
            append = false;
            outRedirect = false;
            inRedirect = false;
            inFile = false;
        }
    }
}