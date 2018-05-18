//
// Created by sean on 07/05/18.
//

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "linenoise/linenoise.h"
#include "variables.h"

#define MAX_ARGS 255
//#define DEBUG


int Debug(char *_toPrint, ...) {
#ifdef DEBUG
    printf("%s", _toPrint);
#endif
}

int internalVars(char **_args, int MAX_VAR_SIZE) {
    int counter = 0;
    for (int i = 0; i < sizeof(_args[0]); i++) {
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
            if (k < sizeof(_args[0])) {
                Debug("DEBUG: %c != %c\n", _args[0][i], '=');
                k++;
            }
            //strncpy(&name[i], &temp[i], 1);
            var1name[i] = _args[0][i];
            i++;
        }
        var1name[i + 1] = '\0';
        i++;
        for (int j = 0; i < sizeof(_args[0]); j++) {
            var2name[j] = _args[0][i];
            i++;
        }
        var2name[i] = '\0';

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
                Debug("DEBUG: %c != %c\n", temp[i], '=');
                k++;
            }
            //strncpy(&name[i], &temp[i], 1);
            name[i] = temp[i];
            i++;
        }
        name[i + 1] = '\0';
        printf("DEBUG: name ends at %d\n", i);

        i++;

        for (int j = 0; i < sizeof(temp); j++) {
            data[j] = temp[i];
            i++;
        }
        data[i] = '\0';
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

int internalPrint(char *_args[MAX_ARGS], int MAX_VAR_SIZE) {
    int i = 1;
    if(strstr(_args[i], "\"")) {
        bool firstTime = true;
        bool found = false;
        for (i = 1; i < MAX_ARGS && _args[i] != NULL && !found; i++) {
            char *temp = _args[i];
            if (strstr(_args[i], "\"")) {
                if(firstTime != true) {
                    printf(" ");
                    for (int j = 0; j < sizeof(temp); j++) {
                        if (temp[j] == 34) {
                            j = sizeof(temp) + 1;
                            found = true;
                        } else {
                            printf("%c", temp[j]);
                        }
                    }
                }
                else {
                    int x = 0;;
                    if (firstTime) {
                        firstTime = false;
                        x = 1;
                    }
                    //no idea why a 1 has to be here.
                    for (int j = x; j < sizeof(temp) - x - 1; j++) {
                        printf("%c", temp[j]);
                    }
                    printf(" ");
                }
            }
            else{
                printf("%s", temp);
            }
        }
        printf("\n");
    }else {
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

int internalCommands(char *_args[MAX_ARGS], int MAX_VAR_SIZE) {
    if (strstr(_args[0], "=")) {
        internalVars(_args, MAX_VAR_SIZE);
    }
    if (strstr(_args[0], "print")) {
        internalPrint(_args, MAX_VAR_SIZE);
    }

    if (strstr(_args[0], "exit")) {
        return -1;
    }

    if (strstr(_args[0], "all")) {
        int size = getVarsSize();
        VAR *vars = getVars();
        for (int i = 0; i < size; i++) {
            printf("Variable %s, data: %s\n", vars[i].name, vars[i].data);
        }
    }
}

int main() {
    const int MAX_VAR_SIZE = 255;
    char *line,
            *token = NULL,
            *args[MAX_ARGS],
            *prompt = "eggshell v1.0->";
    int tokenIndex;

    createVariable("PATH", getenv("PATH"));
    createVariable("USER", getenv("USER"));
    createVariable("CWD", getenv("PWD"));
    createVariable("HOME", getenv("HOME"));
    char shell[1024];
    realpath("main.c", shell);
    createVariable("SHELL", shell);
    createVariable("TERMINAL", ttyname(STDIN_FILENO));
    createVariable("EXITCODE", "0");

    VAR path;
    path = findVariable("PATH");
    if (path.data != NULL) {
        printf("%s\n", path.data);
    } else {
        printf("Could not be found\n");
    }
    while ((line = linenoise(prompt)) != NULL) {
        token = strtok(line, " ");

        for (tokenIndex = 0; token != NULL && tokenIndex < (MAX_ARGS - 1); tokenIndex++) {
            args[tokenIndex] = token;
            token = strtok(NULL, " ");
        }

        int returnCode = internalCommands(args, MAX_VAR_SIZE);
        if (returnCode == -1) {
            break;
        } else {
            //TODO fix exit codes they are currently broken
            Debug("DEBUG: changing exitcode\n");
            size_t length = (size_t) snprintf(NULL, 0, "%d", returnCode);
            char *src = malloc(sizeof(char) * (length + 1));
            snprintf(src, length + 1, "%d", returnCode);
            editVariable("EXITCODE", src);
        }
        // set last token to NULL
        args[tokenIndex] = NULL;

        // Free allocated memory
        linenoiseFree(line);
    }
}