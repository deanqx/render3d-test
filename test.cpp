#include <iostream>
#include <vector>
#include <math.h>

void SDL_SetRenderDrawColor(int ren, int r, int g, int b, int a)
{
    printf("r=%i; g=%i; b=%i\n", r, g, b);
}
void SDL_RenderDrawPoint(int r, int x, int y)
{
    printf("x=%i; y=%i\n", x, y);
}

int renderer = 0;


struct vec2
{
    int x, y;
};

std::vector<vec2> bresenham(int startX, int startY, int endX, int endY)
{

    vec2 p;
    p.x = startX; // positionX
    p.y = startY; // positionX
    float dx = (float)(std::abs(endX) - std::abs(startX)); // deltaX
    float dy = (float)(std::abs(endY) - std::abs(startY)); // deltaY

    int dirX, dirY; // directionX, directionY
    if (dx >= 0.0f)
        dirX = 1;
    else
        dirX = -1;
    if (dy >= 0.0f)
        dirY = 1;
    else
        dirY = -1;

    // TODO Calculate number of positions
    std::vector<vec2> pos; // positions 

    dx = std::abs(dx);
    dy = std::abs(dy);

    pos.push_back(p);

    if (dx < dy)
    {
        pos.reserve(dy);

        p.y += dirY;

        float error = dy * 0.5f;
        for (float i = 0; i < dy; ++i, p.y += dirY)
        {
            error -= dx;
            if (error >= 0.0f)
            {
                pos.push_back(p);
            }
            else
            {
                p.x += dirX;
                pos.push_back(p);
                error += dy;
            }
        }
    }
    else
    {
        pos.reserve(dx);

        p.x += dirX;

        float error = dy * 0.5f;
        for (float i = 0; i < dx; ++i, p.x += dirX)
        {
            error -= dy;
            if (error >= 0.0f)
            {
                pos.push_back(p);
            }
            else
            {
                p.y += dirY;
                pos.push_back(p);
                error += dx;
            }
        }
    }

    return pos;
}

void DrawInterpolation(int r0, int g0, int b0, int r1, int g1, int b1, int startX, int startY, int endX, int endY)
{
    SDL_SetRenderDrawColor(renderer, r0, g0, b0, 255);
    SDL_RenderDrawPoint(renderer, startX, startY);
    SDL_SetRenderDrawColor(renderer, r1, g1, b1, 255);
    SDL_RenderDrawPoint(renderer, endX, endY);

    std::vector<vec2> bresen = bresenham(startX, startY, endX, endY);

    float total = (float)bresen.size() - 1.0f;
    float A = total - 1.0f;
    int B = 1;
    float p0;
    float p1;
    for (int r, g, b; A > 0.0f; --A, ++B)
    {
        p0 = A / total;
        p1 = B / total;
        r = (int)(p0 * (float)r0 + p1 * (float)r1);
        g = (int)(p0 * (float)g0 + p1 * (float)g1);
        b = (int)(p0 * (float)b0 + p1 * (float)b1);

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawPoint(renderer, bresen[B].x, bresen[B].y);
    }
}

int main()
{
    DrawInterpolation(0, 102, 255, 255, 0, 0, 50, 50, 57, 50);

    return 0;
}