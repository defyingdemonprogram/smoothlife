#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define HEIGHT 100
#define WIDTH 100

float grid[HEIGHT][WIDTH] = {0};
float grid_diff[HEIGHT][WIDTH] = {0};
float ra = 11.0f;
float alpha_n = 0.028f;
float alpha_m = 0.147f;
float b1 = 0.278f;
float b2 = 0.365f;
float d1 = 0.267f;
float d2 = 0.445f;
float dt = 0.05f;
float M = 0.0f;
float N = 0.0f;

// Define window size (scaling each cell)
#define CELL_SIZE 10
#define WIN_WIDTH (WIDTH * CELL_SIZE)
#define WIN_HEIGHT (HEIGHT * CELL_SIZE)

// Background color (#181818)
#define BACKGROUND_COLOR (Color){0x18, 0x18, 0x18, 255}

float rand_float(void) {
    return (float)rand() / (float)RAND_MAX;
}

void random_grid(void) {
    size_t w = WIDTH / 2;
    size_t h = HEIGHT / 2;
    for (size_t dy = 0; dy < h; ++dy) {
        for (size_t dx = 0; dx < w; ++dx) {
            size_t x = dx + WIDTH / 2 - w / 2;
            size_t y = dy + HEIGHT / 2 - h / 2;
            grid[y][x] = rand_float();
        }
    }
}

int emod(int a, int b) {
    return (a % b + b) % b;
}

float sigma(float x, float a, float alpha) {
    return 1.0f / (1.0f + expf(-(x - a) * 4 / alpha));
}

float sigma_n(float x, float a, float b) {
    return sigma(x, a, alpha_n) * (1.0f - sigma(x, b, alpha_n));
}

float sigma_m(float x, float y, float m) {
    return x * (1.0f - sigma(m, 0.5f, alpha_m)) + y * sigma(m, 0.5f, alpha_m);
}

float s(float n, float m) {
    return sigma_n(n, sigma_m(b1, d1, m), sigma_m(b2, d2, m));
}

void clamp(float *x, float l, float h) {
    if (*x < l) *x = l;
    if (*x > h) *x = h;
}

void compute_grid_diff(void) {
    for (int cy = 0; cy < HEIGHT; ++cy) {
        for (int cx = 0; cx < WIDTH; ++cx) {
            float m = 0.0f;
            float n = 0.0f;
            float ri = ra / 3.0f;

            int range = (int)ceil(ra);
            for (int dy = -range; dy <= range; ++dy) {
                for (int dx = -range; dx <= range; ++dx) {
                    int x = emod(cx + dx, WIDTH);
                    int y = emod(cy + dy, HEIGHT);
                    float dist_sq = (float)(dx * dx + dy * dy);
                    if (dist_sq <= ri * ri) {
                        m += grid[y][x];
                    } else if (dist_sq <= ra * ra) {
                        n += grid[y][x];
                    }
                }
            }
            m = (M > 0.0f) ? m / M : 0.0f;
            n = (N > 0.0f) ? n / N : 0.0f;
            float q = s(n, m);
            grid_diff[cy][cx] = 2.0f * q - 1.0f;
        }
    }
}

void apply_grid_diff(void) {
    for (size_t y = 0; y < HEIGHT; ++y) {
        for (size_t x = 0; x < WIDTH; ++x) {
            grid[y][x] += dt * grid_diff[y][x];
            clamp(&grid[y][x], 0.0f, 1.0f);
        }
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    // Precompute normalization factors
    float ri = ra / 3.0f;
    int range = (int)ceil(ra);
    for (int dy = -range; dy <= range; ++dy) {
        for (int dx = -range; dx <= range; ++dx) {
            float dist_sq = (float)(dx * dx + dy * dy);
            if (dist_sq <= ri * ri) {
                M += 1.0f;
            } else if (dist_sq <= ra * ra) {
                N += 1.0f;
            }
        }
    }

    random_grid();

    InitWindow(WIN_WIDTH, WIN_HEIGHT, "SmoothLife Raylib");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) break;

        compute_grid_diff();
        apply_grid_diff();

        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                float v = grid[y][x];
                Color c = (Color){(unsigned char)(v * 255), (unsigned char)(v * 255), (unsigned char)(v * 255), 255};
                DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, c);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

