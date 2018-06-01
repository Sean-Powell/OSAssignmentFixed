//
// Created by sean on 24/05/2018.
//

#ifndef OSASSINGMENT_REDIRECTION_H
#define OSASSINGMENT_REDIRECTION_H

#include <stdbool.h>
#include <stdio.h>
#include "variables.h"
int startOutputRedirection(char* _fileLocation, bool _append);
int stopOutputRedirection();
int checkForRedirection(const char* _input);



#endif //OSASSINGMENT_REDIRECTION_H
