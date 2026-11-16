/**
* Author: Mohammad Alyousef
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/Menu.h"
#include "CS3113/Loss.h"
#include "CS3113/Win.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f;

int       glives           = 3;
Camera2D gCamera = {};

Scene                     *gCurrentScene = nullptr;
std::map<SceneID, Scene *> gLevels       = {};

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{
    if (gCurrentScene) gCurrentScene->shutdown();
    gCurrentScene = scene;
    gCurrentScene->initialise();
    gCamera.target = {ORIGIN.x , gCurrentScene->getState().player->getPosition().y};
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();
    
    gLevels[MENU] = new Menu(ORIGIN, "#94C6E4");
    gLevels[LEVEL_A] = new LevelA(ORIGIN, "#94C6E4");
    gLevels[LEVEL_B] = new LevelB(ORIGIN, "#94C6E4");
    gLevels[LEVEL_C] = new LevelC(ORIGIN, "#94C6E4");
    gLevels[WIN] = new Win(ORIGIN, "#94C6E4");
    gLevels[LOSS] = new Loss(ORIGIN, "#94C6E4");

    switchToScene(gLevels[MENU]);

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1000.0f / 1200.0f;

    SetTargetFPS(FPS);
}

void processInput()
{
    gCurrentScene->getState().player->resetMovement();

    if      (IsKeyDown(KEY_A)) gCurrentScene->getState().player->moveLeft();
    else if (IsKeyDown(KEY_D)) gCurrentScene->getState().player->moveRight();

    if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE))&&
        gCurrentScene->getState().player->isCollidingBottom())
    {
        gCurrentScene->getState().player->jump();
        PlaySound(gCurrentScene->getState().jumpSound);
    }

    if  (IsKeyPressed(KEY_ONE)) switchToScene(gLevels[LEVEL_A]);
    if  (IsKeyPressed(KEY_TWO)) switchToScene(gLevels[LEVEL_B]);
    if  (IsKeyPressed(KEY_THREE)) switchToScene(gLevels[LEVEL_C]);
    // to avoid faster diagonal speed
    if (GetLength(gCurrentScene->getState().player->getMovement()) > 1.0f)
        gCurrentScene->getState().player->normaliseMovement();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    // Delta time
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        Vector2 currentPlayerPosition = {
            ORIGIN.x,
            gCurrentScene->getState().player->getPosition().y
        };

        panCamera(&gCamera, &currentPlayerPosition);
    }

    gTimeAccumulator = deltaTime;
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);

    gCurrentScene->render();

    EndMode2D();
    EndDrawing();
}

void shutdown()
{
    for (std::pair<const SceneID, Scene *> &entry : gLevels)
        delete entry.second;

    gLevels.clear();

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID != NO_SCENE)
        {
            SceneID id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}
