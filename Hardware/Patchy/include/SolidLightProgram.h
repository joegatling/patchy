#pragma once

#include <NeoPixelBusLg.h>
#include "ILightProgram.h"

class SolidLightProgram : public ILightProgram
{
public:
    SolidLightProgram(RgbColor color)
    {
        _color = color;
    }

    RgbColor Update() override 
    {
        return _color;
    }

private:
    RgbColor _color;
};