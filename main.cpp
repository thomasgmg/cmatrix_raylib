#include "raylib.h"
#include <stdlib.h>

int const ScreenWidth = 1500;
int const ScreenHeight = 900;

Texture2D background;

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
int StreamCount = 0;

int SelectedColorIndex = 0;
bool RandomColorMode = false;

bool pause = false;

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

    /* background = LoadTexture("resources/Alley.png");
       background = LoadTexture("resources/Ascii.png");
       background = LoadTexture("resources/Fantasy.jpg");
       background = LoadTexture("resources/FinalFantasy.jpg");
       background = LoadTexture("resources/galactic.jpg");
       background = LoadTexture("resources/Hogwarts.jpg");
       background = LoadTexture("resources/inzsky.png");
       background = LoadTexture("resources/Landscape.jpg");
       background = LoadTexture("resources/LordofRings1.jpg");
       background = LoadTexture("resources/LordofRings2.jpg");
       background = LoadTexture("resources/Monoke.jpg");
       background = LoadTexture("resources/Realm.jpg");
       background = LoadTexture("resources/samarkand.png");
       background = LoadTexture("resources/stairway.jpg");
       background = LoadTexture("resources/storm.jpg");
       background = LoadTexture("resources/sumerupond.jpeg");
       background = LoadTexture("resources/wallhaven.jpg"); */

    InitStreams();

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            pause = !pause;
        }

        UpdateGame();
        DrawGame();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}

void InitStreams(void)
{
    StreamCount = ScreenWidth / 20;
    for (int i = 0; i < StreamCount; i++)
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
    for (int i = 0; i < StreamCount; i++)
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
    if (pause)
    {
        return;
    }

    UpdateStreams();

    for (int i = 0; i < StreamCount; i++)
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
        SelectedColorIndex = 0; /* DARKGREEN */
    }
    if (IsKeyPressed(KEY_TWO))
    {
        SelectedColorIndex = 1; /* GOLD */
    }
    if (IsKeyPressed(KEY_THREE))
    {
        SelectedColorIndex = 2; /* GRAY */
    }
    if (IsKeyPressed(KEY_FOUR))
    {
        SelectedColorIndex = 3; /* ORANGE */
    }
    if (IsKeyPressed(KEY_FIVE))
    {
        SelectedColorIndex = 4; /* MAROON */
    }
    if (IsKeyPressed(KEY_SIX))
    {
        SelectedColorIndex = 5; /* SKYBLUE */
    }
    if (IsKeyPressed(KEY_SEVEN))
    {
        SelectedColorIndex = 6; /* DARKBLUE */
    }
    if (IsKeyPressed(KEY_EIGHT))
    {
        SelectedColorIndex = 7; /* VIOLET */
    }
    if (IsKeyPressed(KEY_NINE))
    {
        SelectedColorIndex = 8; /* DARKPURPLE */
    }
    if (IsKeyPressed(KEY_ZERO))
    {
        SelectedColorIndex = 9; /* WHITE */
    }
    if (IsKeyPressed(KEY_R))
    {
        RandomColorMode = !RandomColorMode;
    }
}

void DrawGame(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(background, {0, 0, (float)background.width, (float)background.height},
                   {0, 0, (float)ScreenWidth, (float)ScreenHeight}, {0, 0}, 0.0f, WHITE);

    Color colors[] = {
        (Color){0, 100, 0, 255},     /* DARKGREEN */
        (Color){218, 165, 32, 255},  /* GOLD */
        (Color){128, 128, 128, 255}, /* GRAY */
        (Color){255, 165, 0, 255},   /* ORANGE */
        (Color){128, 0, 0, 255},     /* MAROON */
        (Color){135, 206, 235, 255}, /* SKYBLUE */
        (Color){0, 0, 139, 255},     /* DARKBLUE */
        (Color){238, 130, 238, 255}, /* VIOLET */
        (Color){139, 0, 139, 255},   /* DARKPURPLE */
        (Color){255, 255, 255, 255}, /* WHITE */
    };
    int colorCount = 10;

    for (int i = 0; i < StreamCount; i++)
    {
        if (streams[i].active)
        {
            Color baseColor = RandomColorMode ? colors[i % colorCount] : colors[SelectedColorIndex];

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

    if (pause)
    {
        DrawTextEx(font, "PAUSE",
                   (Vector2){(float)GetScreenWidth() / 2 - MeasureTextEx(font, "PAUSE", 30, 1).x / 2,
                             (float)GetScreenHeight() / 2},
                   30, 1, colors[SelectedColorIndex]);
    }

    EndDrawing();
}
