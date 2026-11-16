/**
* Author: Mohammad Alyousef
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelB.h"

LevelB::LevelB()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelB::LevelB(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelB::~LevelB() { shutdown(); }


extern int glives;
void LevelB::initialise()
{
    mGameState.nextSceneID = NO_SCENE;

    mGameState.bgm = LoadMusicStream("assets/music.wav");
    SetMusicVolume(mGameState.bgm, 0.15f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/jump.wav");
    SetSoundVolume(mGameState.jumpSound, 0.25f);

    updraft = LoadSound("assets/updraft.wav");
    SetSoundVolume(updraft, 0.25f);

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
    // Knight by Mattz Art -- 8-frame run, right on row 0, left on row 1
    std::map<Direction, std::vector<int>> playerAnimationAtlas = {
        { RIGHT, { 0, 1,  2,  3,  4,  5,  6,  7 } },
        { LEFT,  { 8, 9, 10, 11, 12, 13, 14, 15 } },
    };

    // spawn: column 3, standing on the ground in row 25
    Vector2 spawn = {
        mGameState.map->getLeftBoundary() + (3 + 0.5f) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary()  + 25 * LevelA::TILE_DIMENSION - 0.75f * LevelA::TILE_DIMENSION
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

    //  ghost
    std::map<Direction, std::vector<int>> ghostAtlas = {
        { RIGHT, { 0, 1} },
        { LEFT,  { 2, 3} }
    };
    Vector2 ghostspawn = {
        mGameState.map->getLeftBoundary() + (14 + 0.5f) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary() + (4 + 0.5f) * LevelA::TILE_DIMENSION
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
   
    for(int i=0;i<9;i++){
        spikes[i] = new Entity(
            {mGameState.map->getLeftBoundary() + (1 + i + 0.5f) * LevelA::TILE_DIMENSION,
            mGameState.map->getTopBoundary() + (17 + 0.5f) * LevelA::TILE_DIMENSION},
            {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
            "assets/spikes.png",
            NONE
        );
    }
    
    //   updraft
    Vector2 updraftspawn = {
        mGameState.map->getLeftBoundary() + (13 + 0.5f) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary() + (20 + 0.5f) * LevelA::TILE_DIMENSION
    };
    
    mUpdraft = new Entity(
        updraftspawn,
        {LevelA::TILE_DIMENSION * 3, LevelA::TILE_DIMENSION * 5},
        "assets/updraft.png",
        NONE
    );
    
    //          GOAL
    Vector2 starspawn = {
        mGameState.map->getLeftBoundary() + (1 + 0.5f) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary() + (3 + 0.5f) * LevelA::TILE_DIMENSION,
    };
    
    mGameState.star = new Entity(
        starspawn,
        {LevelA::TILE_DIMENSION, LevelA::TILE_DIMENSION},
        "assets/star.png",
        NONE
    );
}

void LevelB::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    mGameState.player->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );


    Rectangle pipeEntrance = {
        mGameState.map->getLeftBoundary() + 1  * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary()  + 14 * LevelA::TILE_DIMENSION,
        2 * LevelA::TILE_DIMENSION,
        1 * LevelA::TILE_DIMENSION
    };

    if (CheckCollisionPointRec(mGameState.player->getPosition(), pipeEntrance)){
        mGameState.player->setPosition({
        mGameState.map->getLeftBoundary() + 12 * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary()  + 8  * LevelA::TILE_DIMENSION - 0.75f * LevelA::TILE_DIMENSION});
        pipeentered = true;
    }
    if(pipeentered){
        ghost->update(
            deltaTime,
            mGameState.player,
            nullptr,
            nullptr,
            0
        );
    }
    bool spikestouched = false;
    for(int i=0;i<9;i++){
        if(mGameState.player->isColliding(spikes[i])){
            spikestouched = true;
        }
    }
    
    if(mGameState.player->isColliding(ghost) || spikestouched){
        glives--;
        if(glives <= 0){
            mGameState.nextSceneID = LOSS;
        }
        else{
            mGameState.nextSceneID = LEVEL_B;
        }
        spikestouched = false;
        pipeentered = false;
    }


    // Goal: trophies at tiles (1, 3) and (2, 3)
    Rectangle thegoal  = {
        mGameState.map->getLeftBoundary() + (1) * LevelA::TILE_DIMENSION,
        mGameState.map->getTopBoundary()  + (3) * LevelA::TILE_DIMENSION,
        LevelA::TILE_DIMENSION,
        LevelA::TILE_DIMENSION    
    };

    Rectangle theupdraft = {
    mGameState.map->getLeftBoundary() + 12 * LevelA::TILE_DIMENSION,
    mGameState.map->getTopBoundary()  + 18 * LevelA::TILE_DIMENSION,
    LevelA::TILE_DIMENSION * 3,
    LevelA::TILE_DIMENSION * 5
};

 
    if (CheckCollisionPointRec(mGameState.player->getPosition(), theupdraft)){
        Vector2 v = mGameState.player->getVelocity();
        v.y = -1000.0f;                     
        mGameState.player->setVelocity(v); 
        PlaySound(updraft);
    }

    if (CheckCollisionPointRec(mGameState.player->getPosition(), thegoal))
        mGameState.nextSceneID = LEVEL_C;
}

void LevelB::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    for(int i=0;i<9;i++) spikes[i]->render();
    mGameState.star->render();
    mUpdraft->render();
    ghost->render();
    mGameState.player->render();
}

void LevelB::shutdown()
{
    delete mGameState.player;
    mGameState.player = nullptr;
    delete ghost;
    ghost = nullptr;
    delete mUpdraft;
    mUpdraft = nullptr;
    // delete [] spikes;
    for(int i=0;i<9;i++) {delete spikes[i]; spikes[i] = nullptr;}
    delete mGameState.map;
    mGameState.map = nullptr;

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};
    UnloadSound(mGameState.jumpSound);
    mGameState.jumpSound = {};
    UnloadSound(updraft);
    updraft = {};
}