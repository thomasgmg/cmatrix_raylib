#include "raylib.h"
#include <stdlib.h>

int const ScreenWidth = 1500;
int const ScreenHeight = 900;

Font font;

typedef struct Stream
{
    float x;
    float y;
    float speed;
    char characters[50];
    int length;
    bool active;
} Stream;

Stream streams[100];
int streamCount = 0;

int selectedColorIndex = 0;
bool randomColorMode = false;

/* Declarations */
void InitStreams(void);
void UpdateStreams(void);
void UpdateGame(void);
void DrawGame(void);

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(ScreenWidth, ScreenHeight, "cmatrix raylib");

    SetTargetFPS(60);

    font = LoadFontEx("resources/font.ttf", 96, 0, 0);
    InitStreams();

    while (!WindowShouldClose())
    {
        UpdateStreams();
        UpdateGame();
        DrawGame();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}

void InitStreams(void)
{
    streamCount = ScreenWidth / 20;
    for (int i = 0; i < streamCount; i++)
    {
        streams[i].x = i * 20;
        streams[i].y = (float)(rand() % ScreenHeight);
        streams[i].speed = (float)(rand() % 5 + 2);
        streams[i].length = rand() % 20 + 10;
        streams[i].active = true;

        /* Fill character array with random printable ASCII characters */
        for (int j = 0; j < streams[i].length; j++)
        {
            streams[i].characters[j] = (char)(rand() % 94 + 33); /* ASCII 33-126 */
        }
    }
}

void UpdateStreams(void)
{
    for (int i = 0; i < streamCount; i++)
    {
        if (streams[i].active)
        {
            /* Update stream position */
            streams[i].y += streams[i].speed;

            /* Reset stream if it goes off screen */
            if (streams[i].y - streams[i].length * 35 > ScreenHeight)
            {
                streams[i].y = 0;
                streams[i].length = rand() % 20 + 10;
                for (int j = 0; j < streams[i].length; j++)
                {
                    streams[i].characters[j] = (char)(rand() % 94 + 33);
                }
            }

            /* Occasionally change a character in the stream */
            if (rand() % 100 < 10)
            {
                int charIndex = rand() % streams[i].length;
                streams[i].characters[charIndex] = (char)(rand() % 94 + 33);
            }
        }
    }
}

void UpdateGame(void)
{
    for (int i = 0; i < streamCount; i++)
    {
        if (streams[i].active)
        {
            if (IsKeyPressed(KEY_J))
            {
                streams[i].speed -= 2;
            }
            if (IsKeyPressed(KEY_K))
            {
                streams[i].speed += 2;
            }
        }
    }

    if (IsKeyPressed(KEY_ONE))
    {
        selectedColorIndex = 0; /* DARKGREEN */
    }
    if (IsKeyPressed(KEY_TWO))
    {
        selectedColorIndex = 1; /* GOLD */
    }
    if (IsKeyPressed(KEY_THREE))
    {
        selectedColorIndex = 2; /* GRAY */
    }
    if (IsKeyPressed(KEY_FOUR))
    {
        selectedColorIndex = 3; /* ORANGE */
    }
    if (IsKeyPressed(KEY_FIVE))
    {
        selectedColorIndex = 4; /* MAROON */
    }
    if (IsKeyPressed(KEY_SIX))
    {
        selectedColorIndex = 5; /* SKYBLUE */
    }
    if (IsKeyPressed(KEY_SEVEN))
    {
        selectedColorIndex = 6; /* DARKBLUE */
    }
    if (IsKeyPressed(KEY_EIGHT))
    {
        selectedColorIndex = 7; /* VIOLET */
    }
    if (IsKeyPressed(KEY_NINE))
    {
        selectedColorIndex = 8; /* DARKPURPLE */
    }
    if (IsKeyPressed(KEY_ZERO))
    {
        selectedColorIndex = 9; /* WHITE */
    }
    if (IsKeyPressed(KEY_R))
    {
        randomColorMode = !randomColorMode;
    }
}

void DrawGame(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    Color colors[] = {
        (Color){0,   100, 0,   0}, /* DARKGREEN */
        (Color){218, 165, 32,  0}, /* GOLD */
        (Color){128, 128, 128, 0}, /* GRAY */
        (Color){255, 165, 0,   0}, /* ORANGE */
        (Color){128, 0,   0,   0}, /* MAROON */
        (Color){135, 206, 235, 0}, /* SKYBLUE */
        (Color){0,   0,   139, 0}, /* DARKBLUE */
        (Color){238, 130, 238, 0}, /* VIOLET */
        (Color){139, 0,   139, 0}, /* DARKPURPLE */
        (Color){255, 255, 255, 0}, /* WHITE */
        (Color){0,  0,   0,   0}  /* WHITE */
    };
    int colorCount = 10;

    for (int i = 0; i < streamCount; i++)
    {
        if (streams[i].active)
        {
            Color baseColor = randomColorMode ? colors[i % colorCount] : colors[selectedColorIndex];

            for (int j = 0; j < streams[i].length; j++)
            {
                /* Calculate fading color (brighter at the top) */
                int alpha = 255 - (j * 255 / streams[i].length);
                Color color = {baseColor.r, baseColor.g, baseColor.b, (unsigned char)alpha};

                /* Draw each character */
                char text[2] = {streams[i].characters[j], '\0'};
                DrawTextEx(font, text, (Vector2){streams[i].x, streams[i].y - j * 20}, 20, 0, color);
            }
        }
    }

    EndDrawing();
}
