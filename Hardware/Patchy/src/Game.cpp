#include "Game.h"

#define MAX_REQUESTED_CONNECION_COUNT min(MAX_CONNECTIONS, max(2, Game::GetGameTime() / 20000))
//#define MAX_REQUESTED_CONNECION_COUNT MAX_CONNECTIONS

#define MAX_WAIT_TIME 5000
#define MAX_CONNECTION_TIME 20000
#define MIN_CONNECTION_TIME 15000
#define MAX_CONNECTION_INTERVAL 3000

Game* Game::__instance = 0;

Game* Game::GetInstance()
{
    if(__instance == 0)
    {
        __instance = new Game();
    }

    // Serial.print("Instance: 0x");
    // Serial.println((int)__instance, HEX);

    return __instance;
}

Game::Game():
_colors(MAX_CONNECTIONS)
{
    char buffer[10];




    // Generate a list of (hopefully) visually distinct colors
    // for(int i = 0; i < MAX_CONNECTIONS; i++)
    // {   
    //     auto c = RgbColor(HsbColor((1.0f / (MAX_CONNECTIONS + 1)) * i, 1.0f, 1.0f));
    //     _colors.Add(c);
    // }

    _colors.Add(RgbColor(255,0,0));
    _colors.Add(RgbColor(0,255,0));
    _colors.Add(RgbColor(0,0,255));
    _colors.Add(RgbColor(0,255,255));
    _colors.Add(RgbColor(255,255,0));
    _colors.Add(RgbColor(255,0,255));

    // for(int i = 0; i < _colors.GetCount(); i++)
    // {   
    //     RgbColor c = _colors[i];

    //     sprintf(buffer, "Color: %02x%02x%02x", c.R, c.G, c.B);
    //     Serial.println(buffer);
    // }    

   // Serial.println(_colors.GetCount());
}

void Game::GenerateNewConnectionRequest(int plugA = -1, int exclude = -1)
{
    //Serial.println("Time for a new connection");

    // Do we have too many desired connections already?
    if(_desiredConnectionCount >= MAX_CONNECTIONS)
    {
        //Serial.println("Already reached limit of desired connections.");
        return;
    }

    // Step 1 - Find an unused plug
    if(plugA < 0)
    {
        plugA = random(0, _rows * _columns);

        while(IsPlugAlreadyInPlay(plugA))
        {
            plugA = random(0,  _rows * _columns);
        }
    }
    else
    {
        if(IsPlugAlreadyInPlay(plugA))
        {
            return;
        }
    }

    // Serial.print("Plug A is ");
    // Serial.println(plugA);

    // Step 2 - Generate a list of all plugs within range that are unoccupied
    int centerX = GetXCoordinate(plugA);
    int centerY = GetYCoordinate(plugA);

    int minDistance = 2;
    int maxDistance = 4;

    int indexes[maxDistance * maxDistance]; // This is bigger than needed, but I don't know how to allocate the right amount
    int indexCount = 0;

    for (int y = 0; y < _rows; y++) 
    {
        for (int x = 0; x < _columns; x++) 
        {
            int dx = abs(centerX - x);
            int dy = abs(centerY - y);

            int currentDistance = dx + dy;

            if (currentDistance >= minDistance && currentDistance <= maxDistance) 
            {
                int currentIndex = GetIndex(x,y);

                // Serial.print("   Plug ");
                // Serial.print(currentIndex);

                if(!IsPlugAlreadyInPlay(currentIndex) || currentIndex == exclude)
                {
                    indexes[indexCount++] = currentIndex;
                    //Serial.println(" is available.");
                } 
                else
                {
                    //Serial.println(" is OCCUPIED.");
                }
            }
        }
    }

    if(indexCount == 0)
    {
        //Serial.println("No candidate plugs found!");
        return;
    }
    else
    {
        // Serial.print("Candidate plugs: ");
        // Serial.println(indexCount);
    }

    // Step 3 - Select a random plug and generate a connection request
    int plugB = indexes[random(0,indexCount)];

    int bX = GetXCoordinate(plugB);
    int bY = GetYCoordinate(plugB);

    int dx = abs(centerX - bX);
    int dy = abs(centerY - bY);

    int currentDistance = dx + dy;

    _desiredConnections[_desiredConnectionCount].plugA = plugA;
    _desiredConnections[_desiredConnectionCount].plugB = plugB;

    if(_colors.GetCount() > 0)
    {
        _desiredConnections[_desiredConnectionCount].color = _colors.Dequeue();

        // char buffer[10];
        // sprintf(buffer, "Color: %02x%02x%02x", _desiredConnections[_desiredConnectionCount].color.R, _desiredConnections[_desiredConnectionCount].color.G, _desiredConnections[_desiredConnectionCount].color.B);
        // Serial.println(buffer);

    }
    else
    {
        //Serial.println("WHITE");
        _desiredConnections[_desiredConnectionCount].color = RgbColor(255,255,255);
    }

    _desiredConnections[_desiredConnectionCount].waitTimeRemaining = MAX_WAIT_TIME;
    _desiredConnections[_desiredConnectionCount].timeRemaining = random(MIN_CONNECTION_TIME, MAX_CONNECTION_TIME);



    // char buffer[64];
    // sprintf(buffer, "New task: %d to %d - Color: %02x%02x%02x - Distance: %d", plugA, plugB, _desiredConnections[_desiredConnectionCount].color.R, _desiredConnections[_desiredConnectionCount].color.G, _desiredConnections[_desiredConnectionCount].color.B, currentDistance);
    // Serial.println(buffer);


    _desiredConnectionCount++;
}

bool Game::IsPlugAlreadyInPlay(int plug)
{
    for(unsigned int i = 0; i < _desiredConnectionCount; i++)
    {
        if(_desiredConnections[i].includesPlug(plug))
        {
            return true;
        }
    }

    return false;
}


void Game::Initialize(int rows, int columns, NeoPixelBusLg<NeoRgbFeature, Neo800KbpsMethod> *strip, Patchboard *patchboard)
{
    _rows = rows;
    _columns = columns;

    _strip = strip;
    _patchboard = patchboard;

    // Serial.print("Init Patchboard: ");
    // Serial.println(_patchboard->GetPlugCount());

    _desiredConnectionCount = 0;

    auto connectionCallback = [](int plugA, int plugB, bool isConnected)
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

        Game::GetInstance()->OnConnectionChanged(plugA, plugB, isConnected);
    };

    _patchboard->SetConnectionCallback(connectionCallback);
}

void Game::Update()
{
    // Calculate dt since last update
    unsigned long deltaTime = millis() - _lastUpdateMillis;
    _lastUpdateMillis = millis();

    _strip->ClearTo(RgbColor(0));    
    _strip->SetLuminance(128);

    float t = (sin(millis() / 100.0f) + 1.0f) / 2.0f;
    float t2 = (sin(millis() / 300.0f) + 1.0f) / 2.0f;

    // Serial.print("Loop Patchboard: ");
    // Serial.println((int)_patchboard);


    for(int i = _desiredConnectionCount-1; i >= 0; i--)
    {
        auto connection = &_desiredConnections[i];

        if(_patchboard->IsConnected(connection->plugA, connection->plugB))
        {
            // Logic for when the plugs are connected
            if(connection->isConnected == false)
            {
                connection->isConnected = true;
                _flashColor = connection->color;
                _flashStartTime = millis();
            }

            // Recharge the waiting time
            connection->waitTimeRemaining = min(MAX_WAIT_TIME, connection->waitTimeRemaining + deltaTime / 2);

            // Check to see if we're done
            if(connection->timeRemaining < deltaTime)
            {
                int plugA = connection->plugA;
                int plugB = connection->plugB;

                if(random(1) == 0)
                {
                    int temp = plugA;
                    plugA = plugB;
                    plugB = temp;
                }

                connection->timeRemaining = 0;

                Serial.print("Desired connection (");
                Serial.print(connection->plugA);
                Serial.print(",");
                Serial.print(connection->plugB);
                Serial.println(") has been completed!");

                // Add the color back to the list
                _colors.Enqueue(connection->color);

                // Remove this connection
                _desiredConnections[i] = _desiredConnections[_desiredConnectionCount-1];
                _desiredConnectionCount--;

                _flashColor = RgbColor(255);
                _flashStartTime = millis();  

                // Rapid Reconnect Option              
                if(_desiredConnectionCount < MAX_REQUESTED_CONNECION_COUNT)
                {
                    if(random(100) < 50)
                    {
                        GenerateNewConnectionRequest(plugA);
                    }
                }
            }
            else
            {
                // Decrement the time remaining
                connection->timeRemaining = connection->timeRemaining - deltaTime;

                // RgbColor c = connection->color;
                // c = c.Dim(64);
                RgbColor c = RgbColor(255,255,255);
                
                if(connection->timeRemaining > 1500)
                {
                    c = c.Dim(96);
                }

                _strip->SetPixelColor(connection->plugA, c);
                _strip->SetPixelColor(connection->plugB, c);
            }
        }
        else
        {
            // Logic for when the plugs are disoconnected
            connection->isConnected = false;

            connection->waitTimeRemaining = max(MAX_WAIT_TIME, connection->waitTimeRemaining - deltaTime);
            connection->timeRemaining = min(MAX_CONNECTION_TIME, connection->timeRemaining + deltaTime);

            if(connection->waitTimeRemaining == 0)
            {
                // Do nothing for now
                Serial.print("Desired connection (");
                Serial.print(connection->plugA);
                Serial.print(",");
                Serial.print(connection->plugB);
                Serial.println(") has timed out!");                
            }
            
            RgbColor color = RgbColor::LinearBlend(connection->color.Dim(32), connection->color, connection->waitTimeRemaining > 2 ? t : t2);
            
            _strip->SetPixelColor(connection->plugA, color);
            _strip->SetPixelColor(connection->plugB, color);

        }
    }

    if(_desiredConnectionCount < MAX_REQUESTED_CONNECION_COUNT)
    {


        if(_newConnectionDelay < deltaTime)
        {
            _newConnectionDelay = MAX_CONNECTION_INTERVAL;

            GenerateNewConnectionRequest();
        }
        else
        {
            _newConnectionDelay = _newConnectionDelay - deltaTime;

            // Serial.print("New connection in ");
            // Serial.println(_newConnectionDelay);
        }

    }

    if(_flashStartTime != 0 && (millis() - _flashStartTime) < 200)    
    {
        uint8_t t = (millis() - _flashStartTime);
        uint8_t dim =  (t / 20) % 2 ? 255 : 64;
        _strip->ClearTo(_flashColor.Dim(dim));
    }
    
    _strip->Show();
}

void Game::BeginGame()
{
//     Serial.print("Begin Patchboard: ");
//     Serial.println(_patchboard->GetPlugCount());

    pinMode(A0, INPUT);
    pinMode(A1, INPUT);

    int a = analogRead(A0);
    int b = analogRead(A1);

    // Serial.println(a);
    // Serial.println(b);

    randomSeed(a * b);

    _gameStartMillis = millis();
    _lastUpdateMillis = millis();
    _newConnectionDelay = 1000;

    _strip->ClearTo(RgbColor(0));
    _strip->Show();

    for (int y = 0; y < _rows; y++) 
    {
        for (int x = 0; x < _columns; x++) 
        {
            int index = GetIndex(x,y);
            _strip->SetPixelColor(index, RgbColor(255,255,255));
            _strip->Show();
            delay(10);
        }
    }

    delay(200);
}

void Game::OnConnectionChanged(int plugA, int plugB, bool isConnected)
{

}
