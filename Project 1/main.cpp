/**

* Author: Mohammad Alyousef

* Assignment: Simple 2D Scene

* Date due: 10/05/2026
* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

#include "CS3113/cs3113.h"
#include <math.h>


// Global Constants
constexpr int   SCREEN_WIDTH  = 960 ,
                SCREEN_HEIGHT = 600 ,
                FPS           = 60,
                SIZE          = 200,
                TRACKSIZE     = 400,
                CARSIZE       = 50,
                TROPHYSIZE    = 50, 
                CROWDSIZE     = 100,
                FRAME_LIMIT   = 100,
                MAX_CAR       = 10;

constexpr float MAX_AMP       = 15.0f;

constexpr char    BG_COLOUR[] = "#000000";
constexpr char    READY_BG_COLOUR[] = "#ff0000";
constexpr char    SET_BG_COLOUR[] = "#f1de07";
constexpr char    GO_BG_COLOUR[] = "#089a00";

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 TRACK_ORIGIN      = { SCREEN_WIDTH /2 , SCREEN_HEIGHT - TRACKSIZE/2};
constexpr Vector2 CAR_ORIGIN      = { SCREEN_WIDTH /2 , SCREEN_HEIGHT - TRACKSIZE + CARSIZE};
constexpr Vector2 TROPHY_ORIGIN      = { SCREEN_WIDTH /2 , CAR_ORIGIN.y - 70};
constexpr Vector2 CROWD_ORIGIN      = { SCREEN_WIDTH /2 , 100};

constexpr Vector2 BASE_SIZE   = { static_cast<float>(SIZE), static_cast<float>(SIZE) };
constexpr Vector2 Track_BASE_SIZE   = { static_cast<float>(TRACKSIZE*2), static_cast<float>(TRACKSIZE) };
constexpr Vector2 CAR_BASE_SIZE   = { static_cast<float>(CARSIZE*2), static_cast<float>(CARSIZE) };
constexpr Vector2 TROPHY_BASE_SIZE   = { static_cast<float>(TROPHYSIZE), static_cast<float>(TROPHYSIZE) };
constexpr Vector2 CROWD_BASE_SIZE   = { static_cast<float>(CROWDSIZE*2), static_cast<float>(CROWDSIZE) };

constexpr char Race_Track[] = "assets/race_track.png";
constexpr char Race_Car[] = "assets/race_car.png";
constexpr char Trophy[] = "assets/trophy.png";
constexpr char Crowd[] = "assets/crowd.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
enum    traffic  {ready, set, go};

float   gScaleFactor    = SIZE,
        gAngle          = 0.0f,
        gPulseTime      = 0.0f;
float   gPreviousTicks  = 0.0f;

float   gCurCarAngle    = -90.0f,
        gCurTrophyAngle = -90.0f,
        gCurCrowdAngle  = 0.0f,
        gCarRadius      = TRACK_ORIGIN.y - CAR_ORIGIN.y,
        gTrophyRadius   = 70.0f,
        gCrowdRadius    = 50.0f;

          
Vector2 gPosition       = ORIGIN,
        gScale          = BASE_SIZE;

Vector2 gTrackPosition  = TRACK_ORIGIN,
        gTrackScale     = Track_BASE_SIZE;

Vector2 gCarPosition    = CAR_ORIGIN,
        gCarScale       = CAR_BASE_SIZE;
float   gCarAngle       = 0.0f;
traffic carStatus       = ready;
float   carCounter      = 0;
bool    gLeftStart      = true;

Vector2 gTrophyPosition = TROPHY_ORIGIN,
        gTrophyScale    = TROPHY_BASE_SIZE;

Vector2 gCrowdPosition  = CROWD_ORIGIN,
        gCrowdScale     = CROWD_BASE_SIZE;
float   gCrowdStatus    = 1.0f;

Vector2   gStartPoint = {0.0f,0.0f};
float gCurFrame = 0.0f;


Texture2D gTrackTexture;
Texture2D gCarTexture;
Texture2D gTrophyTexture;
Texture2D gCrowdTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures");

    gTrackTexture = LoadTexture(Race_Track);
    gCarTexture = LoadTexture(Race_Car);
    gTrophyTexture = LoadTexture(Trophy);
    gCrowdTexture = LoadTexture(Crowd);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
       // Delta time
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    if(gCrowdPosition.x >= SCREEN_WIDTH - CROWDSIZE && gCrowdStatus == 1.0f) gCrowdStatus *= -1;
    else if(gCrowdPosition.x <= CROWDSIZE && gCrowdStatus == -1.0f) gCrowdStatus *= -1;
    
    gCrowdPosition.x += 250*deltaTime * gCrowdStatus;
    gCrowdPosition.y = gCrowdRadius * sin(gCurCrowdAngle * 3.14/180) + CROWD_ORIGIN.y;
    gCurCrowdAngle += 250*deltaTime;
    

    gPulseTime += 10*deltaTime;
    gTrophyScale = {
        TROPHY_BASE_SIZE.x + MAX_AMP * cos(gPulseTime),
        TROPHY_BASE_SIZE.y + MAX_AMP * cos(gPulseTime)
    };
    gTrophyPosition.x = gTrophyRadius * cos(gCurTrophyAngle * 3.14/180) + gCarPosition.x;
    gTrophyPosition.y = gTrophyRadius * sin(gCurTrophyAngle * 3.14/180) + gCarPosition.y ;
    gCurTrophyAngle += 250*deltaTime;


    if(gCarPosition.x >= CAR_ORIGIN.x && gLeftStart == true) {
        carStatus = ready;
        gCarPosition.x = CAR_ORIGIN.x;
        gLeftStart = false;
    }

    if(carStatus == go){

        if(gCarPosition.x >= TRACK_ORIGIN.x - TRACKSIZE/2 && gCarPosition.x <= TRACK_ORIGIN.x + TRACKSIZE/2){
            if(gCarPosition.y <TRACK_ORIGIN.y){
                gCarPosition = {
                    gCarPosition.x+250*deltaTime,
                    gCarPosition.y
                };
                gCarAngle = 0.0f;
            }
            else{
                gCarPosition = {
                    gCarPosition.x-250*deltaTime,
                    gCarPosition.y
                };
                gCarAngle = 180.0f;
            }
        }
        else{
            if(gCarPosition.x > TRACK_ORIGIN.x + TRACKSIZE/2){
                gCarPosition.x = gCarRadius * cos(gCurCarAngle * 3.14/180) + TRACK_ORIGIN.x + TRACKSIZE/2;
                gCarPosition.y = gCarRadius * sin(gCurCarAngle * 3.14/180) + TRACK_ORIGIN.y ;
                gCurCarAngle += 100*deltaTime;
                gCarAngle += 100*deltaTime;
            }
            
            if(gCarPosition.x < TRACK_ORIGIN.x - TRACKSIZE/2){
                gCarPosition.x = gCarRadius * cos(gCurCarAngle * 3.14/180) + TRACK_ORIGIN.x - TRACKSIZE/2;
                gCarPosition.y = gCarRadius * sin(gCurCarAngle * 3.14/180) + TRACK_ORIGIN.y ;
                gCurCarAngle += 100*deltaTime; 
                gCarAngle += 100*deltaTime; 
                gLeftStart = true;
            }   
        }
    }
    else{
        if(carStatus == ready){
            carCounter += 10*deltaTime;
            if(carCounter >= MAX_CAR)carStatus = set, carCounter = 0;
        }
        else{
            carCounter += 10*deltaTime;
            if(carCounter >= MAX_CAR)carStatus = go, carCounter = 0;
        }
    }
    
}

void render()
{
    BeginDrawing();
    if(carStatus == ready){
        ClearBackground(ColorFromHex(READY_BG_COLOUR));
    }
    else if(carStatus == set){
        ClearBackground(ColorFromHex(SET_BG_COLOUR));
    }
    else{
        ClearBackground(ColorFromHex(GO_BG_COLOUR));
    }
    // track rendering
    Rectangle trackTextureArea = {
        // top-left corner
        0.0f, 0.0f,
        // bottom-right corner (of texture)
        static_cast<float>(gTrackTexture.width),
        static_cast<float>(gTrackTexture.height)
    };
    // Destination rectangle – centred on gPosition
    Rectangle trackDestinationArea = {
        gTrackPosition.x,
        gTrackPosition.y,
        static_cast<float>(gTrackScale.x),
        static_cast<float>(gTrackScale.y)
    };
    // Origin inside the source texture (centre of the texture)
    Vector2 trackObjectOrigin = {
        static_cast<float>(gTrackScale.x) / 2.0f,
        static_cast<float>(gTrackScale.y) / 2.0f
    };

    // car rendering
    Rectangle carTextureArea = {
        0.0f, 0.0f,

        static_cast<float>(gCarTexture.width),
        static_cast<float>(gCarTexture.height)
    };
    Rectangle carDestinationArea = {
        gCarPosition.x,
        gCarPosition.y,
        static_cast<float>(gCarScale.x),
        static_cast<float>(gCarScale.y)
    };
    Vector2 carObjectOrigin = {
        static_cast<float>(gCarScale.x) / 2.0f,
        static_cast<float>(gCarScale.y) / 2.0f
    };

    // Trophy Rendering
     Rectangle trophyTextureArea = {
        0.0f, 0.0f,

        static_cast<float>(gTrophyTexture.width),
        static_cast<float>(gTrophyTexture.height)
    };
    Rectangle trophyDestinationArea = {
        gTrophyPosition.x,
        gTrophyPosition.y,
        static_cast<float>(gTrophyScale.x),
        static_cast<float>(gTrophyScale.y)
    };
    Vector2 trophyObjectOrigin = {
        static_cast<float>(gTrophyScale.x) / 2.0f,
        static_cast<float>(gTrophyScale.y) / 2.0f
    };

    // Crowd Rendering
     Rectangle crowdTextureArea = {
        0.0f, 0.0f,

        static_cast<float>(gCrowdTexture.width),
        static_cast<float>(gCrowdTexture.height)
    };
    Rectangle crowdDestinationArea = {
        gCrowdPosition.x,
        gCrowdPosition.y,
        static_cast<float>(gCrowdScale.x),
        static_cast<float>(gCrowdScale.y)
    };
    Vector2 crowdObjectOrigin = {
        static_cast<float>(gCrowdScale.x) / 2.0f,
        static_cast<float>(gCrowdScale.y) / 2.0f
    };


    // Render the texture on screen
    DrawTexturePro(
        gTrackTexture,
        trackTextureArea,
        trackDestinationArea,
        trackObjectOrigin,
        gAngle,
        WHITE
    );
    DrawTexturePro(
        gCarTexture,
        carTextureArea,
        carDestinationArea,
        carObjectOrigin,
        gCarAngle,
        WHITE
    );
    DrawTexturePro(
        gCrowdTexture,
        crowdTextureArea,
        crowdDestinationArea,
        crowdObjectOrigin,
        gAngle,
        WHITE
    );
    DrawTexturePro(
        gTrophyTexture,
        trophyTextureArea,
        trophyDestinationArea,
        trophyObjectOrigin,
        gAngle,
        WHITE
    );
    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gTrackTexture);
    UnloadTexture(gCarTexture);
    UnloadTexture(gCrowdTexture);
    UnloadTexture(gTrophyTexture);
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