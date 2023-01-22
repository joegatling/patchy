#pragma once
#include <Arduino.h>

extern "C" {
typedef void (*connectionCallbackFunction)(int, int, bool);
}


class Patchboard 
{
public:
    Patchboard(int plugCount);

    void SetConnection(int plugA, int plugB);
    void ClearConnection(int plugA);

    void SetConnectionCallback(connectionCallbackFunction function);

    //void Update();

private:
    int* _connections;
    int _plugCount;

    //unsigned long* _debounceTimers;

    void SendCallback(int plugA, int plugB, bool connectionState);

    connectionCallbackFunction _connectionUpdatedCallback = NULL;
    
};