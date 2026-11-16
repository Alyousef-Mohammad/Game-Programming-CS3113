/**
* Author: Mohammad Alyousef
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelC.h"

LevelC::LevelC()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelC::LevelC(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelC::~LevelC() { shutdown(); }

extern int glives;
void LevelC::initialise()
{
    mGameState.nextSceneID = NO_SCENE;

    mGameState.bgm = LoadMusicStream("assets/music.wav");
    SetMusicVolume(mGameState.bgm, 0.15f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/jump.wav");
    SetSoundVolume(mGameState.jumpSound, 0.25f);

    key = LoadSound("assets/key.wav");
    SetSoundVolume(key, 0.25f);


    /*
        ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT, // map grid cols & rows
        mLevelData,                // grid data
        "assets/tileset.png", // texture filepath
        LevelA::TILE_DIMENSION,            // tile size
        13, 1,                     // texture cols & rows
        mOrigin                    // in-game origin
    );

    /*
        ----------- PROTAGONIST -----------
    */

    std::map<Direction, std::vector<int>> playerAnimationAtlas = {
        { RIGHT, { 0, 1,  2,  3,  4,  5,  6,  7 } },
        { LEFT,  { 8, 9, 10, 11, 12, 13, 14, 15 } },
    };

    Vector2 spawn = {
        mGameState.map->getLeftBoundary() + (6 + 0.5f) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary()  + 3 * LevelA::TILE_DIMENSION - 0.75f * LevelA::TILE_DIMENSION
    };

    mGameState.player = new Entity(
        spawn,                                            // position
        { 1.0f * LevelA::TILE_DIMENSION, 1.5f * LevelA::TILE_DIMENSION },  // scale (1 x 1.5 tiles)
        "assets/player.png",                         // texture file address
        ATLAS,                                            // single image or atlas?
        { 2, 8 },                                         // atlas: 2 rows, 8 columns
        playerAnimationAtlas,                             // actual atlas
        PLAYER                                            // entity type
    );

    mGameState.player->setJumpingPower(700.0f); // ~3.3 tiles high
    mGameState.player->setColliderDimensions({
        0.95f * LevelA::TILE_DIMENSION,
        1.45f * LevelA::TILE_DIMENSION
    });
    mGameState.player->setAcceleration({ 0.0f, LevelA::ACCELERATION_OF_GRAVITY });

    //      
    //      ghost
    std::map<Direction, std::vector<int>> ghostAtlas = {
        { RIGHT, { 0, 1} },
        { LEFT,  { 2, 3} }
    };
    Vector2 ghostspawn = {
        mGameState.map->getLeftBoundary() + (1 + 0.5f) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary() + (1 + 0.5f) * LevelA::TILE_DIMENSION
    };
    ghost = new Entity(
        ghostspawn,
        {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
        "assets/ghost.png",
        ATLAS,
        {2,2},
        ghostAtlas,
        NPC
    );
    ghost->setAcceleration({0.0f,0.0f});
    ghost->setSpeed(100);
    ghost->setFrameSpeed(4);
    ghost->setAIType(FOLLOWER);

    //   spikes
   
    for(int i=0;i<2;i++){
        spikes1[i] = new Entity(
            {mGameState.map->getLeftBoundary() + (8 + i + 0.5f) * LevelA::TILE_DIMENSION,
            mGameState.map->getTopBoundary() + (14 + 0.5f) * LevelA::TILE_DIMENSION},
            {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
            "assets/spikes.png",
            NONE
        );
        spikes2[i] = new Entity(
            {mGameState.map->getLeftBoundary() + (13 + i + 0.5f) * LevelA::TILE_DIMENSION,
            mGameState.map->getTopBoundary() + (20 + 0.5f) * LevelA::TILE_DIMENSION},
            {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
            "assets/spikes.png",
            NONE
        );
    }
    //      keys
    keys[0] = new Entity(
        {mGameState.map->getLeftBoundary() + (1 + 0.5f) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary() + (6 + 0.5f) * LevelA::TILE_DIMENSION},
        {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
        "assets/key.png",
        NONE
    );
    keys[1] = new Entity(
        {mGameState.map->getLeftBoundary() + (14 + 0.5f) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary() + (13 + 0.5f) * LevelA::TILE_DIMENSION},
        {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
        "assets/key.png",
        NONE
    );
    keys[2] = new Entity(
        {mGameState.map->getLeftBoundary() + (3 + 0.5f) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary() + (23 + 0.5f) * LevelA::TILE_DIMENSION},
        {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
        "assets/key.png",
        NONE
    );
    // doors
    for (int i=0;i<3;i++){
        doors[i*2] = new Entity(
            {mGameState.map->getLeftBoundary() + (9 + 2*i + 0.5f) * LevelA::TILE_DIMENSION,
            mGameState.map->getTopBoundary() + (23 + 0.5f) * LevelA::TILE_DIMENSION},
            {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
            "assets/prison.png",
            BLOCK
        ); 
        doors[i*2+1]= new Entity(
            {mGameState.map->getLeftBoundary() + (9 + 2*i + 0.5f) * LevelA::TILE_DIMENSION,
            mGameState.map->getTopBoundary() + (24 + 0.5f) * LevelA::TILE_DIMENSION},
            {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
            "assets/prison.png",
            BLOCK
        ); 
    }
    //          GOAL
    Vector2 starspawn = {
        mGameState.map->getLeftBoundary() + (14 + 0.5f) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary() + (24 + 0.5f) * LevelA::TILE_DIMENSION
    };

    mGameState.star = new Entity(
        starspawn,
        {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
        "assets/star.png",
        NONE
    );
}

void LevelC::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    mGameState.player->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );
    ghost->update(
        deltaTime,
        mGameState.player,
        nullptr,
        nullptr,
        0
    );
    
    for (int i = 0; i < 6; i++)
    if (mGameState.player->isColliding(doors[i]))   // skips opened (inactive) doors
    {
        Vector2 p = mGameState.player->getPosition();
        p.x = doors[i]->getPosition().x - LevelA::TILE_DIMENSION / 2      // door's left edge
                                        - 0.95f * LevelA::TILE_DIMENSION / 2;  // minus half his width
        mGameState.player->setPosition(p);
    }

    bool spikestouched = false;
    for(int i=0;i<2;i++){
        if(mGameState.player->isColliding(spikes1[i])){
            spikestouched = true;
        }
        if(mGameState.player->isColliding(spikes2[i])){
            spikestouched = true;
        }
    }
    
    if(mGameState.player->isColliding(ghost) || spikestouched){
        glives--;
        if(glives <= 0){
            mGameState.nextSceneID = LOSS;
        }
        else{
            mGameState.nextSceneID = LEVEL_C;
        }
        spikestouched = false;
        for(int i=0;i<3;i++) keys[i]->activate();
        keysleft = 3;
    }
    for(int i=0;i<3;i++){
        if(mGameState.player->isColliding(keys[i]) && keys[i]->isActive()){
            keys[i]->deactivate();
            keysleft--;
            doors[i*2]->deactivate();
            doors[i*2+1]->deactivate();
            PlaySound(key);
        }
    }


    Rectangle thegoal  = {
        mGameState.map->getLeftBoundary() + (14) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary()  + (24) * LevelA::TILE_DIMENSION,
        LevelA::TILE_DIMENSION,
        LevelA::TILE_DIMENSION    
    };

    if (CheckCollisionPointRec(mGameState.player->getPosition(), thegoal))
        mGameState.nextSceneID = WIN;
}

void LevelC::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.star->render();
    ghost->render();
    for(int i=0;i<2;i++){spikes1[i]->render();spikes2[i]->render();}
    for(int i=0;i<3;i++){keys[i]->render();}
    for(int i=0;i<6;i++){doors[i]->render();}
    mGameState.player->render();
}

void LevelC::shutdown()
{
    delete mGameState.player;
    mGameState.player = nullptr;
    for(int i=0;i<2;i++){delete spikes1[i];spikes1[i] = nullptr;
                         delete spikes2[i];spikes2[i] = nullptr;}

    for(int i=0;i<3;i++){delete keys[i];keys[i] = nullptr;}
    for(int i=0;i<6;i++){delete doors[i];doors[i] = nullptr;}
    delete ghost;
    ghost = nullptr;
    delete mGameState.star;
    mGameState.star = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};
    UnloadSound(mGameState.jumpSound);
    mGameState.jumpSound = {};
    UnloadSound(key);
    key = {};
}