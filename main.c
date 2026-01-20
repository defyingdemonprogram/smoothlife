#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define HEIGHT 100
#define WIDTH 100

// Define proper struct to hold simulation state
typedef struct {
    float grid[HEIGHT][WIDTH];
    float grid_diff[HEIGHT][WIDTH];
    float ra;      // Radius for the outer neighborhood
    float alpha_n; // Width of the birth/survival interval
    float alpha_m; // Width of the threshold transitions
    float b1;      // Birth interval start
    float b2;      // Birth interval end
    float d1;      // Death interval start
    float d2;      // Death interval end
    float dt;      // Time step size
    float M;       // Normalization area factor for inner disk
    float N;       // Normalization area factor for outer annulus
} SmoothLife;

// Define window size (scaling each cell)
#define CELL_SIZE 6
#define WIN_WIDTH (WIDTH * CELL_SIZE)
#define WIN_HEIGHT (HEIGHT * CELL_SIZE)

// Background color (#181818)
#define BACKGROUND_COLOR (Color){0x18, 0x18, 0x18, 255}

float rand_float(void) {
    return (float)rand() / (float)RAND_MAX;
}

void init_smoothlife(SmoothLife *sl) {
    // Standard SmoothLife parameter set
    sl->ra = 11.0f;
    sl->alpha_n = 0.028f;
    sl->alpha_m = 0.147f;
    sl->b1 = 0.278f;
    sl->b2 = 0.365f;
    sl->d1 = 0.267f;
    sl->d2 = 0.445f;
    sl->dt = 0.025f;
    sl->M = 0.0f;
    sl->N = 0.0f;

    // Precompute normalization factors
    float ri = sl->ra / 3.0f;
    int range = (int)ceil(sl->ra);
    
    for (int dy = -range; dy <= range; ++dy) {
        for (int dx = -range; dx <= range; ++dx) {
            float dist_sq = (float)(dx * dx + dy * dy);
            if (dist_sq <= ri * ri) {
                sl->M += 1.0f;
            } else if (dist_sq <= sl->ra * sl->ra) {
                sl->N += 1.0f;
            }
        }
    }

    // Initialize random grid
    // Use a separate function so we can re-randomize later
    extern void randomize_grid(SmoothLife *sl); // Forward declaration
    randomize_grid(sl);
}

void randomize_grid(SmoothLife *sl) {
    size_t w = WIDTH / 2;
    size_t h = HEIGHT / 2;
    for (size_t y = 0; y < HEIGHT; ++y) {
        for (size_t x = 0; x < WIDTH; ++x) {
            sl->grid[y][x] = 0.0f;
        }
    }

    for (size_t dy = 0; dy < h; ++dy) {
        for (size_t dx = 0; dx < w; ++dx) {
            size_t x = dx + WIDTH / 2 - w / 2;
            size_t y = dy + HEIGHT / 2 - h / 2;
            sl->grid[y][x] = rand_float();
        }
    }
}

int emod(int a, int b) {
    return (a % b + b) % b;
}

float sigma(float x, float a, float alpha) {
    return 1.0f / (1.0f + expf(-(x - a) * 4 / alpha));
}

float sigma_n(float x, float a, float b, float alpha_n) {
    return sigma(x, a, alpha_n) * (1.0f - sigma(x, b, alpha_n));
}

float sigma_m(float x, float y, float m, float alpha_m) {
    return x * (1.0f - sigma(m, 0.5f, alpha_m)) + y * sigma(m, 0.5f, alpha_m);
}

// Transition function s(n, m)
float s(float n, float m, SmoothLife *sl) {
    return sigma_n(n, sigma_m(sl->b1, sl->d1, m, sl->alpha_m), sigma_m(sl->b2, sl->d2, m, sl->alpha_m), sl->alpha_n);
}

void clamp(float *x, float l, float h) {
    if (*x < l) *x = l;
    if (*x > h) *x = h;
}

void compute_grid_diff(SmoothLife *sl) {
    for (int cy = 0; cy < HEIGHT; ++cy) {
        for (int cx = 0; cx < WIDTH; ++cx) {
            float m = 0.0f;
            float n = 0.0f;
            float ri = sl->ra / 3.0f;

            int range = (int)ceil(sl->ra);
            for (int dy = -range; dy <= range; ++dy) {
                for (int dx = -range; dx <= range; ++dx) {
                    int x = emod(cx + dx, WIDTH);
                    int y = emod(cy + dy, HEIGHT);
                    float dist_sq = (float)(dx * dx + dy * dy);
                    if (dist_sq <= ri * ri) {
                        m += sl->grid[y][x];
                    } else if (dist_sq <= sl->ra * sl->ra) {
                        n += sl->grid[y][x];
                    }
                }
            }
            m = (sl->M > 0.0f) ? m / sl->M : 0.0f;
            n = (sl->N > 0.0f) ? n / sl->N : 0.0f;
            float q = s(n, m, sl);
            sl->grid_diff[cy][cx] = 2.0f * q - 1.0f;
        }
    }
}

void apply_grid_diff(SmoothLife *sl) {
    for (size_t y = 0; y < HEIGHT; ++y) {
        for (size_t x = 0; x < WIDTH; ++x) {
            sl->grid[y][x] += sl->dt * sl->grid_diff[y][x];
            clamp(&sl->grid[y][x], 0.0f, 1.0f);
        }
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    // Allocate on heap to avoid stack overflow if sizes increase
    SmoothLife *sl = (SmoothLife *)malloc(sizeof(SmoothLife));
    if (!sl) {
        fprintf(stderr, "Failed to allocate memory for SmoothLife\n");
        return 1;
    }
    
    init_smoothlife(sl);

    InitWindow(WIN_WIDTH, WIN_HEIGHT, "SmoothLife Raylib");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) break;

        // Reset grid on 'R' press
        if (IsKeyPressed(KEY_R)) {
            randomize_grid(sl);
        }

        // Draw with mouse
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            int cx = (int)(mousePos.x / CELL_SIZE);
            int cy = (int)(mousePos.y / CELL_SIZE);
            
            // Draw a small circle of life
            int radius = 1;
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int x = emod(cx + dx, WIDTH);
                    int y = emod(cy + dy, HEIGHT);
                    sl->grid[y][x] = 1.0f;
                }
            }
        }

        // Run physics with sub-stepping for stability
        // 2 steps of 0.025 instead of 1 step of 0.05
        for (int step = 0; step < 2; step++) {
            compute_grid_diff(sl);
            apply_grid_diff(sl);
        }

        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                float v = sl->grid[y][x];
                Color c = (Color){(unsigned char)(v * 255), (unsigned char)(v * 255), (unsigned char)(v * 255), 255};
                DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, c);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    free(sl);
    return 0;
}

