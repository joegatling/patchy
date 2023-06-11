#pragma once

#include <NeoPixelBusLg.h>
#include "ILightProgram.h"

class PulsingLightProgram : public ILightProgram
{
public:
    PulsingLightProgram(RgbColor colorA, RgbColor colorB, unsigned long interval)
    {
        _colorA = colorA;
        _colorB = colorB;
        _interval = interval;
        _startTime = millis();
    }

    PulsingLightProgram(RgbColor color, unsigned long interval)
    {
        _colorA = color;
        _colorB = RgbColor(0);
        _interval = interval;
    }


    RgbColor Update() override 
    {
        double t = (1.0 + cos((millis() * PI * 2) / 1000)) / 2.0;
        return RgbColor::LinearBlend(_colorA, _colorB, t);
    }

private:
    RgbColor _colorA;
    RgbColor _colorB;

    unsigned long _interval;
    unsigned long _startTime;
};