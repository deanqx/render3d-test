#include <iostream>
#include <math.h>

void SDL_SetRenderDrawColor(int ren, int r, int g, int b, int a)
{
}
void SDL_RenderDrawPoint(int r, int x, int y)
{
    printf("x=%i; y=%i\n", x, y);
}

int renderer = 0;

#define ABSOLUTE(x) int temp = x >> 31; x ^= temp; x += temp & 1
void DrawLine(int r, int g, int b, int startX, int startY, int endX, int endY)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    int px = (int)startX;
    int py = (int)startY;
    float dx = (float)std::abs(endX - startX);
    float dy = (float)std::abs(endY - startY);


    SDL_RenderDrawPoint(renderer, px, py);

    if (dx < dy)
    {
        ++py;
        float error = dy * 0.5f;
        int _dy = (int)dy;
        ABSOLUTE(_dy);
        for (int i = 0; i < _dy; ++i, ++py)
        {
            error -= dx;
            if (error > 0.0f)
            {
                SDL_RenderDrawPoint(renderer, px, py);
            }
            else
            {
                ++px;
                SDL_RenderDrawPoint(renderer, px, py);
                error += dy;
            }
        }
    }
    else
    {
        ++px;
        float error = dx * 0.5f;
        int _dx = (int)dx;
        ABSOLUTE(_dx); // use on all
        for (int i = 0; i < _dx; ++i, ++px)
        {
            error -= dy;
            if (error > 0.0f)
            {
                SDL_RenderDrawPoint(renderer, px, py);
            }
            else
            {
                ++py;
                SDL_RenderDrawPoint(renderer, px, py);
                error += dx;
            }
        }
    }
}

int main()
{
    DrawLine(255, 255, 255, 50, 50, 0, 0);
    return 0;
    DrawLine(255, 255, 255, 50, 50, 100, 0);
    DrawLine(255, 255, 255, 50, 50, 100, 100);
    DrawLine(255, 255, 255, 50, 50, 0, 100);

}