/**
* Author: Mohammad Alyousef
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Loss.h"

Loss::Loss()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

Loss::Loss(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

Loss::~Loss() { shutdown(); }



void Loss::initialise()
{
    mGameState.bgm = LoadMusicStream("assets/music.wav");
    SetMusicVolume(mGameState.bgm, 0.15f);
    PlayMusicStream(mGameState.bgm);

    
    
    mGameState.nextSceneID = NO_SCENE;

    mGameState.player = new Entity();          
    mGameState.player->setPosition(mOrigin);  
    screen = new Entity(
        mOrigin,                 // centre of the window
        { 1200.0f, 720.0f },     // full window size
        "assets/loss.png",
        NONE
    );
}


void Loss::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

  
    if (IsKeyPressed(KEY_ENTER))
        mGameState.nextSceneID = MENU;
}

void Loss::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    screen->render();
}

void Loss::shutdown()
{
    delete mGameState.player;
    mGameState.player=nullptr;
    delete screen;
    screen = nullptr;

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};

}