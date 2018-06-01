//
// Created by sean on 25/05/2018.
//

#include <bits/signum.h>
#include <signal.h>
#include <stdio.h>
#include "Signals.h"

#define MAX_PIDS 255

pid_t pid;
pid_t pidStack[MAX_PIDS];
int stackSize = -1;

void pushToStack(int _pid){
     if(stackSize + 1 <= MAX_PIDS){
         printf("pid %d added to stack\n", _pid);
         stackSize++;
         pidStack[stackSize] = _pid;
     }else{
         printf("ERROR: pid stack is full\n");
     }
}

pid_t popFromStack(){
    if(stackSize == -1){
        printf("ERROR: no pids on the stack\n");
    }else{
        pid_t returnPid = pidStack[stackSize];
        printf("pid %d removed from stack\n", returnPid);
        stackSize--;
        return returnPid;
    }
}

void signalManager(int _signal){
    pid = popFromStack();
    switch(_signal){
        case SIGCONT:
            wakeHandler(_signal);
            break;
        case SIGTSTP:
            sleepHandler(_signal);
            break;
        case SIGINT:
            quitHandler(_signal);
            break;
        default:
            printf("An unknown signal occurred\n");
            break;
    }
}

void quitHandler(int _signal){
    printf("Quitting\n");
    kill(pid, SIGINT);
    popFromStack();
}

void sleepHandler(int _signal){
    printf("Sleeping\n");
    kill(pid, SIGTSTP);
}

void wakeHandler(int _signal){
    pid = popFromStack();
    printf("Waking up\n");
    kill(pid, SIGCONT);
    pushToStack(pid);
}