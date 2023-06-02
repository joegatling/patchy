
#include <Adafruit_MCP23X08.h>
#include <NeoPixelBusLg.h>
#include <Wire.h>

#include "Patchboard.h"
#include "Game.h"

#define SHOW_PATCH_DEBUG 0

#if SHOW_PATCH_DEBUG
    #define PATCH_PRINTLN(x) Serial.println(x);
    #define PATCH_PRINT(x) Serial.print(x);
#else
    #define PATCH_PRINTLN(x) ((void)0)
    #define PATCH_PRINT(x) ((void)0)
#endif


#if NODEMCU
    #define LED_PIN             D1       
#endif

#if UNO
    #define LED_PIN             8
#endif

#define PLUG_COUNT          32   // Total number of plugs

#define PANEL_COLUMNS 6
#define PANEL_ROWS 3

unsigned int currentPlug = 0;

Patchboard patchboard(PLUG_COUNT);

Adafruit_MCP23X08 mcp[PLUG_COUNT / 8];
NeoPixelBusLg<NeoGrbFeature, Neo800KbpsMethod> strip(PANEL_COLUMNS * PANEL_ROWS, LED_PIN);

void updateInput()
{
    // Step 1 - Increment the current pin and set that one to LOW

    currentPlug = (currentPlug + 1) % PLUG_COUNT;

    // Serial.print("Current plug: ");
    // Serial.println(currentPlug);

    mcp[currentPlug/8].pinMode(currentPlug%8, OUTPUT);
    mcp[currentPlug/8].digitalWrite(currentPlug%8, LOW);

    // Step 2 - Loop through all the input pins. The current pin is LOW, and other LOW
    // pins must be connected to it.
    int connectedPlug = -1;
    
    for(unsigned int i = 0; i < PLUG_COUNT; i++)
    {
        if(i != currentPlug)        
        {
            uint8_t val = mcp[i/8].digitalRead(i%8);
            
            PATCH_PRINT(val == HIGH ? "_" : "^" );

            if(val == 0)
            {
                
                connectedPlug = i;
                break;
                // Working on a 1-to-1 assumption, once we have found a single connection
                // we can break out of the loop early.
            }

        }
        else
        {
            PATCH_PRINT("*");
        }
        if(i%8 == 7)
        {
            PATCH_PRINT(" ");
        }
    }
    
    PATCH_PRINTLN("");

    mcp[currentPlug/8].pinMode(currentPlug%8, INPUT_PULLUP);    

    patchboard.SetConnection(currentPlug, connectedPlug);
}


// void onConnectionChanged(int plugA, int plugB, bool isConnected)
// {
//     Serial.print("Plug ");
//     Serial.print(plugA);

//     if(isConnected)
//     {
//         Serial.print(" CONNECTED to plug ");
//     }
//     else
//     {
//         Serial.print(" DISCONNECTED from plug ");
//     }

//     Serial.println(plugB);

//     int x = (plugA % PANEL_COLUMNS);
//     int y = (plugA / PANEL_COLUMNS);

//     if(isConnected)
//     {
//         game.OnPlugConnected(x, y);
//     }
//     else
//     {
//         game.OnPlugDisconnected(x, y);
//     }    
// }

void setup() 
{
    Serial.begin(115200);

    Serial.println("Patchy Test");

    bool mcpError = false;

    for(int i = 0; i < PLUG_COUNT / 8; ++i)
    {
        if (!mcp[i].begin_I2C(MCP23XXX_ADDR + i)) 
        {
            Serial.print("MCP ");
            Serial.print(i); 
            Serial.println(" Error.");
            mcpError = true;
        }
    }

    if(mcpError)
    {
        while(1)
        {
            //Do Nothing
        }
    }

    // Set up input pins
    for(int i = 0; i < PLUG_COUNT; i++)
    {
        mcp[i/8].pinMode(i%8, INPUT_PULLUP);
    }

    //patchboard.SetConnectionCallback(onConnectionChanged);
    
    Game::GetInstance()->Initialize(PANEL_ROWS, PANEL_COLUMNS, strip, patchboard);
    Game::GetInstance()->BeginGame();
}

void loop() 
{
    updateInput();
    patchboard.Update();
    Game::GetInstance()->Update();
}