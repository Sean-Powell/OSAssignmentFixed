//
// Created by sean on 24/05/2018.
//

#include "redirection.h"

#include <malloc.h>
#include <memory.h>
#include <stdbool.h>
#include "variables.h"

char* targetForRedirection;

int outputRedirection(char *_string, VAR *_vars, int _numOfVars, bool _append) {
    char *output;
    size_t size = strlen(_string);
    int offset = 0;
    int i = 0;
    for(i = 0; i < _numOfVars; i++){
        size+=strlen(_vars[i].data);
    }
    i = 0;
    if (_numOfVars > 0) {
        output = malloc(size * sizeof(char) + 1);
        int j = 0;
        while (i < strlen(_string)) {
            if (_string[i] == '%') {
                if (j < _numOfVars) {
                    int k = 0;
                    while (k < strlen(_vars[j].data)) {
                        if (_vars[j].data[k] != '\0') {
                            output[i + offset] = _vars[j].data[k];
                            k++;
                            offset++;
                        }
                    }
                    j++;
                    offset--;
                } else {
                    return -2;
                }
            } else {
                if (_string[i] != '\0')
                    output[i + offset] = _string[i];
            }
            i++;
        }
    } else {
        output = _string;
    }
    FILE *file;
    if(_append) {
        file = fopen(targetForRedirection, "a+");
    }else{
        file = fopen(targetForRedirection, "w");
    }
    output[i + offset]='\0';
    fwrite(output, sizeof(char), strlen(output), file);
    fclose(file);
    return 0;
}

int checkForRedirection(const char* _input){
    int numOfForward = 0;
    int numOfBackwards = 0;
    int i = 0;
    while(_input[i] != '\0'){
        if(_input[i] == '>'){
            numOfForward++;
        }else if(_input[i] == '<'){
            numOfBackwards++;
        }
    }

    if(numOfBackwards == 1 && numOfForward == 0){//input from file
        return -1;
    }else if(numOfBackwards == 3 && numOfForward == 0){//input string
        return -2;
    }else if(numOfBackwards == 0 && numOfForward == 0){ //run normally
        return 0;
    }else if(numOfBackwards == 0 && numOfForward == 1){ // output
        return 1;
    }else if(numOfBackwards == 0 && numOfForward == 2){ //output appends
        return 2;
    }else{//ERROR
        return -3;
    }
}

int getTarget(char* _target){
    int indexStart = -1;
    for(int i = 0; i < strlen(_target); i++){
        if(_target[i] == '>'){
            if(_target[i + 1] == '>') {
                indexStart = i + 2;
            }else{
                indexStart = i;
            }
            i = (int) strlen(_target);
        }else if(_target[i] == '<'){
            if(_target[i + 2] == '<'){
                indexStart = i + 3;
            }else{
                indexStart = i;
            }
            i = (int) strlen(_target);
        }
    }

    targetForRedirection = malloc((strlen(_target) - indexStart + 1) * sizeof(char));
    for(int i = indexStart; i < strlen(_target); i++){
        targetForRedirection[i] = _target[i + indexStart];
    }

    targetForRedirection[strlen(_target) - indexStart + 1] = '\0';
}