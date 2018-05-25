//
// Created by sean on 24/05/2018.
//

#include "redirection.h"

#include <malloc.h>
#include <memory.h>
#include <stdbool.h>
#include "variables.h"

char* targetForRedirection;

int outputRedirection(char *_string, VAR *_vars, int _numOfVars, bool _append, FILE* file) {
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

    output[i + offset]='\0';
    printf("Writing %s to file\n", output);
    fwrite(output, sizeof(char), strlen(output), file);
    return 0;
}

int checkForRedirection(const char* _input){
    int numOfForward = 0;
    int numOfBackwards = 0;
    int i = 0;
    while(i < strlen(_input)){
        printf("%c is being checked\n", _input[i]);
        if(_input[i] == '>'){
            numOfForward++;
        }else if(_input[i] == '<'){
            numOfBackwards++;
        }
        i++;
    }
    printf("Num of >: %d, Num of <: %d\n", numOfForward, numOfBackwards);

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

char* getTarget(){
    return targetForRedirection;
}

int setTarget(char* _target){
    printf("Setting target to %s\n", _target);
    targetForRedirection = malloc((strlen(_target))* sizeof(char));
    strcpy(targetForRedirection, _target);
    printf("Target set to %s\n", _target);
    return 0;
}