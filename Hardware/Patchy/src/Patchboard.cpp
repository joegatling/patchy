#include "Patchboard.h"
#include <stdlib.h>

//#define DEBOUNCE_TIME 20

Patchboard::Patchboard(int plugCount)
{
    _connections = (int*) calloc(plugCount, sizeof(int));
    _plugCount = plugCount;
    //_debounceTimers = (unsigned long*) calloc(plugCount, sizeof(unsigned long));

    // Clear all connections
    for(int i = 0; i < _plugCount; ++i)
    {
        _connections[i] = -1;
      //  _debounceTimers[i] = 0;
    }
}

// void Patchboard::Update()
// {
//     //for(int i = 0; i < )
// }

void Patchboard::SetConnection(int plugA, int plugB)
{
    if(plugB < 0)
    {
        ClearConnection(plugA);
        return;
    }

    if(plugA == plugB)
    {
        return;        
    }

    if(_connections[plugA] == plugB)
    {
        return;
    }

    ClearConnection(plugA);
    ClearConnection(plugB);

    _connections[plugA] = plugB;
    _connections[plugB] = plugA;

    SendCallback(plugA, plugB, true);
}

void Patchboard::ClearConnection(int plug)
{
    if(_connections[plug] < 0)
    {
        return;
    }

    int oldConnection = _connections[plug];
    _connections[oldConnection] = -1;
    _connections[plug] = -1;

    SendCallback(plug, oldConnection, false);
}

void Patchboard::SetConnectionCallback(connectionCallbackFunction function)
{
    _connectionUpdatedCallback = function;
}

void Patchboard::SendCallback(int plugA, int plugB, bool connectionState)
{
    if(_connectionUpdatedCallback != NULL)
    {
        _connectionUpdatedCallback(min(plugA, plugB), max(plugA, plugB), connectionState);
    }
}