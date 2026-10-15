/**

* Author: Mohammad Alyousef

* Assignment: Pong Clone

* Date due: 10/16/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

#include "CS3113/cs3113.h"
#include <math.h>

// Global Constants
enum Players  {PVP, PVE};
enum Won {notyet, player1, player2, draw};
constexpr int SCREEN_WIDTH  = 1600 / 2,
              SCREEN_HEIGHT = 900 / 2,
              FPS           = 60,
              SIZE          = 500 / 2,
              SPEED         = 200,
              BALL_SPEED    = 125,
              WIN_SCORE     = 6;

constexpr char    BG_COLOUR[]    = "#07480a";
constexpr Vector2 ORIGIN         = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  STICK_BASE_SIZE      = { 8.0f, 150.0f },
                  BALL_BASE_SIZE      = { 20.0f, 20.0f },
                  BALL7_INIT_POS       = {ORIGIN.x, ORIGIN.y-50},
                  BALL8_INIT_POS       = {ORIGIN.x, ORIGIN.y},
                  BALL9_INIT_POS       = {ORIGIN.x, ORIGIN.y+50},
                  P1_INIT_POS       = { ORIGIN.x - 300, ORIGIN.y },
                  P2_INIT_POS       = { ORIGIN.x + 300, ORIGIN.y };

constexpr char POOL_STICK[]  = "assets/poolstick.png";
constexpr char BALL_7[]  = "assets/7Ball.png";
constexpr char BALL_8[]  = "assets/8Ball.png";
constexpr char BALL_9[]  = "assets/9Ball.png";
constexpr char P1WINS[]  = "assets/player1_wins.png";
constexpr char P2WINS[]  = "assets/player2_wins.png";
constexpr char DRAW[]  = "assets/draw.png";
constexpr char BACK_GROUND[]  = "assets/pool_table.png";

// Global Variables

AppStatus gAppStatus     = RUNNING;
Players playingStyle = PVP;
Won     whowon  = notyet;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;
int num_ball = 1, gP1Score = 0, gP2Score = 0;
bool reset7 = false, reset8 = false, reset9=false;
Vector2 gP1Position  = P1_INIT_POS,
        gP1Movement  = { 0.0f, 0.0f },
        gP1Scale     = STICK_BASE_SIZE ;

Vector2 gP2Position  = P2_INIT_POS,
        gP2Movement  = { 0.0f, 0.0f },
        gP2Scale     = STICK_BASE_SIZE ;

Vector2 gBall7Position = BALL7_INIT_POS,
        gBall8Position = BALL8_INIT_POS,
        gBall9Position = BALL9_INIT_POS,

        gBall7Movement  = {0.0f,0.0f},
        gBall8Movement  = {1.0f,1.0f},
        gBall9Movement  = {0.0f,0.0f},

        gBallScale     = BALL_BASE_SIZE;

Vector2 gEndPosition = ORIGIN,
        gEndScale   = {ORIGIN.x*2,ORIGIN.y*2};        

Texture2D gP1Texture;
Texture2D gP2Texture;
Texture2D gBall8Texture, gBall9Texture, gBall7Texture;

Texture2D gDrawTexture, gP1WinsTexture, gP2WinsTexture, gBackGroundTexture;


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions

bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    // TODO: implement AABB collision detection
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x+scaleB->x)/2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y+scaleB->y)/2.0f);

    return xDistance <0.0f && yDistance <0.0f;
}

void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale)
{
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    DrawTexturePro(
        *texture,
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pool Pong");


    gBackGroundTexture  = LoadTexture(BACK_GROUND);

    gP1Texture  = LoadTexture(POOL_STICK);
    gP2Texture = LoadTexture(POOL_STICK);
    gBall7Texture = LoadTexture(BALL_7);
    gBall8Texture = LoadTexture(BALL_8);
    gBall9Texture = LoadTexture(BALL_9);

    gP1WinsTexture = LoadTexture(P1WINS);
    gP2WinsTexture = LoadTexture(P2WINS);
    gDrawTexture = LoadTexture(DRAW);

    SetTargetFPS(FPS);
}

void processInput()
{
    // to reset the game when done
    if(IsKeyPressed(KEY_SPACE) && whowon!=notyet){
        whowon = notyet;
    
        gP1Score = gP2Score = 0;
        num_ball = 1;
        reset7 = false, reset8 = false, reset9=false;
        gBall7Position = BALL7_INIT_POS;
        gBall8Position = BALL8_INIT_POS;
        gBall9Position = BALL9_INIT_POS;

        gBall7Movement  = {0.0f,0.0f};
        gBall8Movement  = {1.0f,1.0f};
        gBall9Movement  = {0.0f,0.0f};
    }
    
    // switch to and from PVP and PVE
    if(IsKeyPressed(KEY_T)) {
        if(playingStyle == PVE)
        playingStyle = PVP;
        else
        playingStyle = PVE;
    }
    
    gP1Movement = { 0.0f, 0.0f };
    if(playingStyle == PVP)
        gP2Movement.y = 0.0f;

    
    // to control players
    if (IsKeyDown(KEY_W)) gP1Movement.y = -1.0f;
    else if (IsKeyDown(KEY_S)) gP1Movement.y = 1.0f;
    if(playingStyle == PVP){
        if (IsKeyDown(KEY_UP)) gP2Movement.y = -1.0f;
        else if (IsKeyDown(KEY_DOWN)) gP2Movement.y = 1.0f;
    }



    // to control number of balls
    if (IsKeyPressed(KEY_ONE)){
        num_ball = 1;
        reset8 = true;
    } 
    else if (IsKeyPressed(KEY_TWO)){
        num_ball = 2;
        reset7 = true;
        reset8=true;
    } 
    else if (IsKeyPressed(KEY_THREE)){
        num_ball = 3;
        reset9 = true;
        reset7=true;
        reset8=true;
    } 
    if(reset8){
        gBall7Position = BALL7_INIT_POS;
        gBall8Position = BALL8_INIT_POS;
        gBall9Position = BALL9_INIT_POS;

        gBall8Movement  = {1.0f,1.0f};
        gBall7Movement = gBall9Movement = {0.0f,0.0f};
        reset8 = false;
    }
    if(reset7){
        gBall7Movement  = {-1.0f,1.0f};
        reset7=false;
    }
    if(reset9){
        gBall9Movement = {1.25f, 0.75f};
        reset9=false;
    }
 

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    float ticks    = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;
    
    // to stop the game when on end screen
    if(whowon!=notyet)return;

    // using deltatime to apply movement
    if(gP1Position.y + SPEED * gP1Movement.y*deltaTime > gP1Scale.y/2 && 
        gP1Position.y + SPEED * gP1Movement.y*deltaTime < SCREEN_HEIGHT - gP1Scale.y/2)
        gP1Position.y += SPEED * gP1Movement.y*deltaTime;
    
    if( playingStyle == PVE){
        if(gP2Movement.y==0.0f)gP2Movement.y=1.0f;
        if(gP2Position.y+gP2Scale.y/2 >= SCREEN_HEIGHT || gP2Position.y-gP2Scale.y/2 <= 0)
            gP2Movement.y*=-1;

        gP2Position.y += (SPEED-50) * gP2Movement.y*deltaTime;
        
    }
    else if(gP2Position.y + SPEED * gP2Movement.y*deltaTime > gP2Scale.y/2 && 
        gP2Position.y + SPEED * gP2Movement.y*deltaTime < SCREEN_HEIGHT - gP2Scale.y/2){
        gP2Position.y += SPEED * gP2Movement.y*deltaTime;
    }
    



    // for the 8 ball
    if(gBall8Position.y <= BALL_BASE_SIZE.y/2 || gBall8Position.y >= SCREEN_HEIGHT-BALL_BASE_SIZE.y/2) 
        gBall8Movement.y*=-1; 
    if(isColliding(&gP1Position, &gP1Scale, &gBall8Position, &gBallScale) ||
       isColliding(&gP2Position, &gP2Scale, &gBall8Position, &gBallScale) )
        gBall8Movement.x*=-1;

    gBall8Position ={
        gBall8Position.x += BALL_SPEED * gBall8Movement.x*deltaTime,
        gBall8Position.y += BALL_SPEED * gBall8Movement.y*deltaTime
    };

    // for the 7 ball
    if(gBall7Position.y <= BALL_BASE_SIZE.y/2 || gBall7Position.y >= SCREEN_HEIGHT-BALL_BASE_SIZE.y/2) 
        gBall7Movement.y*=-1; 
    if(isColliding(&gP1Position, &gP1Scale, &gBall7Position, &gBallScale) ||
       isColliding(&gP2Position, &gP2Scale, &gBall7Position, &gBallScale) )
        gBall7Movement.x*=-1;

    gBall7Position ={
        gBall7Position.x += BALL_SPEED * gBall7Movement.x*deltaTime,
        gBall7Position.y += BALL_SPEED * gBall7Movement.y*deltaTime
    };

    // for the 9 ball
    if(gBall9Position.y <= BALL_BASE_SIZE.y/2 || gBall9Position.y >= SCREEN_HEIGHT-BALL_BASE_SIZE.y/2) 
        gBall9Movement.y*=-1; 
    if(isColliding(&gP1Position, &gP1Scale, &gBall9Position, &gBallScale) ||
       isColliding(&gP2Position, &gP2Scale, &gBall9Position, &gBallScale) )
        gBall9Movement.x*=-1;

    gBall9Position ={
        gBall9Position.x += BALL_SPEED * gBall9Movement.x*deltaTime,
        gBall9Position.y += BALL_SPEED * gBall9Movement.y*deltaTime
    };

    // checking if ball is out of bounds and adding score to whoever scored
    if( gBall7Position.x <= 0 - BALL_BASE_SIZE.x/2 || gBall7Position.x > SCREEN_WIDTH + BALL_BASE_SIZE.x/2){
        if(gBall7Position.x <= 0 - BALL_BASE_SIZE.x/2 ) gP2Score ++;
        else gP1Score ++;
        reset7 = true;
    }
    if( gBall8Position.x <= 0 - BALL_BASE_SIZE.x/2 || gBall8Position.x > SCREEN_WIDTH + BALL_BASE_SIZE.x/2){
        if(gBall8Position.x <= 0 - BALL_BASE_SIZE.x/2) gP2Score++;
        else gP1Score++;
        reset8 = true;
    }
    if( gBall9Position.x <= 0 - BALL_BASE_SIZE.x/2 || gBall9Position.x > SCREEN_WIDTH + BALL_BASE_SIZE.x/2){
        if(gBall9Position.x <= 0 - BALL_BASE_SIZE.x/2) gP2Score++;
        else gP1Score++;
        reset9 = true;
    }


    // reseting the balls to their position
    if(reset7){
        gBall7Position = BALL7_INIT_POS;
        gBall7Movement = {-1.0f,1.0f};
        reset7=false;
    }
    if(reset8){
        gBall8Position = BALL8_INIT_POS;
        gBall8Movement = {1.0f,1.0f};
        reset8=false;
    }
    if(reset9){
        gBall9Position = BALL9_INIT_POS;
        gBall9Movement = {1.25f,0.75f};
        reset9=false;
    }

    // now check of someone won or not
    if(gP1Score >= WIN_SCORE){
        if(gP2Score>gP1Score)whowon = player2;
        else if(gP2Score == gP1Score)whowon = draw;
        else whowon = player1;
        gBall8Movement = gBall7Movement = gBall9Movement = {0.0f,0.0f};
    }
    else if(gP2Score >= WIN_SCORE){
        if(gP1Score > gP2Score) whowon = player1;
        else if(gP2Score == gP1Score)whowon = draw;
        else whowon = player2;
        gBall8Movement = gBall7Movement = gBall9Movement = {0.0f,0.0f};
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
   
    // end screens and the background use the same dimensions sooo i just used them, i know i shouldve picked a better name
    renderObject(&gBackGroundTexture,  &gEndPosition,  &gEndScale);
    // players render
    renderObject(&gP1Texture,  &gP1Position,  &gP1Scale);
    renderObject(&gP2Texture, &gP2Position, &gP2Scale);
    // balls render
    renderObject(&gBall8Texture, &gBall8Position, &gBallScale);
    if(num_ball>1)
        renderObject(&gBall7Texture, &gBall7Position, &gBallScale);
    if(num_ball>2)
        renderObject(&gBall9Texture, &gBall9Position, &gBallScale);

    // end screen render
    if(whowon == draw){
        renderObject(&gDrawTexture, &gEndPosition, &gEndScale);
    }
    else if (whowon == player1){
        renderObject(&gP1WinsTexture, &gEndPosition, &gEndScale);
    }
    else if(whowon == player2){
        renderObject(&gP2WinsTexture, &gEndPosition, &gEndScale);
    }
    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gBackGroundTexture);
    UnloadTexture(gP1Texture);
    UnloadTexture(gP2Texture);
    UnloadTexture(gBall7Texture);
    UnloadTexture(gBall8Texture);
    UnloadTexture(gBall9Texture);
    UnloadTexture(gP1WinsTexture);
    UnloadTexture(gP2WinsTexture);
    UnloadTexture(gDrawTexture);

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
