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
#define DEBUG

int printFunction(char **args) {
    if (strstr(args[1], "\"")) {
        printf("Contains \"\n");
        bool firstTime = true;
        bool found = false;
        for (int i = 1; i < MAX_ARGS && args[i] != NULL && !found; i++) {
            char *temp = args[i];
            if (strstr(args[i], "\"") && firstTime != true) {
                printf(" ");
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
                }else{
                    printf(" ");
                }
                //no idea why a 1 has to be here.
                for (int j = x; j < sizeof(temp) - x - 1; j++) {
                    printf("%c", temp[j]);
                }
            }
        }
        printf("\n");
    } else {
        printf("%s\n", args[1]);
    }
    return 0;
}

int internalCommands(char* _args[MAX_ARGS], int MAX_VAR_SIZE){
    if (strstr(_args[0], "print")) {
        if(strstr(_args[1], "$")){//checks to see if there is a '$' as this indicates a variable
            VAR *tempVars = getVars();
            for(int i = 0; i < getVarsSize(); i++){
                if(strstr(_args[1], tempVars[i].name)){
                    printf("Variable %s, data: %s\n", tempVars[i].name, tempVars[i].data);
                }
            }
        }else {
            printFunction(_args);
        }
    }

    if (strstr(_args[0], "exit")) {
        return -1;
    }

    if(strstr(_args[0], "all")){
        int size = getVarsSize();
        VAR *vars = getVars();
        for(int i = 0; i < size; i++){
            printf("Variable %s, data: %s\n", vars[i].name, vars[i].data);
        }
    }

    if (strstr(_args[0], "=")) {
#ifdef DEBUG
        printf("DEBUG: contains =\n");
#endif
        char name[MAX_VAR_SIZE];
        char data[MAX_VAR_SIZE];

        char *temp = _args[0];
        int i = 0, k = 0;

        while (temp[i] != '=') {
            if (k < sizeof(temp)) {
#ifdef DEBUG
                printf("DEBUG: %c != %c\n", temp[i], '=');
#endif
                k++;
            }
            //strncpy(&name[i], &temp[i],1);
            name[i] = temp[i];
            i++;
        }
        name[i + 1] = '\0';
#ifdef DEBUG
        printf("DEBUG: name ends at %d\n", i);
#endif

        i++;

        for (int j = 0; i < sizeof(temp); j++) {
            data[j] = temp[i];
            i++;
        }
        data[i] = '\0';
#ifdef DEBUG
        printf("DEBUG: data ends at %d\n", i);
        printf("DEBUG: creating variable name: %s, data: %s\n", name, data);
#endif
        VAR*vars = getVars();
        int varSize = getVarsSize();
        bool found = false;
        for(int j = 0; j < varSize && !found; j++){
            if(strstr(vars[j].name, name)){
                found = true;
               editVariable(name, data);
           }
        }
        if(!found) {
            createVariable(name, data);
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
        if(returnCode == -1){
            break;
        }else{
#ifdef DEBUG
            printf("DEBUG: changing exitcode\n");
#endif
            size_t length = (size_t) snprintf(NULL, 0, "%d", returnCode);
            char *src = malloc(sizeof(char) * (length + 1));
            snprintf(src, length + 1, "%d", returnCode);
            editVariable("EXITCODE",  src);
        }
        // set last token to NULL
        args[tokenIndex] = NULL;

        // Free allocated memory
        linenoiseFree(line);
    }
}