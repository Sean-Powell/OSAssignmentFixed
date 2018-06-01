//
// Created by sean on 24/05/2018.
//

#include "redirection.h"

#include <malloc.h>
#include <memory.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include "variables.h"
int fileFd;
int temp;

int startOutputRedirection(char* _fileLocation, bool _append){
    temp = dup(fileno(stdout));
    if(_append) {
        printf("apending\nn");
        fileFd = open(_fileLocation, O_WRONLY|O_CREAT|O_APPEND, 0666);
    }else{
        fileFd = open(_fileLocation, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    }

    if(dup2(fileFd, fileno(stdout)) == -1){
        printf("ERROR starting output redirection\n");
        return -1;
    }

    return 0;
}

int stopOutputRedirection(){
    fflush(stdout);
    close(fileFd);

    dup2(temp, fileno(stdout));
    close(temp);

    return 0;
}

int checkForRedirection(const char* _input){
    int numOfForward = 0;
    int numOfBackwards = 0;
    int i = 0;
    while(i < strlen(_input)){
        if(_input[i] == '>'){
            numOfForward++;
        }else if(_input[i] == '<'){
            numOfBackwards++;
        }
        i++;
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

