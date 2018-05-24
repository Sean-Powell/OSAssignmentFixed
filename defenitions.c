//
// Created by sean on 24/05/2018.
//


#include <stdbool.h>

bool inRedirect = false, outRedirect = false, append = false, inFile = false;

bool getInRedirect(){
    return inRedirect;
}

bool getOutRedirect(){
    return outRedirect;
}

bool getAppend(){
    return append;
}

bool getInFile(){
    return inFile;
}

void setInRedirect(bool _new){
    inRedirect = _new;
}

void setOutRedirect(bool _new){
    outRedirect = _new;
}

void setInFile(bool _new){
    inFile = _new;
}

void setAppend(bool _new){
    append = _new;
}


