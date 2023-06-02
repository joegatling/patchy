#include "Game.h"

#define MAX_REQUESTED_CONNECION_COUNT max(MAX_CONNECTIONS, Game::GetGameTime() / 20000)

#define MAX_WAIT_TIME 5000
#define MAX_CONNECTION_TIME 20000
#define MIN_CONNECTION_TIME 15000
#define MAX_CONNECTION_INTERVAL 3000

Game* Game::__instance = 0;

Game::Game():
_colors(MAX_CONNECTIONS)
{
    // Generate a list of (hopefully) visually distinct colors
    for(int i = 0; i < MAX_CONNECTIONS; i++)
    {        
        _colors.Add(RgbColor(HsbColor((1.0f / MAX_CONNECTIONS + 1) * i, 1.0f, 1.0f)));
    }
}

void Game::GenerateNewConnectionRequest()
{
    Serial.println("Time for a new connection");

    // Do we have too many desired connections already?
    if(_desiredConnectionCount >= MAX_CONNECTIONS)
    {
        Serial.println("Already reached limit of desired connections.");
        return;
    }

    // Step 1 - Find an unused plug
    int plugA = random(0, _patchboard->GetPlugCount());

    while(_patchboard->isPlugOccupied(plugA))
    {
        int plugA = random(0, _patchboard->GetPlugCount());
    }

    Serial.print("Plug A is ");
    Serial.println(plugA);

    // Step 2 - Generate a list of all plugs within range that are unoccupied
    int centerX = plugA % _columns;
    int centerY = plugA / _columns;    

    int minDistance = 2;
    int maxDistance = 5;

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
                int currentIndex = y * _columns + x;

                Serial.print("   Plug");
                Serial.print(currentIndex);

                if(!_patchboard->isPlugOccupied(currentIndex))
                {
                    indexes[indexCount++] = currentIndex;
                    Serial.println(" is available.");
                } 
                else
                {
                    Serial.println(" is OCCUPIED.");
                }
            }
        }
    }

    if(indexCount == 0)
    {
        Serial.println("No candidate plugs found!");
        return;
    }
    else
    {
        Serial.print("Candidate plugs: ");
        Serial.println(indexCount);
    }

    // Step 3 - Select a random plug and generate a connection request
    int plugB = indexes[random(0,indexCount)];

    _desiredConnections[_desiredConnectionCount].plugA = plugA;
    _desiredConnections[_desiredConnectionCount].plugB = plugB;

    if(_colors.GetCount() > 0)
    {
        _desiredConnections[_desiredConnectionCount].color = _colors.Dequeue();
    }
    else
    {
        _desiredConnections[_desiredConnectionCount].color = RgbColor(255,255,255);
    }

    _desiredConnections[_desiredConnectionCount].waitTimeRemaining = MAX_WAIT_TIME;
    _desiredConnections[_desiredConnectionCount].timeRemaining = random(MIN_CONNECTION_TIME, MAX_CONNECTION_TIME);

    Serial.print("New desired connection: ");
    Serial.print(plugA);
    Serial.print(" to ");
    Serial.println(plugB);

    _desiredConnectionCount++;
}

bool Game::IsPlugAlreadyInPlay(int plug)
{
    for(int i = 0; i < _desiredConnectionCount; i++)
    {
        if(_desiredConnections[i].includesPlug(plug))
        {
            return true;
        }
    }

    return false;
}

Game* Game::GetInstance()
{
    if(__instance == 0)
    {
        __instance = new Game();
    }

    return __instance;
}

void Game::Initialize(int rows, int columns, NeoPixelBusLg<NeoGrbFeature, Neo800KbpsMethod> strip, Patchboard patchboard)
{
    _rows = rows;
    _columns = columns;

    _strip = &strip;
    _patchboard = &patchboard;

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

    for(int i = _desiredConnectionCount-1; i >= 0; i--)
    {
        auto connection = &_desiredConnections[i];

        if(_patchboard->IsConnected(connection->plugA, connection->plugB))
        {
            // Logic for when the plugs are connected

            // Recharge the waiting time
            connection->waitTimeRemaining = min(MAX_WAIT_TIME, connection->waitTimeRemaining + deltaTime);

            // Decrement the time remaining
            connection->timeRemaining = max(0, connection->timeRemaining - deltaTime);

            // Check to see if we're done
            if(connection->timeRemaining == (unsigned long)0)
            {
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
            }
        }
        else
        {
            // Logic for when the plugs are disoconnected

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
        }

        if(_desiredConnectionCount < MAX_REQUESTED_CONNECION_COUNT)
        {
            _newConnectionDelay = max(0, _newConnectionDelay - deltaTime);

            if(_newConnectionDelay == 0)
            {
                _newConnectionDelay = MAX_CONNECTION_INTERVAL;

                GenerateNewConnectionRequest();
            }


        }


    }

    
}

void Game::BeginGame()
{
    _gameStartMillis = millis();
    _lastUpdateMillis = millis();
}

void Game::OnConnectionChanged(int plugA, int plugB, bool isConnected)
{

}
