/**

* Author: Mohammad Alyousef

* Assignment: Lunar Lander!

* Date due: [10/30/2026]

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

#include "CS3113/Entity.h"

// Global Constants
enum GameState {NOTYET, WIN, LOSE};
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr char    BG_COLOUR[] = "#eededb";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr int   NUMBER_OF_RED_PIPES     = 2;
constexpr float PIPE_DIMENSION          = 100.0f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 10.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f,
                END_GAME_THRESHOLD      = 800.0f,
                MAX_FUEL = 100.0f;

// Global Variables
AppStatus gAppStatus       = RUNNING;
GameState gGameState = NOTYET;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f,
          gDecayFactor = 50.0f;

Entity *gIronMan = nullptr;
Entity *gGPipe   = nullptr;
Entity *gRPipes  = nullptr;
Entity *gLoss = nullptr, *gWin = nullptr;
Entity *gBackground = nullptr;
Entity *gFuelFrame = nullptr, *gFuelBar = nullptr; 
Entity *gFlame = nullptr;

bool gLaunched = false, gMoving = false;
float gFuel = 100.0f;
// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "IronMan Lunar");

    /*
        ----------- PROTAGONIST -----------
    */
   gBackground = new Entity(
       {ORIGIN.x, ORIGIN.y},
       {SCREEN_WIDTH, SCREEN_HEIGHT},
       "assets/background.png",
       NONE
    );
    
    gFuelFrame = new Entity(
        {500, 30},
        {200, 40},
        "assets/fuel-frame.png",
        NONE
    );
    gFuelBar = new Entity(
        {500, 35},
        {162, 6},
        "assets/fuel-bar.png",
        NONE
    );
    
    gLoss = new Entity(
        {ORIGIN.x,ORIGIN.y},
        {SCREEN_WIDTH,SCREEN_HEIGHT},
        "assets/lose-screen.png",
        NONE
    );
    gWin = new Entity(
        {ORIGIN.x,ORIGIN.y},
        {SCREEN_WIDTH,SCREEN_HEIGHT},
        "assets/win-screen.png",
        NONE
    );
    
    float sizeRatio = 200.0f / 397.0f;
    
    gIronMan = new Entity(
        { ORIGIN.x , ORIGIN.y - 200.0f }, // position
        { 80.0f * sizeRatio, 80.0f },           // scale
        "assets/iron-man.png",                   // texture file address
        PLAYER                                    // entity type
    );
    
    gIronMan->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    
    std::map<Direction, std::vector<int>> flameAtlas = {
        { RIGHT, { 0,1,2,3,4,5 } }
    };
    
    gFlame = new Entity(
        {ORIGIN.x, ORIGIN.y - 200.0f + 56.0f},
        {16.0f,32.0f},
        "assets/flame-atlas.png",
        ATLAS,
        {1,6},
        flameAtlas,
        NONE
    );
    /*
        ----------- PIPES -----------
    */
    gGPipe = new Entity(
        {ORIGIN.x, SCREEN_HEIGHT - PIPE_DIMENSION/2},
        {PIPE_DIMENSION, PIPE_DIMENSION},
        "assets/green-pipe.png",
        PLATFORM
    );
    gGPipe->setVelocity({170.0f,0.0f});


    gRPipes = new Entity[NUMBER_OF_RED_PIPES];


    for (int i = 0; i < NUMBER_OF_RED_PIPES; i++)
    {
        gRPipes[i].setTexture("assets/red-pipe.png");
        gRPipes[i].setEntityType(PLATFORM);
        gRPipes[i].setScale({ PIPE_DIMENSION, PIPE_DIMENSION });
        gRPipes[i].setColliderDimensions({PIPE_DIMENSION,PIPE_DIMENSION});
        gRPipes[i].setPosition({
            100.f,
            SCREEN_HEIGHT - PIPE_DIMENSION/2
        });
        gRPipes[i].setVelocity({120.0f * (i+1),0.0f});
    }
    gRPipes[1].setPosition({SCREEN_WIDTH-100.0f,SCREEN_HEIGHT-PIPE_DIMENSION/2});

    

    SetTargetFPS(FPS);
}

void processInput()
{
    if(IsKeyPressed(KEY_SPACE)) gLaunched = true;
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    if(!gLaunched || gGameState != NOTYET)return;

    if      (IsKeyDown(KEY_A) && gFuel>0) {gIronMan->setAcceleration( { -400.0f, ACCELERATION_OF_GRAVITY }); gMoving = true;}
    else if (IsKeyDown(KEY_D) && gFuel>0) {gIronMan->setAcceleration( { 400.0f, ACCELERATION_OF_GRAVITY }); gMoving = true;}
    else {gIronMan->accelerationDecay(); gMoving = false;}

    // to avoid faster diagonal speed
    // if (GetLength(gIronMan->getMovement()) > 1.0f)
    //     gIronMan->normaliseMovement();

}

void update()
{
    // Delta time
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    if(gGameState!=NOTYET) return;
    
    // Fixed timestep
    deltaTime += gTimeAccumulator;
    
    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }
    
    while(deltaTime >= FIXED_TIMESTEP){
        if(gLaunched){
            if(gMoving == true){
                gFuel = fmaxf(gFuel- 30*FIXED_TIMESTEP, 0.0f);
                gFuelBar->setScale({162.0f * gFuel/100.0f, 6});
            }
            
            gIronMan->update(FIXED_TIMESTEP, gGPipe, 1, gRPipes, NUMBER_OF_RED_PIPES);
            if(gMoving){
                gFlame->setPosition({gIronMan->getPosition().x, gIronMan->getPosition().y+56});
                gFlame->update(FIXED_TIMESTEP, nullptr,0,nullptr,0);
            }
        }




        if(gIronMan->getGreenCollide() && gIronMan->isCollidingBottom()){
            gGameState = WIN;
        }
        else if(gIronMan->getRedCollide() || gIronMan->getGreenCollide()){
            gGameState = LOSE;
        }
        float gvx = gGPipe->getVelocity().x;
        if(gvx>0 && gGPipe->getPosition().x + gGPipe->getScale().x/2 >= SCREEN_WIDTH){
            gGPipe->setVelocity({-gvx,0});
        }
        else if(gvx<0 && gGPipe->getPosition().x - gGPipe->getScale().x/2 <= 0){
            gGPipe->setVelocity({-gvx,0});
        }
        gGPipe->update(FIXED_TIMESTEP, nullptr, 0, nullptr,0);
        
        
        for(int i=0;i<NUMBER_OF_RED_PIPES; i++){
            float rvx = gRPipes[i].getVelocity().x;
            if(rvx>0 && gRPipes[i].getPosition().x + gRPipes[i].getScale().x/2 >= SCREEN_WIDTH){
                gRPipes[i].setVelocity({-rvx,0});
            }
            else if(rvx<0 && gRPipes[i].getPosition().x - gRPipes[i].getScale().x/2 <= 0){
                gRPipes[i].setVelocity({-rvx,0});
            }
            gRPipes[i].update(FIXED_TIMESTEP, nullptr, 0, nullptr,0);
        }
        deltaTime -= FIXED_TIMESTEP;
    }

    gTimeAccumulator = deltaTime;

    if (gIronMan->getPosition().y > END_GAME_THRESHOLD) gGameState = LOSE;
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    gBackground->render();
    gFuelFrame->render();
    gFuelBar->render();

    if (!gLaunched)
    {
        const char *msg = "Press SPACE to launch";
        int fontSize    = 30;
        int textWidth   = MeasureText(msg, fontSize);

        DrawText(msg, SCREEN_WIDTH / 2 - textWidth / 2, SCREEN_HEIGHT / 2 - fontSize / 2, fontSize, WHITE);
    }

    gIronMan->render();
    if(gMoving)
        gFlame->render();
    gGPipe->render();

    for (int i = 0; i < NUMBER_OF_RED_PIPES; i++)  gRPipes[i].render();

    if(gGameState == LOSE){
        gLoss->render();
    }
    else if (gGameState == WIN){
        gWin->render();
    }
    EndDrawing();
}

void shutdown()
{
    // Unload the textures while the window (GL context) still exists
    delete gIronMan;
    delete gGPipe;
    delete[] gRPipes;
    delete gFuelBar;
    delete gFuelFrame;
    delete gBackground;
    delete gWin;
    delete gLoss;
    delete gFlame;
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
