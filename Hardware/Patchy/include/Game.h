#pragma once
#include <Arduino.h>
#include <NeoPixelBusLg.h>

#include "Patchboard.h"

class Game
{
public:
    Game(int rows, int columns, NeoPixelBusLg<NeoGrbFeature, Neo800KbpsMethod> strip);

    void Initialize();
    void Update();

    void BeginGame();

    void OnPlugConnected(int x, int y);
    void OnPlugDisconnected(int x, int y);

    unsigned long GetGameTime() { return millis() - _gameStartMillis; }

private:

    struct EstablishedConnection
    {
        int plugA;
        int plugB;

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


    int _rows;
    int _columns;

    unsigned long _gameStartMillis = 0;

    NeoPixelBusLg<NeoGrbFeature, Neo800KbpsMethod>* _strip = 0;

    EstablishedConnection _connections[12];
    int _connectionCount = 0;


};