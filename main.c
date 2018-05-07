//
// Created by sean on 07/05/18.
//

#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linenoise/linenoise.h"

#define MAX_ARGS 255
typedef struct variable {
    char *data;
    char *name;
} VAR;

VAR *vars;
int varsSize = 0;

int printFunction(char *_line, char **args) {
    //TODO find the entire print statement if it is in ""
    printf("%s\n", "DEBUG: starting");
    char *toPrint;
    if (strstr(args[1], "\"")) {
        printf("Contains \"");
        toPrint = malloc(0);
        bool firstTime = true;
        bool found = false;
        for (int i = 1; i < MAX_ARGS && args[i] != NULL && !found; i++) {
            printf("Finding end block, current block is: %s\n", args[i]);
            size_t  size = sizeof(toPrint);
            int k = 0;
            char *temp = args[i];

            if (strstr(args[i], "\"") && firstTime != true) {
                printf("End block is: %d\n", i);
                found = true;
                int offset = 0;
                toPrint = realloc(toPrint, sizeof(toPrint) + sizeof(args[i]) - 1);
                size_t newSize = sizeof(toPrint);

                for(int j = (int) size; j < (int) newSize - 1; j++){
                    if(temp[k] == '\"'){
                        offset = 1;
                    }else{
                        toPrint[j] = temp[k + offset];
                        k++;
                    }
                }

            } else {
                printf("End block is not: %d\n", i);
                if(firstTime){
                    firstTime = false;
                    toPrint = realloc(toPrint, sizeof(toPrint) + sizeof(args[i]) - 1);
                    for(int j = 1; j < sizeof(args[i]); j++){
                        toPrint[j - 1] = temp[j];
                    }
                }else {
                    toPrint = realloc(toPrint, sizeof(toPrint) + sizeof(args[i]));
                    size_t newSize = sizeof(toPrint);
                    for (int j = (int) size; j < (int) newSize; j++) {
                        toPrint[j] = temp[k];
                        k++;
                    }
                }
            }
            printf("toPrint currently: %s\n", toPrint);
        }

        toPrint[sizeof(toPrint) - 1] = '\0';

        printf("%s\n", toPrint);
        free(toPrint);


    } else {
        char *token = strtok(_line, " ");
        printf("%s\n", "DEBUG: does not contain \"");
        printf("%s\n", args[1]);
    }
    return 0;
}

int createVariable(char *_name, char *_data) {
    varsSize++;
    vars = realloc(vars, sizeof(VAR) * varsSize);
    VAR v1;
    v1.data = _data;
    v1.name = _name;
    vars[varsSize - 1] = v1;
}

VAR findVariable(char *_name) {
    for (int i = 0; i < varsSize; i++) {
        if (strcmp(vars[i].name, _name) == 0) {
            return vars[i];
        }
    }
    VAR var;
    var.name = NULL;
    var.data = NULL;
    return var;
}

int main() {
    const int MAX_VAR_SIZE = 255;
    char *line,
            *token = NULL,
            *args[MAX_ARGS],
            *prompt = "eggshell v1.0->";
    int tokenIndex;

    createVariable("PATH", getenv("PATH"));

    VAR path = findVariable("PATH");
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

        if (strstr(args[0], "print")) {
            printFunction(line, args);
        }

        if (strstr(args[0], "exit")) {
            break;
        }

        if (strstr(args[0], "=")) {
            printf("DEBUG: contains =\n");
            char name[MAX_VAR_SIZE];
            char data[MAX_VAR_SIZE];

            char *temp = args[0];
            int i = 0, k = 0;

            while (temp[i] != '=') {
                if (k < sizeof(temp)) {
                    printf("DEBUG: %c != %c\n", temp[i], '=');
                    k++;
                }
                name[i] = temp[i];
                i++;
            }
            printf("DEBUG: name ends at %d\n", i);

            i++;

            for (int j = 0; i < sizeof(temp); j++) {
                data[j] = temp[i];
                i++;
            }
            printf("DEBUG: data ends at %d\n", i);

            printf("DEBUG: creating variable name: %s, data: %s\n", name, data);

            createVariable(name, data);
        }

        // set last token to NULL
        args[tokenIndex] = NULL;

        // Free allocated memory
        linenoiseFree(line);
    }
}