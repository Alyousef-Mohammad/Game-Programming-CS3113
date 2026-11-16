/**
* Author: Mohammad Alyousef
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelA.h"

LevelA::LevelA()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelA::LevelA(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelA::~LevelA() { shutdown(); }


extern int glives;

void LevelA::initialise()
{
    mGameState.nextSceneID = NO_SCENE;

    // TODO: your own music + jump sound (lecture audio isn't allowed)
    mGameState.bgm = LoadMusicStream("assets/music.wav");
    SetMusicVolume(mGameState.bgm, 0.15f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/jump.wav");
    SetSoundVolume(mGameState.jumpSound, 0.25f);

    /*
        ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT, // map grid cols & rows
        mLevelData,                // grid data
        "assets/tileset.png",      // texture filepath
        TILE_DIMENSION,            // tile size
        13, 1,                     // texture cols & rows
        mOrigin                    // in-game origin
    );

    /*
        ----------- PROTAGONIST -----------
    */
    // TODO: swap in your player sprite sheet + its frame indices
    std::map<Direction, std::vector<int>> playerAnimationAtlas = {
        { RIGHT, { 0, 1, 2,  3,  4,  5,  6,  7 } },
        { LEFT,  { 8, 9, 10, 11, 12, 13, 14, 15 } },
    };

    // spawn: player fills tile (3, 23) and the tile below it
    Vector2 playerspawn = {
        mGameState.map->getLeftBoundary() + (3 + 0.5f) * TILE_DIMENSION,  // column 3
        mGameState.map->getTopBoundary()  + (25) * TILE_DIMENSION  // row 23
    };    

    mGameState.player = new Entity(
        playerspawn,                                            // position
        { 1.0f * TILE_DIMENSION, 1.5f * TILE_DIMENSION }, 
        "assets/player.png",                              // texture file address
        ATLAS,                                            // single image or atlas?
        { 2, 8 },                                         // atlas dimensions
        playerAnimationAtlas,                            // actual atlas
        PLAYER                                            // entity type
    );

    mGameState.player->setJumpingPower(700.0f); // ~3.3 tiles high
    mGameState.player->setColliderDimensions({
        0.95f * TILE_DIMENSION,
        1.45f * TILE_DIMENSION
    });
    mGameState.player->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });



    //          AI

    std::map<Direction, std::vector<int>> enemyAtlas = {
        { RIGHT, { 0, 1 } },
        { LEFT,  { 2, 3 } },
    };

    Vector2 wormspawn = {
        mGameState.map->getLeftBoundary() + (3) * TILE_DIMENSION,  // column 3
        mGameState.map->getTopBoundary()  + (13 + 0.5f) * TILE_DIMENSION  // row 23
    }; 
    worm = new Entity(
        wormspawn,
        { LevelA::TILE_DIMENSION, 0.75f * LevelA::TILE_DIMENSION },
        "assets/worm.png",
        ATLAS,
        { 2, 2 },          // 2 rows, 2 columns
        enemyAtlas,
        NONE
    );
    worm->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    worm->setFrameSpeed(4);  
    worm->setSpeed(100);

    std::map<Direction, std::vector<int>> flyAtlas = {
        { RIGHT, { 2, 3 } },
        { UP,   { 2, 3 } },
        { DOWN, { 2, 3 } },
    };

    Vector2 flyspawn = {
        mGameState.map->getLeftBoundary() + (11.5f) * TILE_DIMENSION,  // column 3
        mGameState.map->getTopBoundary()  + (6.5f) * TILE_DIMENSION  // row 23
    }; 
    fly = new Entity(
        flyspawn,
        { LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION },
        "assets/fly.png",
        ATLAS,
        { 2, 2 },          // 2 rows, 2 columns
        flyAtlas,
        NONE
    );
    fly->setDirection(UP);
    fly->setAcceleration({ 0.0f, 0.0f });
    fly->setFrameSpeed(4);  
    fly->setSpeed(100);
    //          GOAL
    Vector2 starspawn = {
        mGameState.map->getLeftBoundary() + (7 + 0.5f) * TILE_DIMENSION,
        mGameState.map->getTopBoundary() + (3 + 0.5f) * TILE_DIMENSION
    };

    mGameState.star = new Entity(
        starspawn,
        {TILE_DIMENSION, TILE_DIMENSION},
        "assets/star.png",
        NONE
    );

    
}

void LevelA::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    mGameState.player->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    if(mGameState.player->isColliding(worm) || mGameState.player->isColliding(fly)){
        glives--;
        if(glives <= 0){
            mGameState.nextSceneID = LOSS;
        }
        else{
            mGameState.nextSceneID = LEVEL_A;
        }
    }

    if(worm->getPosition().x >= mGameState.map->getLeftBoundary() + (6) * TILE_DIMENSION) wormgoingright = false;
    if(worm->getPosition().x <= mGameState.map->getLeftBoundary() + (2) * TILE_DIMENSION) wormgoingright = true;

    if(wormgoingright) worm->moveRight();
    else worm->moveLeft();

    worm->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    if(fly->getPosition().y >= mGameState.map->getTopBoundary() + (8.5) * TILE_DIMENSION) flygoingup = true;
    if(fly->getPosition().y <= mGameState.map->getTopBoundary() + (4.5) * TILE_DIMENSION) flygoingup = false;

    if(flygoingup) fly->moveUp(); 
    else fly->moveDown();

    fly->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        nullptr, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    Rectangle thegoal  = {
        mGameState.map->getLeftBoundary() + (7) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary()  + (3) * LevelA::TILE_DIMENSION,
        TILE_DIMENSION,
        TILE_DIMENSION    
    };

    if (CheckCollisionPointRec(mGameState.player->getPosition(), thegoal))
        mGameState.nextSceneID = LEVEL_B;
}

void LevelA::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    worm->render();
    fly->render();
    mGameState.player->render();
    mGameState.star->render();
}

void LevelA::shutdown()
{
    delete mGameState.player;
    mGameState.player = nullptr;
    delete mGameState.star;
    mGameState.star = nullptr;
    delete worm;
    worm = nullptr;
    delete fly;
    fly = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};
    UnloadSound(mGameState.jumpSound);
    mGameState.jumpSound = {};
}