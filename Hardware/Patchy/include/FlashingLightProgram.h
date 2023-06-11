#pragma once

#include <NeoPixelBusLg.h>
#include "ILightProgram.h"

class FlashingLightProgram : public ILightProgram
{
public:
    FlashingLightProgram(RgbColor colorA, RgbColor colorB, unsigned long interval)
    {
        _colorA = colorA;
        _colorB = colorB;
        _interval = interval;
        _startTime = millis();
    }

    FlashingLightProgram(RgbColor color, unsigned long interval)
    {
        _colorA = color;
        _colorB = RgbColor(0);
        _interval = interval;
    }


    RgbColor Update() override 
    {
        unsigned long val = (millis() - _startTime) % (_interval*2);
        return val < _interval ? _colorA : _colorB;
    }

private:
    RgbColor _colorA;
    RgbColor _colorB;

    unsigned long _interval;
    unsigned long _startTime;
};