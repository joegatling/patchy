// Controls an LED via an attached button.

// ok to include only the one needed
// both included here to make things simple for example
#include <Adafruit_MCP23X08.h>
//#include <ESP8266WiFi.h>

#include "Patchboard.h"

#define SERIAL_OUT_PIN      D0

#define LATCH_PIN           D5
#define CLOCK_PIN           D6
#define CLEAR_PIN           D7

#define PLUG_COUNT          8   // Total number of plugs

unsigned int currentPlug = 0;

Patchboard patchboard(PLUG_COUNT);

// uncomment appropriate line
Adafruit_MCP23X08 mcp;

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

    // Serial.print("Current plug: ");
    // Serial.println(currentPlug);

    // Step 2 - Loop through all the input pins. The pin corresponding to the output is expected
    // to be high. Any other pin that is high must have been patched.
    int connectedPlug = -1;

    // Serial.print("Connections: ");

    for(unsigned int i = 0; i < PLUG_COUNT; i++)
    {
        //Serial.print(mcp.digitalRead(i));

        if(i != currentPlug)
        {
            if(mcp.digitalRead(i) == 0)
            {
                connectedPlug = i;
                break;
                // Working on a 1-to-1 assumption, once we have found a single connection
                // we can break out of the loop early.
            }
        }
    }

    //Serial.println("");

    patchboard.SetConnection(currentPlug, connectedPlug);
}

void initOutputCycle()
{
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(SERIAL_OUT_PIN, CLOCK_PIN, MSBFIRST, ~1);
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
        Serial.print(" is CONNECTED to plug ");
    }
    else
    {
        Serial.print(" DISCONNECTED from plug ");
    }

    Serial.println(plugB);
}

void setup() 
{
    Serial.begin(9600);
    Serial.println("Patchy Test");

    if (!mcp.begin_I2C()) 
    {
        Serial.println("MCP Error.");
        while (1);
    }

    // // Set up input pins
    // for(int i = 0; i < PLUG_COUNT; i++)
    // {
    //     mcp.pinMode(i, INPUT);
    // }

    // Set up ouput cycler
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(SERIAL_OUT_PIN, OUTPUT);

    initOutputCycle();

    patchboard.SetConnectionCallback(onConnectionChanged);

    //WiFi.mode(WIFI_OFF);
}

void loop() 
{
    updateInput();
    delay(10);
    //mcp.digitalWrite(LED_PIN, !mcp.digitalRead(BUTTON_PIN));
}

// void startOutputCycler()
// {
// for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) 
// {
//         // take the latchPin low so
//     // the LEDs don't change while you're sending in bits:
//     digitalWrite(LATCH_PIN, LOW);
//     // shift out the bits:
//     shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, numberToDisplay);
//     //take the latch pin high so the LEDs will light up:
//     digitalWrite(LATCH_PIN, HIGH);
//     // pause before next value:
//     delay(500);
// }
// }