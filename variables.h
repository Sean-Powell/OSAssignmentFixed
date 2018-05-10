//
// Created by sean on 10/05/18.
//

#ifndef OSASSINGMENT_VARIABLES_H
#define OSASSINGMENT_VARIABLES_H
typedef struct variable {
    char *data;
    char *name;
} VAR;
int getVarsSize();
VAR* getVars();
VAR findVariable(char *_name);
int editVariable(char *_name, char* _newData);
int createVariable(char *_name, char *_data);

#endif //OSASSINGMENT_VARIABLES_H
