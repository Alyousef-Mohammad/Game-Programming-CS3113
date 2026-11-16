/**
* Author: Mohammad Alyousef
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Win.h"

Win::Win()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

Win::Win(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

Win::~Win() { shutdown(); }


void Win::initialise()
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
        "assets/win.png",
        NONE
    );
}


void Win::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

  
    if (IsKeyPressed(KEY_ENTER))
        mGameState.nextSceneID = MENU;
}

void Win::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    screen->render();
}

void Win::shutdown()
{
    delete mGameState.player;
    mGameState.player=nullptr;
    delete screen;
    screen = nullptr;

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};

}