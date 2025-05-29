#include "raylib.h"          
#include "raymath.h"        
#include <stdlib.h>          
#include <time.h>            
#include <math.h>            

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define NUM_LANES 3          
#define LANE_WIDTH 100       
#define CAT_WIDTH 31.5        
#define CAT_HEIGHT 54         
#define NUM_FRAMES 4         
#define OBSTACLE_SIZE 50      
#define MAX_OBSTACLES 2       

Texture2D mainBackground;
Texture2D gameoverBackground;
Texture2D creditsBackground;
Texture2D catSprite;

typedef struct Obstacle {
    Vector2 position;  
    int lane;          
    Color color;       
    int shape;         
    bool active;       
} Obstacle;

typedef enum {
    MENU,
    GAME,
    CREDITS
} GameScreen;

int GetLaneX(int lane) {
    int startX = SCREEN_WIDTH / 2 - (LANE_WIDTH * NUM_LANES) / 2;
    return startX + lane * LANE_WIDTH + (LANE_WIDTH - CAT_WIDTH)/2;
}

void GenerateNewObstacles(Obstacle obstacles[], int lanePositions[]) {
    int lane1 = rand() % NUM_LANES;
    int lane2;
    do {
        lane2 = rand() % NUM_LANES;
    } while (lane2 == lane1);
    
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        int lane = (i == 0) ? lane1 : lane2;
        obstacles[i].lane = lane;
        obstacles[i].position.x = lanePositions[lane];
        obstacles[i].position.y = -OBSTACLE_SIZE - (rand() % 200); 
        obstacles[i].shape = rand() % 3; 
        obstacles[i].color = (Color){ rand()%256, rand()%256, rand()%256, 255 };
        obstacles[i].active = true;
    }
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Catway Surfers 800x600");
    SetTargetFPS(60); 
    
    srand(time(NULL)); 
    
    InitAudioDevice();
    Music music = LoadMusicStream("assets/sounds/theme.ogg");
    PlayMusicStream(music);
    music.looping = true;
    
    mainBackground = LoadTexture("assets/images/bg-main.jpeg");
    creditsBackground = LoadTexture("assets/images/bg-credits.jpg");
    catSprite = LoadTexture("assets/images/sprite.png");
    gameoverBackground = LoadTexture("assets/images/bg-game-over.png");
    
    GameScreen currentScreen = MENU;
    bool soundOn = false;
    int catSkin = 0;
    int currentFrame = 0;
    float frameTime = 0;
    float catJumpPhase = 0;
    bool gameRunning = false;
    float timeAlive = 0;
    int score = 0;
    int highScore = 0;
    float scoreTimer = 0;
    
    int lanePositions[NUM_LANES];
    for (int i = 0; i < NUM_LANES; i++)
        lanePositions[i] = GetLaneX(i);
    
    int catLane = 1;
    Rectangle cat = { lanePositions[catLane], SCREEN_HEIGHT - CAT_HEIGHT - 20, CAT_WIDTH, CAT_HEIGHT };
    
    Obstacle obstacles[MAX_OBSTACLES];
    GenerateNewObstacles(obstacles, lanePositions);
    
    Rectangle btnStart = { SCREEN_WIDTH/2 - 100, 200, 200, 50 };
    Rectangle btnSkins = { SCREEN_WIDTH/2 - 100, 270, 200, 50 };
    Rectangle btnPrint = { SCREEN_WIDTH/2 - 100, 410, 200, 50 };
    Rectangle btnSound = { SCREEN_WIDTH/2 - 100, 340, 200, 50 };
    Rectangle btnCredits = { SCREEN_WIDTH/2 - 100, 527, 200, 50 };
    Color btnColor = LIGHTGRAY;
    
    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_P))TakeScreenshot("Print.png");
        
        Vector2 mouse = GetMousePosition();
        
        if (soundOn) UpdateMusicStream(music);
        
        if (currentScreen == MENU) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, btnStart)) {
                    currentScreen = GAME;
                    gameRunning = true;
                    timeAlive = 0;
                    score = 0;
                    scoreTimer = 0;
                    catLane = 1;
                    cat.x = lanePositions[catLane];
                    GenerateNewObstacles(obstacles, lanePositions);
                } else if (CheckCollisionPointRec(mouse, btnCredits)) {
                    currentScreen = CREDITS;
                } else if (CheckCollisionPointRec(mouse, btnPrint)) {
                    TakeScreenshot("Print.png");
                } else if (CheckCollisionPointRec(mouse, btnSound)) {
                    soundOn = !soundOn;
                    btnColor = (btnColor.r == ORANGE.r) ? LIGHTGRAY : ORANGE;
                    if (soundOn) PlayMusicStream(music);
                    else PauseMusicStream(music);
                } else if (CheckCollisionPointRec(mouse, btnSkins)) {
                    catSkin = (catSkin + 1) % 4;
                }
            }
            
            frameTime += GetFrameTime();
            catJumpPhase += 0.1f;
            if (frameTime >= 0.1f) {
                currentFrame = (currentFrame + 1) % NUM_FRAMES;
                frameTime = 0;
            }
            
            BeginDrawing();
            ClearBackground(DARKBLUE);
            DrawTexture(mainBackground, 0, 0, WHITE);
            
            Rectangle source = { currentFrame * CAT_WIDTH, catSkin * CAT_HEIGHT, CAT_WIDTH, CAT_HEIGHT };
            Vector2 pos = { SCREEN_WIDTH / 2.0f - CAT_WIDTH / 2.0f , 120 };
            DrawTextureRec(catSprite, source, pos, WHITE);
            
            DrawText("Catway Surfers", SCREEN_WIDTH/2 - MeasureText("Catway Surfers", 30)/2, 60, 30, BLACK);
            
            DrawRectangleRec(btnStart, LIGHTGRAY);
            DrawText("Iniciar Jogo", btnStart.x + 40, btnStart.y + 15, 20, BLACK);
            
            DrawRectangleRec(btnSound, btnColor);
            DrawText(soundOn ? "Som: Ligado" : "Som: Desligado", btnSound.x + 40, btnSound.y + 15, 20, BLACK);
            
            DrawRectangleRec(btnSkins, LIGHTGRAY);
            DrawText("Trocar Skin", btnSkins.x + 40, btnSkins.y + 15, 20, BLACK);
            
            DrawRectangleRec(btnCredits, LIGHTGRAY);
            DrawText("Créditos", btnCredits.x + 60, btnCredits.y + 15, 20, BLACK);
            
            DrawRectangleRec(btnPrint, LIGHTGRAY);
            DrawText("Tirar Print", btnPrint.x + 50, btnPrint.y + 15, 20, BLACK);
            
            EndDrawing();
            continue;
        }
        
        if (currentScreen == CREDITS) {
            if (IsKeyPressed(KEY_ENTER)) currentScreen = MENU;
            
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(creditsBackground, 0, 0, WHITE);
            
            DrawText("Créditos:", 50, 100, 30, BLACK);
            DrawText("Desenvolvido por:", 50, 150, 20, BLACK);
            DrawText("Lucas Henrique Bastoge", 50, 200, 20, BLACK);
            DrawText("Tiago Rodrigo Ferreira Crippa", 50, 250, 20, BLACK);
            DrawText("Rafael Lopo Montalvão", 50, 300, 20, BLACK);
            DrawText("Matheus Ribeiro França", 50, 350, 20, BLACK);
            DrawText("Pressione ENTER para voltar", 50, 400, 20, ORANGE);
            
            EndDrawing();
            continue;
        }
        
        if (!gameRunning) {
            if (score > highScore) highScore = score;
            
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(gameoverBackground, 0, 0, WHITE);
            
            DrawText("Pressione ENTER para jogar novamente", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 200, 20, DARKGRAY);
            
            EndDrawing();
            
            if (IsKeyPressed(KEY_ENTER)) {
                gameRunning = true;
                timeAlive = 0;
                score = 0;
                scoreTimer = 0;
                catLane = 1;
                cat.x = lanePositions[catLane];
                GenerateNewObstacles(obstacles, lanePositions);
            }
            continue;
        }
        
        if (IsKeyPressed(KEY_A) && catLane > 0) {
            catLane--;
            cat.x = lanePositions[catLane];
        }
        if (IsKeyPressed(KEY_D) && catLane < NUM_LANES - 1) {
            catLane++;
            cat.x = lanePositions[catLane];
        }
        
        timeAlive += GetFrameTime();
        frameTime += GetFrameTime();
        scoreTimer += GetFrameTime();
        
        if (scoreTimer >= 0.5f) {
            score += 1; 
            scoreTimer = 0;
        }
        
        if (frameTime >= 0.1f) {
            currentFrame = (currentFrame + 1) % NUM_FRAMES;
            frameTime = 0;
        }
        
        bool allInactive = true;
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (obstacles[i].active) {
                obstacles[i].position.y += 5; 
                
                Rectangle obsRect = { obstacles[i].position.x, obstacles[i].position.y, OBSTACLE_SIZE, OBSTACLE_SIZE };
                if (CheckCollisionRecs(cat, obsRect)) gameRunning = false;
                
                if (obstacles[i].position.y > SCREEN_HEIGHT) obstacles[i].active = false;
                else allInactive = false;
            }
        }
        
        if (allInactive) GenerateNewObstacles(obstacles, lanePositions);
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        for (int i = 0; i < NUM_LANES - 1; i++)
            DrawRectangleLines(lanePositions[i] - 4 + CAT_WIDTH/2, 0, LANE_WIDTH, SCREEN_HEIGHT, LIGHTGRAY);
        
        Rectangle src = { currentFrame * CAT_WIDTH, catSkin * CAT_HEIGHT, CAT_WIDTH, CAT_HEIGHT };
        Vector2 dst = { cat.x + (LANE_WIDTH - CAT_WIDTH) / 2.0f - 36, cat.y };
        DrawTextureRec(catSprite, src, dst, WHITE);
        
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (!obstacles[i].active) continue;
            Vector2 pos = obstacles[i].position;
            Color col = obstacles[i].color;
            pos.x -= 20;
            
            if (obstacles[i].shape == 0)
                DrawRectangle(pos.x, pos.y, OBSTACLE_SIZE, OBSTACLE_SIZE, col);
            else if (obstacles[i].shape == 1)
                DrawCircle(pos.x + OBSTACLE_SIZE/2, pos.y + OBSTACLE_SIZE/2, OBSTACLE_SIZE/2, col);
            else
                DrawTriangle((Vector2){pos.x + OBSTACLE_SIZE/2, pos.y},
                             (Vector2){pos.x, pos.y + OBSTACLE_SIZE},
                             (Vector2){pos.x + OBSTACLE_SIZE, pos.y + OBSTACLE_SIZE}, col);
        }
        
        DrawText(TextFormat("Tempo: %.1f", timeAlive), 10, 10, 20, BLACK);
        DrawText(TextFormat("Pontos: %d", score), 10, 35, 20, BLACK);
        DrawText(TextFormat("Recorde: %d", highScore), 10, 60, 20, BLACK);
        
        EndDrawing();
    }
    
    UnloadMusicStream(music);
    CloseAudioDevice();
    UnloadTexture(mainBackground);
    UnloadTexture(gameoverBackground);
    UnloadTexture(creditsBackground);
    UnloadTexture(catSprite);
    CloseWindow();
    
    return 0;
}