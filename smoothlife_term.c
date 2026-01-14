#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define HEIGHT 100
#define WIDTH 100

char level[] = " .-=coaA@#";
#define level_count (sizeof(level)/sizeof(level[0]) - 1)

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


float rand_float(void) {
    return (float)rand() / (float)RAND_MAX;
}

void random_grid(void) {
    size_t w = WIDTH/2;
    size_t h = HEIGHT/2;
    for (size_t dy = 0; dy < h; ++dy) {
        for (size_t dx = 0; dx < w; ++dx) {
            size_t x = dx + WIDTH/2 - w/2;
            size_t y = dy + HEIGHT/2 - h/2;
            grid[y][x] = rand_float();
        }
    }
}

void display_grid(float grid[HEIGHT][WIDTH]) {
    for (size_t y = 0; y < HEIGHT; ++y) {
        for (size_t x = 0; x < WIDTH; ++x) {
            int index = (int)(grid[y][x] * (level_count - 1));
            if (index < 0) index = 0;
            if (index >= (int)level_count) index = level_count - 1;
            char c = level[index];
            fputc(c, stdout);
            fputc(c, stdout);
        }
        fputc('\n', stdout);
    }
    fflush(stdout);
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
    display_grid(grid);

    for (;;) {
        compute_grid_diff();
        apply_grid_diff();
        display_grid(grid);
    }
    return 0;
}
