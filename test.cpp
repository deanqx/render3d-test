#define SDL_MAIN_HANDLED

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <chrono>
#include "SDL.h"

void PrintFrameTiming(std::ostream& os = std::cout, float period = 2.0f)
{
    static unsigned int frames = 0;
    frames++;
    static auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();

    float seconds = std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count();
    if (seconds > period)
    {
        float spf = seconds / frames;
        os
            << frames << " frames in "
            << std::setprecision(1) << std::fixed << seconds << " seconds = "
            << std::setprecision(1) << std::fixed << 1.0f / spf << " FPS ("
            << std::setprecision(3) << std::fixed << spf * 1000.0f << " ms/frame)\n";
        frames = 0;
        start = end;
    }
}

int main(int, char**)
{
    const unsigned sizing = 16;
    const unsigned WIDTH = 1024 / sizing;
    const unsigned HEIGHT = 960 / sizing;


    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH * sizing, HEIGHT * sizing, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    // dump renderer info
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    std::cout << "Renderer name: " << info.name << '\n';
    std::cout << "Texture formats: " << '\n';
    for (Uint32 i = 0; i < info.num_texture_formats; i++)
    {
        std::cout << SDL_GetPixelFormatName(info.texture_formats[i]) << '\n';
    }

    // create screen
    SDL_Texture* screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    // main loop
    bool running = true;
    while (running)
    {
        std::vector< unsigned char > pixels(WIDTH * HEIGHT * 4, 0);

        // handle events
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if ((SDL_QUIT == ev.type) ||
                (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode))
            {
                running = false;
                break;
            }
        }

        // splat down some random pixels
        for (unsigned int i = 0; i < 16384; i++)
        {
            const unsigned x = rand() % WIDTH;
            const unsigned y = rand() % HEIGHT;

            const unsigned offset = (WIDTH * y * 4) + x * 4;
            pixels[offset + 0] = rand() % 256;        // b
            pixels[offset + 1] = rand() % 256;        // g
            pixels[offset + 2] = rand() % 256;        // r
            pixels[offset + 3] = 255;                 // a
        }

        SDL_UpdateTexture(screen, nullptr, pixels.data(), WIDTH * 4);
        SDL_RenderCopy(renderer, screen, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        PrintFrameTiming();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}