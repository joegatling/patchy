#pragma once
#include <Arduino.h>
#include <NeoPixelBusLg.h>

#include "CircularBuffer.h"

#include "Patchboard.h"

#define MAX_CONNECTIONS 12

class Game
{
public:
    static Game* GetInstance();

    void Initialize(int rows, int columns, NeoPixelBusLg<NeoGrbFeature, Neo800KbpsMethod> strip, Patchboard patchboard);
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
    static Game* __instance;

    void GenerateNewConnectionRequest();
    bool IsPlugAlreadyInPlay(int plug);

    int _rows;
    int _columns;

    unsigned long _gameStartMillis = 0;

    NeoPixelBusLg<NeoGrbFeature, Neo800KbpsMethod>* _strip = 0;
    Patchboard* _patchboard = 0;

    ConnectionInfo _desiredConnections[MAX_CONNECTIONS];
    int _desiredConnectionCount = 0;

    unsigned long _newConnectionDelay = 0;
    unsigned long _lastUpdateMillis = 0;

    CircularBuffer<RgbColor> _colors;

    
};