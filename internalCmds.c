//
// Created by sean on 24/05/2018.
//

#include <errno.h>
#include <printf.h>
#include <memory.h>
#include <malloc.h>
#include <stdbool.h>
#include <unistd.h>
#include "internalCmds.h"
#include "variables.h"
#include "defenitions.h"
#include "redirection.h"

#define MAX_VAR_SIZE 255
#define MAX_VARS 255

char tempLine[255];

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

int internalPrint(char *_args[MAX_ARGS]) {
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
        if(getOutRedirect()){
            outputRedirection("The working directory was unable to be changed", NULL, 0, getAppend());
        }else {
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
    if (strstr(_args[0], "=")) {
        internalVars(_args);
    } else if (strstr(_args[0], "print")) {
        internalPrint(_args);
    } else if (strstr(_args[0], "exit")) {
        return -1;
    } else if (strstr(_args[0], "chdir")) {
        changeWorkingDir(_args[1]);
    } else if (strstr(_args[0], "source")) {
        scriptFile(_args[1]);
    } else if (strstr(_args[0], "all")) {
        int size = getVarsSize();
        VAR *vars = getVars();
        if(getOutRedirect()){
            VAR *varsForRedirect;
            for(int i = 0; i < size; i++){
                VAR forRedirect1;
                strcpy(forRedirect1.data, vars[i].name);
                VAR forRedirect2;
                strcpy(forRedirect2.data, vars[i].data);
                varsForRedirect = malloc(sizeof(VAR) * 2);
                varsForRedirect[0] = forRedirect1;
                varsForRedirect[1] = forRedirect2;
                outputRedirection("Variable %, data %\n", varsForRedirect, 1, getAppend());
            }
        }
        for (int i = 0; i < size; i++) {
            printf("Variable %s, data: %s\n", vars[i].name, vars[i].data);
        }
    } else {
        //externalCommands(_args);
    }

    return 0;
}