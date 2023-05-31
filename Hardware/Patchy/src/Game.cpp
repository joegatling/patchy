#include "Game.h"


Game::Game(int rows, int columns, NeoPixelBusLg<NeoGrbFeature, Neo800KbpsMethod> strip)
{
    _rows = rows;
    _columns = columns;

    _strip = &strip;
}

void Game::Initialize()
{
}

void Game::Update()
{
}

void Game::BeginGame()
{
    _gameStartMillis = millis();
}

void Game::OnPlugConnected(int x, int y)
{
}

void Game::OnPlugDisconnected(int x, int y)
{
}
