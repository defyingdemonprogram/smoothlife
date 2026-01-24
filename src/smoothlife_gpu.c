#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "raylib.h"

#define FACTOR          100
#define SCREEN_WIDTH    (16 * FACTOR)
#define SCREEN_HEIGHT   (9 * FACTOR)
#define SCALAR          0.2
#define TEXTURE_WIDTH   (SCREEN_WIDTH * SCALAR)
#define TEXTURE_HEIGHT  (SCREEN_HEIGHT * SCALAR)
#define BACKGROUND_COLOR (Color){0x18, 0x18, 0x18, 255}


float rand_float(void) {
    return (float)rand()/(float)RAND_MAX;
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Smoothlife");
    SetTargetFPS(60);

    Image image = GenImageColor(TEXTURE_WIDTH, TEXTURE_HEIGHT, BLACK);
    for (int y = 0; y < TEXTURE_HEIGHT * 3/4; ++y) {
        for (int x = 0; x < TEXTURE_WIDTH * 3/4; ++x) {
            uint8_t v = rand_float()*255.0f;
            Color color = { v, v, v, 255 };
            ImageDrawPixel(&image, x, y, color);
        }
    }
    RenderTexture2D state[2];

    state[0] = LoadRenderTexture(TEXTURE_WIDTH, TEXTURE_HEIGHT);
    SetTextureWrap(state[0].texture, TEXTURE_WRAP_REPEAT);
    SetTextureFilter(state[0].texture, TEXTURE_FILTER_BILINEAR);
    UpdateTexture(state[0].texture, image.data);

    state[1] = LoadRenderTexture(TEXTURE_WIDTH, TEXTURE_HEIGHT);
    SetTextureWrap(state[1].texture, TEXTURE_WRAP_REPEAT);
    SetTextureFilter(state[1].texture, TEXTURE_FILTER_BILINEAR);
    UpdateTexture(state[1].texture, image.data);    

    Shader shader = LoadShader(NULL, "./smoothlife.fs");
    Vector2 resolution = { TEXTURE_WIDTH, TEXTURE_HEIGHT };
    int resolution_loc = GetShaderLocation(shader, "resolution");
    SetShaderValue(shader, resolution_loc, &resolution, SHADER_UNIFORM_VEC2);

    size_t i = 0;
    while (!WindowShouldClose()) {
        BeginTextureMode(state[1 - i]);
            ClearBackground(BACKGROUND_COLOR);
            BeginShaderMode(shader);
                DrawTexture(state[i].texture, 0, 0, WHITE);
            EndShaderMode();
        EndTextureMode();

        i = 1 - i;
        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            DrawTextureEx(state[i].texture, CLITERAL(Vector2){0}, 0, 1/SCALAR, WHITE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}