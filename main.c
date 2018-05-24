//
// Created by sean on 07/05/18.
//

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include "linenoise/linenoise.h"
#include "variables.h"
#include "redirection.h"
#include "defenitions.h"
#include "internalCmds.h"


char *target;

int externalCommands(char *_args[MAX_ARGS]);

int Debug(char *_toPrint, ...) {
#ifdef DEBUG
    printf("%s", _toPrint);
#endif
}

int externalCommands(char *_args[MAX_ARGS]) {

}

int main() {
    char *line, *token = NULL, *args[MAX_ARGS], *prompt = "eggshell v1.0->", buffer[MAX_BUFFFER];
    int tokenIndex;


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

    while ((line = linenoise(prompt)) != NULL) {
        int returnValue = checkForRedirection(line);
        switch (returnValue) {
            case 1:
                setOutRedirect(true);
                getTarget(line);
                break;
            case 2:
                setOutRedirect(true);
                setAppend(true);
                getTarget(line);
                break;
            case 0:
                //there is no redirection needed.
                break;
            case -1:
                setInRedirect(true);
                setInFile(true);
                break;
            case -2:
                setInRedirect(true);
                break;
            default:
                printf("The command has an error in it");
                break;
        }

        //TODO handle inRedirect
        token = strtok(line, " ");
        for (tokenIndex = 0; token != NULL && tokenIndex < (MAX_ARGS - 1); tokenIndex++) {
            printf("Token %d: %s\n", tokenIndex, token);
            args[tokenIndex] = token;
            token = strtok(NULL, " ");
        }
        args[tokenIndex] = NULL;

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

        // Free allocated memory
        free(line);

        //reset variables for redirection
        setAppend(false);
        setOutRedirect(false);
        setInRedirect(false);
        setInFile(false);
        for(int i = 0; i < strlen(target); i++){
            target[i] = '\0';
        }
    }
}
