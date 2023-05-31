#include "Patchboard.h"
#include <stdlib.h>

#define DEBOUNCE_TIME 20

Patchboard::Patchboard(int plugCount)
{
    _plugCount = plugCount;

    _connections = (ConnectionInfo**) calloc(plugCount, sizeof(ConnectionInfo*));
    _connectionCount = 0;

    // // Clear all connections
    // for(int i = 0; i < _plugCount; ++i)
    // {
    //     _connections[i] = -1;
    //     _debounceTimers[i] = 0;
    // }
}

void Patchboard::Update()
{
    for(int i = _connectionCount-1; i >= 0; --i)
    {
        ConnectionInfo* c = _connections[i];
        
        if(c->isActive)
        {
            if(c->debounceTime > 0 && millis() > c->debounceTime + DEBOUNCE_TIME)
            {
                c->debounceTime = 0;
                SendCallback(c->plugA, c->plugB, true);
            }
        }
        else
        {
            if(c->debounceTime > 0 && millis() > c->debounceTime + DEBOUNCE_TIME)
            {
                // Remove the array element by swapping it with the last item
                if(_connectionCount > 1 && i < _connectionCount - 1)
                {
                    _connections[i] = _connections[_connectionCount-1];
                }
                
                _connectionCount--;

                SendCallback(c->plugA, c->plugB, false);
                delete c;
            }
        }
    }

}

void Patchboard::SetConnection(int plugA, int plugB)
{
    if(plugA < 0)
    {
        return;
    }

    if(plugB < 0)
    {
        ClearConnection(plugA);
        return;
    }

    if(plugA == plugB)
    {
        return;        
    }

    if(_connectionCount >= _plugCount)
    {
        return;
    }

    // Serial.print("Setting connection (");
    // Serial.print(plugA);
    // Serial.print(" -> ");
    // Serial.print(plugB);
    // Serial.println(")");

    ConnectionInfo* connection = NULL;

    for(int i = 0; i < _connectionCount; ++i)
    {
        ConnectionInfo* c = _connections[i];

        if(c->connectsBothPlugs(plugA, plugB))
        {            
            connection = c;
            break;
        }
        else if(c->includesPlug(plugA, plugB) && c->isActive == true)
        {
            c->isActive = false;
            c->debounceTime = millis();
        }
    }

    // If we don't have a connection to use, create one here
    if(connection == NULL)
    {
        //Serial.println("Creating a new connection");       
        connection = new ConnectionInfo;
        connection->plugA = plugA;
        connection->plugB = plugB;
        connection->isActive = true;
        connection->debounceTime = millis();

        _connections[_connectionCount] = connection;
        ++_connectionCount;
    }
    else if(connection->isActive == false)
    {
        //Serial.println("Activating an existing connection");       

        connection->isActive = true;
        connection->debounceTime = millis();
    }
    else
    {
        //Serial.println("Connection already active");       

    }

}

void Patchboard::ClearConnection(int plug)
{
    // Loop through our connections and search for this plug. 
    // Mark any connections we find as not active.
    for(int i = 0; i < _connectionCount; ++i)
    {
        ConnectionInfo* c = _connections[i];

        if(c->isActive && c->includesPlug(plug))
        {
            c->isActive = false;
            c->debounceTime = millis();
        }
    }
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

int Patchboard::GetConnectedPlug(int plug)
{
    for(int i = 0; i < _connectionCount; ++i)
    {
        if(_connections[i]->isActive)
        {
            if(_connections[i]->plugA == plug)
            {
                return _connections[i]->plugB;
            }
            else if(_connections[i]->plugB == plug)
            {
                return _connections[i]->plugA;
            }
        }
    }

    return -1;
}

bool Patchboard::IsConnected(int plugA, int plugB)
{
    return GetConnectedPlug(plugA) == plugB;
}
