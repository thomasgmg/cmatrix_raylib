#include "raylib.h"
#include <cstdio>
#include <stdlib.h>
#include <string.h>

/* Chinese text that will be displayed, must be UTF-8 (save this code file as UTF-8) */
static char *chineseText =
    "いろはにほへとちりぬるをわかよたれそつねならむうゐのおくやまけふこえてあさきゆめみしゑひもせす";

int ScreenWidth = 1500;
int ScreenHeight = 900;

Texture2D background;

Font font;

bool ChineseMode = false;

Color const colors[] = {
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
int const ColorCount = 10;

typedef struct Stream
{
    float x;
    float y;
    float speed;
    char characters[50];
    int codepoints[50]; /* Store codepoints for Chinese characters */
    int length;
    bool active;
} Stream;

Stream streams[100];
int StreamCount = 0;

int SelectedColorIndex = 0;
bool RandomColorMode = true;

bool pause = false;

float BaseFontSize = 20.0f;
float ScaleFactor = 1.0f;

/* Declarations */
void InitStreams(void);
void UpdateStreams(void);
void UpdateStreamColor(void);
void UpdateStreamSpeed(void);
void UpdateGame(void);
void DrawGame(void);
void HandleResize(void);

int main(int argc, char *argv[])
{
    bool fullscreen = false;
    if (argc > 1)
    {
        char *arg1 = argv[1];
        if (strcmp(arg1, "-f") == 0)
        {
            fullscreen = true;
        }
    }

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_RESIZABLE);
    InitWindow(ScreenWidth, ScreenHeight, "cmatrix raylib");

    SetTargetFPS(60);

    /* Load codepoints for Chinese characters */
    int codepointCount = 0;
    int *codepoints = LoadCodepoints(chineseText, &codepointCount);

    /* Add ASCII codepoints (32–126) */
    int asciiCount = 95; /* 32 to 126 inclusive */
    int totalCodepointCount = codepointCount + asciiCount;
    int *allCodepoints = (int *)malloc(totalCodepointCount * sizeof(int));

    /* Copy ASCII codepoints */
    for (int i = 0; i < asciiCount; i++)
    {
        allCodepoints[i] = 32 + i; /* ASCII range 32–126 */
    }
    /* Copy Chinese codepoints */
    memcpy(allCodepoints + asciiCount, codepoints, codepointCount * sizeof(int));
    UnloadCodepoints(codepoints);

    /* Load font with combined ASCII and Chinese codepoints */
    font = LoadFontEx("/home/thomas/projects/cplusplus/matrix/resources/DotGothic16-Regular.ttf",
                      (int)(96 * ScaleFactor), allCodepoints, totalCodepointCount);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    free(allCodepoints);

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

    if (fullscreen)
    {
        ToggleFullscreen();
    }

    while (!WindowShouldClose())
    {
        if (IsWindowResized())
        {
            HandleResize();
        }
        if (IsKeyPressed(KEY_SPACE))
        {
            pause = !pause;
        }
        if (IsKeyPressed(KEY_M))
        {
            ChineseMode = !ChineseMode;
            InitStreams();
        }

        UpdateGame();
        DrawGame();
    }

    UnloadFont(font);
    UnloadTexture(background);
    CloseWindow();
    return 0;
}

void InitStreams(void)
{
    /* StreamCount = ScreenWidth / 20; */
    StreamCount = GetScreenWidth() / (int)(20 * ScaleFactor);

    /* Load Chinese codepoints for character selection */
    int codepointCount = 0;
    int *codepoints = LoadCodepoints(chineseText, &codepointCount);

    for (int i = 0; i < StreamCount; i++)
    {
        /* streams[i].x = i * 20; */
        streams[i].x = i * (20 * ScaleFactor);
        streams[i].y = (float)(rand() % ScreenHeight);
        streams[i].speed = (float)(rand() % 5 + 2);
        streams[i].length = rand() % 20 + 10;
        streams[i].active = true;

        if (ChineseMode)
        {
            /* Fill with Chinese characters (codepoints) */
            for (int j = 0; j < streams[i].length; j++)
            {
                streams[i].codepoints[j] = codepoints[rand() % codepointCount];
                streams[i].characters[j] = '\0'; /* Not used in Chinese mode */
            }
        }
        else
        {
            /* Fill with random printable ASCII characters */
            for (int j = 0; j < streams[i].length; j++)
            {
                streams[i].characters[j] = (char)(rand() % 94 + 33); /* ASCII 33-126 */
                streams[i].codepoints[j] = 0;                        /* Not used in ASCII mode */
            }
        }
    }

    UnloadCodepoints(codepoints);
}

void UpdateStreams(void)
{
    /* Load Chinese codepoints for character updates */
    int codepointCount = 0;
    int *codepoints = LoadCodepoints(chineseText, &codepointCount);

    for (int i = 0; i < StreamCount; i++)
    {
        if (streams[i].active)
        {
            /* Update stream position */
            streams[i].y += streams[i].speed;

            /* Reset stream if it goes off screen */
            /* if (streams[i].y - streams[i].length * 35 > ScreenHeight) */
            if (streams[i].y - streams[i].length * (35 * ScaleFactor) > ScreenHeight)
            {
                streams[i].y = 0;
                streams[i].length = rand() % 20 + 10;
                if (ChineseMode)
                {
                    for (int j = 0; j < streams[i].length; j++)
                    {
                        streams[i].codepoints[j] = codepoints[rand() % codepointCount];
                        streams[i].characters[j] = '\0';
                    }
                }
                else
                {
                    for (int j = 0; j < streams[i].length; j++)
                    {
                        streams[i].characters[j] = (char)(rand() % 94 + 33);
                        streams[i].codepoints[j] = 0;
                    }
                }
            }

            /* Occasionally change a character in the stream */
            if (rand() % 100 < 10)
            {
                int charIndex = rand() % streams[i].length;
                if (ChineseMode)
                {
                    streams[i].codepoints[charIndex] = codepoints[rand() % codepointCount];
                    streams[i].characters[charIndex] = '\0';
                }
                else
                {
                    streams[i].characters[charIndex] = (char)(rand() % 94 + 33);
                    streams[i].codepoints[charIndex] = 0;
                }
            }
        }
    }

    UnloadCodepoints(codepoints);
}

void UpdateStreamColor(void)
{
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

void UpdateStreamSpeed(void)
{
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
}

void UpdateGame(void)
{
    if (pause)
    {
        return;
    }
    if (IsKeyPressed(KEY_F))
    {
        ToggleFullscreen();
    }

    UpdateStreams();
    UpdateStreamColor();
    UpdateStreamSpeed();
}

void DrawGame(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    /* This draws the background if you uncomment one of the "LoadTexture" comments in main */
    DrawTexturePro(background, {0, 0, (float)background.width, (float)background.height},
                   {0, 0, (float)ScreenWidth, (float)ScreenHeight}, {0, 0}, 0.0f, WHITE);

    for (int i = 0; i < StreamCount; i++)
    {
        if (streams[i].active)
        {
            Color baseColor = RandomColorMode ? colors[i % ColorCount] : colors[SelectedColorIndex];

            for (int j = 0; j < streams[i].length; j++)
            {
                /* Calculate fading color (brighter at the top) */
                int alpha = 255 - (j * 255 / streams[i].length);
                Color color = {baseColor.r, baseColor.g, baseColor.b, (unsigned char)alpha};

                if (ChineseMode)
                {
                    /* Convert codepoint to UTF-8 string for drawing */
                    int utf8Size = 0;
                    int codepoint = streams[i].codepoints[j];
                    if (codepoint >= 0 && codepoint <= 0x10FFFF) /* Validate codepoint */
                    {
                        const char *utf8Char = CodepointToUTF8(codepoint, &utf8Size);
                        if (utf8Size > 0) /* Check for successful encoding */
                        {
                            DrawTextEx(font, utf8Char,
                                       (Vector2){.x = streams[i].x, .y = streams[i].y - j * (20 * ScaleFactor)},
                                       BaseFontSize * ScaleFactor, 0, color);
                        }
                    }
                }
                else
                {
                    char text[2] = {streams[i].characters[j], '\0'};
                    DrawTextEx(font, text, (Vector2){.x = streams[i].x, .y = streams[i].y - j * (20 * ScaleFactor)},
                               BaseFontSize * ScaleFactor, 0, color);
                }
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

void HandleResize(void)
{
    ScreenWidth = GetScreenWidth();
    ScreenHeight = GetScreenHeight();

    /* Calculate scale factor based on reference resolution (1500x900) */
    ScaleFactor = (float)ScreenWidth / 1500.0f;

    /* Reload font with Chinese and ASCII character support */
    int codepointCount = 0;
    int *codepoints = LoadCodepoints(chineseText, &codepointCount);

    /* Add ASCII codepoints (32–126) */
    int asciiCount = 95; /* 32 to 126 inclusive */
    int totalCodepointCount = codepointCount + asciiCount;
    int *allCodepoints = (int *)malloc(totalCodepointCount * sizeof(int));

    /* Copy ASCII codepoints */
    for (int i = 0; i < asciiCount; i++)
    {
        allCodepoints[i] = 32 + i; /* ASCII range 32–126 */
    }
    /* Copy Chinese codepoints */
    memcpy(allCodepoints + asciiCount, codepoints, codepointCount * sizeof(int));
    UnloadCodepoints(codepoints);

    /* Reload font with combined ASCII and Chinese codepoints */
    UnloadFont(font);
    font = LoadFontEx("/home/thomas/projects/cplusplus/matrix/resources/DotGothic16-Regular.ttf",
                      (int)(96 * ScaleFactor), allCodepoints, totalCodepointCount);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    free(allCodepoints);

    InitStreams();
}
