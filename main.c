#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_PIXELS (1024 * 1024)

SDL_Color palette[16] = {
    {0x00, 0x00, 0x00, 255}, // a Black
    {0x80, 0x00, 0x00, 255}, // b Maroon
    {0x00, 0x80, 0x00, 255}, // c Green
    {0x80, 0x80, 0x00, 255}, // d Olive
    {0x00, 0x00, 0x80, 255}, // e Navy
    {0x80, 0x00, 0x80, 255}, // f Purple
    {0x00, 0x80, 0x80, 255}, // g Teal
    {0xC0, 0xC0, 0xC0, 255}, // h Silver
    {0x80, 0x80, 0x80, 255}, // i Gray
    {0xFF, 0x00, 0x00, 255}, // j Red
    {0x00, 0xFF, 0x00, 255}, // k Lime
    {0xFF, 0xFF, 0x00, 255}, // l Yellow
    {0x00, 0x00, 0xFF, 255}, // m Blue
    {0xFF, 0x00, 0xFF, 255}, // n Fuchsia
    {0x00, 0xFF, 0xFF, 255}, // o Aqua
    {0xFF, 0xFF, 0xFF, 255}  // p White
};

int load_xga(const char* filename, uint8_t pixels[MAX_PIXELS], int* w, int* h) {
    FILE* f = fopen(filename, "r");
    if (!f) return 0;

    if (fscanf(f, "%dx%d;", w, h) != 2 || *w <= 0 || *h <= 0 || *w * *h > MAX_PIXELS) {
        fclose(f);
        return 0;
    }

    int i = 0;
    while (i < (*w) * (*h)) {
        int count = 0;
        int ch = fgetc(f);
        if (isdigit(ch)) {
            count = ch - '0';
            while (isdigit(ch = fgetc(f)))
                count = count * 10 + (ch - '0');
        } else {
            count = 1;
        }

        if (ch >= 'a' && ch <= 'p') {
            uint8_t color = (uint8_t)(ch - 'a');
            for (int j = 0; j < count && i < (*w) * (*h); ++j)
                pixels[i++] = color;
        } else {
            break;
        }
    }

    fclose(f);
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s file.xga\n", argv[0]);
        return 1;
    }

    int width, height;
    uint8_t* pixels = malloc(MAX_PIXELS);
    if (!pixels || !load_xga(argv[1], pixels, &width, &height)) {
        fprintf(stderr, "Failed to load XGA file\n");
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("XGA Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    int zoom_x = WINDOW_WIDTH / width;
    int zoom_y = WINDOW_HEIGHT / height;
    int zoom = zoom_x < zoom_y ? zoom_x : zoom_y;

    int draw_width = width * zoom;
    int draw_height = height * zoom;
    int offset_x = (WINDOW_WIDTH - draw_width) / 2;
    int offset_y = (WINDOW_HEIGHT - draw_height) / 2;

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            int i = y * width + x;
            uint8_t color = pixels[i];
            if (color < 16) {
                SDL_Color c = palette[color];
                SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, 255);
                SDL_Rect rect = {
                    offset_x + x * zoom,
                    offset_y + y * zoom,
                    zoom,
                    zoom
                };
                SDL_RenderFillRect(ren, &rect);
            }
        }

    SDL_RenderPresent(ren);

    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT) break;
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    free(pixels);
    return 0;
}
