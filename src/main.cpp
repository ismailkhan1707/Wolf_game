#include "GameEngine.h" 
#include "raylib.h"
#include <string>

int main() {
    InitWindow(1280, 720, "Wolf Survival");
    ToggleFullscreen(); 
    SetTargetFPS(60);

    // Assets
    Texture2D background = LoadTexture("assets/uforest.png");
    Texture2D wolf1 = LoadTexture("assets/wolf1.png");
    Texture2D wolf2 = LoadTexture("assets/wolf2.png");

    Color customGray = Color{ 200, 240, 250, 200 };
    Color redButton = Color{ 200, 100, 100, 200 }; // --- NEW: Red color for Undo

    GameEngine game;
    game.init(); 

    float timer = 0.0f;
    bool showWolf1 = true;
    float bgX = 0.0f;
    float bgSpeed = 20.0f;

    while (!WindowShouldClose()) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();

        // 1. Define Option Buttons (Bottom Right)
        Rectangle btnBoundsA = { (float)sw - 750, (float)sh - 135, 350, 130 };
        Rectangle btnBoundsB = { (float)sw - 350, (float)sh - 135, 350, 130 };
        
        // 2. --- NEW: Define Undo Button (Bottom Left) ---
        Rectangle btnBoundsUndo = { 20, (float)sh - 80, 150, 60 };

        float deltaTime = GetFrameTime();

        // Update Animation
        bgX -= bgSpeed * deltaTime;
        if (bgX <= -background.width) bgX = 0;
        timer += deltaTime;
        if (timer >= 0.3f) { showWolf1 = !showWolf1; timer = 0.0f; }

        // --- INPUT HANDLING ---
        Vector2 mousePoint = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Check Option 1
            if (!game.current->isEnding && CheckCollisionPointRec(mousePoint, btnBoundsA)) {
                game.makeChoice(1);
            }
            // Check Option 2
            else if (!game.current->isEnding && CheckCollisionPointRec(mousePoint, btnBoundsB)) {
                game.makeChoice(2);
            }
            // --- NEW: Check Undo Click ---
            else if (CheckCollisionPointRec(mousePoint, btnBoundsUndo)) {
                game.undoGame();
            }
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(BLACK);

        // Background & Wolf
        DrawTexture(background, (int)bgX, 0, WHITE);
        DrawTexture(background, (int)bgX + background.width, 0, WHITE);
        if (sw > background.width * 2) DrawTexture(background, (int)bgX + (background.width * 2), 0, WHITE);
        
        if (showWolf1) DrawTexture(wolf1, 0, 420, WHITE);
        else DrawTexture(wolf2, 0, 420, WHITE);

        // Top Dialog
        DrawRectangle(0, 0, sw, 180, customGray);
        DrawText(game.current->description.c_str(), 20, 20, 24, BLACK);

        // Event/Info Popups
        if (game.eventActive) {
            DrawText("EVENT!", GetScreenWidth() - 530 , 150, 30, RED);
            DrawText(game.activeEvent.description.c_str(), GetScreenWidth() - 400, 150, 24, RED);
        }
        // --- NEW: Display "Time Rewound" or "Item Found" message ---
        if (game.currentMessage != "") {
             DrawText(game.currentMessage.c_str(), (GetScreenWidth() /2) - 100, 160, 20, YELLOW);
        }

        // Draw Buttons
        if (!game.current->isEnding) {
            // Option 1
            DrawRectangleRec(btnBoundsA, customGray);
            DrawText(game.current->choiceA.c_str(), (int)btnBoundsA.x + 20, (int)btnBoundsA.y + 50, 20, BLACK);
            DrawText("Option 1", (int)btnBoundsA.x + 5, (int)btnBoundsA.y + 5, 20, DARKGRAY);

            // Option 2
            DrawRectangleRec(btnBoundsB, customGray);
            DrawText(game.current->choiceB.c_str(), (int)btnBoundsB.x + 20, (int)btnBoundsB.y + 50, 20, BLACK);
            DrawText("Option 2", (int)btnBoundsB.x + 5, (int)btnBoundsB.y + 5, 20, DARKGRAY);
        } else {
            // Ending
            DrawText("GAME OVER", sw/2 - 100, sh/2, 50, RED);
        }

        // --- NEW: Draw Undo Button ---
        DrawRectangleRec(btnBoundsUndo, redButton);
        DrawText("UNDO", (int)btnBoundsUndo.x + 40, (int)btnBoundsUndo.y + 20, 20, WHITE);

        // Stats
        string healthText = "Health: " + to_string(game.player.health);
        string energyText = "Energy: " + to_string(game.player.energy);
        string hungerText = "Hunger: " + to_string(game.player.hunger);
        int statX = sw - 250;
        DrawText(healthText.c_str(), statX, 20, 20, BLACK);
        DrawText(energyText.c_str(), statX, 50, 20, BLACK);
        DrawText(hungerText.c_str(), statX, 80, 20, BLACK);

        EndDrawing();
    }
    
    UnloadTexture(background);
    UnloadTexture(wolf1);
    UnloadTexture(wolf2);
    CloseWindow();
    return 0;
}