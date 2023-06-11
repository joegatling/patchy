#include "LightGrid.h"

LightGrid::LightGrid(NeoPixelBusLg<NeoRgbFeature, Neo800KbpsMethod> strip)
{
    _programs = new ILightProgram*[strip.PixelCount()];
    _strip = &strip;
    _strip->ClearTo(RgbColor(0));
    _strip->Show();
}

void LightGrid::SetLightProgram(int index, ILightProgram *program)
{
    if(_programs[index] != 0)
    {
        ClearLightProgram(index);
    }

    program->SetIndex(index);
}

void LightGrid::ClearLightProgram(int index)
{
    if(_programs[index] != 0)
    {
        delete _programs[index];
        _programs[index] = 0;
    }
}

ILightProgram* LightGrid::GetLightProgram(int index)
{
    return _programs[index];
}

void LightGrid::Update()
{   
    for(unsigned int i = 0; i < sizeof(_programs); i++)
    {
        if(_programs[i] != 0)
        {
            _strip->SetPixelColor(i, _programs[i]->Update());
        }
    }
}
