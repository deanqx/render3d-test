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

void DrawLine(int r, int g, int b, int startX, int startY, int endX, int endY)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    int px = (int)startX; // positionX
    int py = (int)startY; // positionX
    float dx = (float)(std::abs(endX) - std::abs(startX)); // deltaX, deltaY
    float dy = (float)(std::abs(endY) - std::abs(startY));

    int dirX, dirY; // directionX, directionY
    if (dx >= 0)
        dirX = 1;
    else
        dirX = -1;
    if (dy >= 0)
        dirY = 1;
    else
        dirY = -1;

    dx = std::abs(dx);
    dy = std::abs(dy);

    SDL_RenderDrawPoint(renderer, px, py);

    if (dx < dy)
    {
        py += dirY;
        float error = dy * 0.5f;
        for (float i = 0; i < dy; ++i, py += dirY)
        {
            error -= dx;
            if (error > 0.0f)
            {
                SDL_RenderDrawPoint(renderer, px, py);
            }
            else
            {
                px += dirX;
                SDL_RenderDrawPoint(renderer, px, py);
                error += dy;
            }
        }
    }
    else
    {
        px += dirX;
        float error = dy * 0.5f;
        for (float i = 0; i < dx; ++i, px += dirX)
        {
            error -= dy;
            if (error > 0.0f)
            {
                SDL_RenderDrawPoint(renderer, px, py);
            }
            else
            {
                py += dirY;
                SDL_RenderDrawPoint(renderer, px, py);
                error += dx;
            }
        }
    }
}

int main()
{
    DrawLine(255, 255, 255, 50, 50, 0, 0);
    DrawLine(255, 255, 255, 50, 50, 0, 100);
    DrawLine(255, 255, 255, 50, 50, 100, 0);
    DrawLine(255, 255, 255, 50, 50, 100, 100);

    return 0;
}