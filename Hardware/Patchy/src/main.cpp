
#include <Adafruit_MCP23X08.h>
#include <NeoPixelBusLg.h>
#include <Wire.h>

#include "Patchboard.h"
#include "Game.h"

#if NODEMCU
    #define SERIAL_OUT_PIN      D0

    #define LATCH_PIN           D5
    #define CLOCK_PIN           D6
    #define CLEAR_PIN           D7

    #define LED_PIN             D1       
#endif

#if UNO
    #define SERIAL_OUT_PIN      7

    #define LATCH_PIN           6
    #define CLOCK_PIN           5
    #define CLEAR_PIN           4

    #define LED_PIN             8
#endif

#define PLUG_COUNT          32   // Total number of plugs

#define PANEL_COLUMNS 6
#define PANEL_ROWS 3

unsigned int currentPlug = 0;

Patchboard patchboard(PLUG_COUNT);

Adafruit_MCP23X08 mcp[PLUG_COUNT / 8];
NeoPixelBusLg<NeoGrbFeature, Neo800KbpsMethod> strip(PANEL_COLUMNS * PANEL_ROWS, LED_PIN);
Game game(PANEL_ROWS, PANEL_COLUMNS, strip);

void updateInput()
{
    // Step 1 - Shift the ouput pin. At each update there is a single plug
    // set HIGH. We can then test the voltage on each other plug to determine 
    // if any have been connected.

    digitalWrite(LATCH_PIN, LOW);
    digitalWrite(CLOCK_PIN, HIGH);
    digitalWrite(CLOCK_PIN, LOW);
    digitalWrite(LATCH_PIN, HIGH);

    // This keeps track of which plug is currently HIGH
    currentPlug = (currentPlug + 1) % PLUG_COUNT;
    //currentPlug = 0;

    Serial.print("Current plug: ");
    Serial.println(currentPlug);

    mcp[currentPlug/8].pinMode(currentPlug%8, OUTPUT);
    mcp[currentPlug/8].digitalWrite(currentPlug%8, HIGH);

    // Step 2 - Loop through all the input pins. The pin corresponding to the output is expected
    // to be high. Any other pin that is high must have been patched.
    int connectedPlug = -1;
    
  //  Serial.println("Connections: ");

    for(unsigned int i = 0; i < PLUG_COUNT; i++)
    {
        //if(i != currentPlug)
        if(true)
        {
            uint8_t val = mcp[i/8].digitalRead(i%8);
            Serial.print(val);

            if(val == 0)
            {
                
                connectedPlug = i;
                //break;
                // Working on a 1-to-1 assumption, once we have found a single connection
                // we can break out of the loop early.
            }

        }
        else
        {
            Serial.print("_");
        }

        if(i%8 == 7)
        {
            Serial.print(" ");
        }
    }
    Serial.println("");

    mcp[currentPlug/8].pinMode(currentPlug%8, INPUT);
    //mcp[currentPlug/8].digitalWrite(currentPlug%8, HIGH);
    //Serial.println("");
    

    patchboard.SetConnection(currentPlug, connectedPlug);
}

void initOutputCycle()
{
    digitalWrite(CLEAR_PIN, HIGH);
    digitalWrite(LATCH_PIN, LOW);

    int mcuCount = PLUG_COUNT/8;
    for(int i = 0; i < mcuCount; ++i)
    {
        //uint8_t val = i == (mcuCount-1) ? 1 : 0;
        //uint8_t val = i == (mcuCount-1) ? ~1 : ~0;
        uint8_t val = 0;
        Serial.print("Output: ");
        
        for (int i = 7; i >= 0; i--)
        {
            bool b = bitRead(val, i);
            Serial.print(b);
        }

        Serial.println("");
        shiftOut(SERIAL_OUT_PIN, CLOCK_PIN, MSBFIRST, val);
    }

    digitalWrite(LATCH_PIN, HIGH);
    digitalWrite(CLOCK_PIN, LOW);
    
    // Set the serial output pin to input. This makes it floating so that
    // it can be driven by the output of the shift register.
    pinMode(SERIAL_OUT_PIN, INPUT);

    currentPlug = 0;
}

void onConnectionChanged(int plugA, int plugB, bool isConnected)
{
    Serial.print("Plug ");
    Serial.print(plugA);

    if(isConnected)
    {
        Serial.print(" CONNECTED to plug ");
    }
    else
    {
        Serial.print(" DISCONNECTED from plug ");
    }

    Serial.println(plugB);

    int x = (plugA % PANEL_COLUMNS);
    int y = (plugA / PANEL_COLUMNS);

    if(isConnected)
    {
        game.OnPlugConnected(x, y);
    }
    else
    {
        game.OnPlugDisconnected(x, y);
    }    
}

void setup() 
{
#if KIT32
	Heltec.begin(true, false, true);
	Wire.begin(SDA_OLED, SCL_OLED); //Scan OLED's I2C address via I2C0
#else    
    Serial.begin(115200);
#endif

    Serial.println("Patchy Test");

    //u8g2.begin();
    bool mcpError = false;

    for(int i = 0; i < PLUG_COUNT / 8; ++i)
    {
        if (!mcp[i].begin_I2C(MCP23XXX_ADDR + i)) 
        {
            Serial.print("MCP ");
            Serial.print(i); 
            Serial.println(" Error.");
            mcpError = true;
            //while (1);
        }
    }

    if(mcpError)
    {
        while(1)
        {
            //Do Nothing
        }
    }

    // // Set up input pins
    for(int i = 0; i < PLUG_COUNT; i++)
    {
        mcp[i/8].pinMode(i%8, INPUT);
    }

    // Set up output cycler
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(CLEAR_PIN, OUTPUT);
    pinMode(SERIAL_OUT_PIN, OUTPUT);

    initOutputCycle();

    patchboard.SetConnectionCallback(onConnectionChanged);
    
   // game.Initialize();
}

void loop() 
{
    updateInput();
    patchboard.Update();

    delay(500);
    //game.Update();
}