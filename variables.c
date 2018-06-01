#include <malloc.h>
#include <memory.h>
#include "variables.h"

//
// Created by sean on 10/05/18.
//

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
            strcpy(vars[i].data, _newData);
            return 0;
        }
    }

    return -1;
}

char* getVarData(char* _varName){
    for(int i = 0; i < varsSize; i++){
        if(strcmp(_varName, vars[i].name) == 0){
            return vars[i].data;
        }
    }
    //returns null if data could not be found
    return NULL;
}

int setVarToVar(char *_varName1, char *_varName2){
    char* varData = getVarData(_varName2);
    if(varData == NULL){
        //var 2 does not exist
        return -2;
    }
    return editVariable(_varName1, varData);
    //-1 if var 1 does not exist, 0 on success.
}


int createVariable(char *_name, char *_data) {
    varsSize++;
    vars = realloc(vars, sizeof(VAR) * varsSize);
    VAR v1;
    v1.data = malloc(strlen(_data));
    v1.name = malloc(sizeof(_name));
    if(strcpy(v1.data, _data) && strcpy(v1.name, _name)){
        vars[varsSize - 1] = v1;
        return 0;
    }
    return -1;
}
