#pragma once
#include <NeoPixelBusLg.h>
#include "ILightProgram.h"

class LightGrid
{
public:
    LightGrid(NeoPixelBusLg<NeoRgbFeature, Neo800KbpsMethod> strip);

    void SetLightProgram(int index, ILightProgram* program);
    void ClearLightProgram(int index);
    ILightProgram* GetLightProgram(int index);

    void Update();

private:
    ILightProgram** _programs;
    NeoPixelBusLg<NeoRgbFeature, Neo800KbpsMethod>* _strip = 0;


};