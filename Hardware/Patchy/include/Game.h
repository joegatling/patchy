#pragma once
#include <Arduino.h>
#include <NeoPixelBusLg.h>

#include "CircularBuffer.h"

#include "Patchboard.h"

#define MAX_CONNECTIONS 6

class Game
{
public:
    static Game* GetInstance();

    void Initialize(int rows, int columns, NeoPixelBusLg<NeoRgbFeature, Neo800KbpsMethod> *strip, Patchboard *patchboard);
    void Update();

    void BeginGame();

    void OnConnectionChanged(int plugA, int plugB, bool isConnected);

    unsigned long GetGameTime() { return millis() - _gameStartMillis; }

private:

    struct ConnectionInfo
    {
        int plugA;
        int plugB;

        unsigned long waitTimeRemaining;
        unsigned long timeRemaining;

        RgbColor color;

        bool isConnected;

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

    Game();
    static Game *__instance;

    void GenerateNewConnectionRequest(int include, int exclude);
    bool IsPlugAlreadyInPlay(int plug);

    int _rows;
    int _columns;

    unsigned long _gameStartMillis = 0;

    NeoPixelBusLg<NeoRgbFeature, Neo800KbpsMethod>* _strip = 0;
    Patchboard* _patchboard = 0;

    ConnectionInfo _desiredConnections[MAX_CONNECTIONS];
    unsigned int _desiredConnectionCount = 0;

    unsigned long _newConnectionDelay = 0;
    unsigned long _lastUpdateMillis = 0;

    unsigned long _flashStartTime = 0;
    RgbColor _flashColor;

    int GetXCoordinate(int index) { return GetYCoordinate(index) % 2 == 0 ? index % _columns : (_columns - 1) - (index % _columns) ; };
    int GetYCoordinate(int index) { return index / _columns; };
    int GetIndex(int x, int y) 
    { 
        if(y % 2 == 0)
        {
            return y * _columns + x;
        }
        else
        {
            return y * _columns + ((_columns-1) - x);
        }
    }

    CircularBuffer<RgbColor> _colors;

    
};