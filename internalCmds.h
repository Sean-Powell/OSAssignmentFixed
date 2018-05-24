//
// Created by sean on 24/05/2018.
//

#ifndef OSASSINGMENT_INTERNALCMDS_H
#define OSASSINGMENT_INTERNALCMDS_H

#include "defenitions.h"

int internalCommands(char *_args[MAX_ARGS]);
int internalVars(char **_args);
int internalPrint(char *_args[MAX_ARGS]);
int changeWorkingDir(char *dir);
int scriptFile(char *path);

#endif //OSASSINGMENT_INTERNALCMDS_H
