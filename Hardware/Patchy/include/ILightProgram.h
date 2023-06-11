#pragma once
#include <NeoPixelBusLg.h>

class ILightProgram 
{
public:
    virtual RgbColor Update() = 0;
    
    int SetIndex(int index) { _index = index; }
    int GetIndex() { return _index; }

private:
    int _index;
};