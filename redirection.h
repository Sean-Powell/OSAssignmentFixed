//
// Created by sean on 24/05/2018.
//

#ifndef OSASSINGMENT_REDIRECTION_H
#define OSASSINGMENT_REDIRECTION_H

#include <stdbool.h>
#include <stdio.h>
#include "variables.h"

int outputRedirection(char* _string, VAR* _vars, int _numOfVars, bool _append, FILE* file);
int checkForRedirection(const char* _input);
int setTarget(char* _target);
char* getTarget();

#endif //OSASSINGMENT_REDIRECTION_H
