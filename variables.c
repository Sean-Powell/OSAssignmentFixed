#include <malloc.h>
#include <memory.h>
#include "variables.h"

//
// Created by sean on 10/05/18.
//
#define DEBUG

VAR *vars;
int varsSize = 0;

int getVarsSize(){
    return varsSize;
}

VAR* getVars(){
    return vars;
}

int editVariable(char* _name, char* _newData){
    for(int i = 0; i < varsSize; i++){
        if(strstr(vars[i].name, _name)){
            char oldData[255];
            strcpy(oldData, vars[i].data);
            vars[i].data = _newData;
#ifdef DEBUG
            printf("DEBUG: editing variable name: %s, from %s to %s\n", vars[i].name, oldData, vars[i].data);
#endif
            return 0;
        }
    }

    return 1;
}

VAR findVariable(char *_name) {
    for (int i = 0; i < varsSize; i++) {
        if (strcmp(vars[i].name, _name) == 0) {
            return vars[i];
        }
    }
    VAR var;
    var.name = NULL;
    var.data = NULL;
    return var;
}

int createVariable(char *_name, char *_data) {
    varsSize++;
    vars = realloc(vars, sizeof(VAR) * varsSize);
    VAR v1;
    v1.data = malloc(strlen(_data));
    v1.name = malloc(sizeof(_name));
    if(strcpy(v1.data, _data) && strcpy(v1.name, _name)){
        vars[varsSize - 1] = v1;
    }
}
