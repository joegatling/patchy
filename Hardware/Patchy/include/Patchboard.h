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

    int GetConnectedPlug(int plug);
    bool IsConnected(int plugA, int plugB);

    void Update();

private:
    struct ConnectionInfo
    {
        int plugA;
        int plugB;

        bool isActive;

        unsigned long debounceTime;

        bool includesPlug(int p)
        {
            return plugA == p || plugB == p;
        }

        bool includesPlug(int p1, int p2)
        {
            return includesPlug(p1) || includesPlug(p2);
        }


        bool connectsBothPlugs(int p1, int p2)
        {
            return (plugA == p1 && plugB == p2) || (plugA == p2 && plugB == p1);
        }
    };

    ConnectionInfo** _connections;
    int _connectionCount;

    int _plugCount;

//    unsigned long* _debounceTimers;

    void SendCallback(int plugA, int plugB, bool connectionState);

    connectionCallbackFunction _connectionUpdatedCallback = NULL;
    
};