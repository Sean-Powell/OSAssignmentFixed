//
// Created by sean on 24/05/2018.
//

#ifndef OSASSINGMENT_DEFENITIONS_H
#define OSASSINGMENT_DEFENITIONS_H
#define MAX_ARGS 255
#define MAX_BUFFFER 255
#define MAX_VAR_SIZE 255

#include <stdbool.h>

bool inRedirect = false;
bool outRedirect = false;
bool append = false;
bool inFile = false;

bool getInRedirect();
bool getOutRedirect();
bool getAppend();
bool getInFile();
void setInRedirect(bool _new);
void setOutRedirect(bool _new);
void setInFile(bool _new);
void setAppend(bool _new);

#endif //OSASSINGMENT_DEFENITIONS_H
