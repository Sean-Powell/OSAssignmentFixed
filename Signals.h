//
// Created by sean on 25/05/2018.
//

#ifndef OSASSINGMENT_SIGNALS_H
#define OSASSINGMENT_SIGNALS_H

void quitHandler(int _signal);
void sleepHandler(int _signal);
void killZombies();
void wakeHandler(int _signal);
void signalManager(int _signal);
void pushToStack(int _pid);
pid_t popFromStack();

#endif //OSASSINGMENT_SIGNALS_H
