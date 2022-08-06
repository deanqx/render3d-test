#include <vector>
#include <math.h>
#include "SDL.h"
#include "gmath.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

void INIT(SDL_Window* w, SDL_Renderer* r)
{
    window = w;
    renderer = r;
}

float vec3::lenght()
{
    return sqrtf(x * x + y * y + z * z);
}

void vec3::normalize()
{
    float l = sqrtf(x * x + y * y + z * z);
    x /= l; y /= l; z /= l;
}

float vec3::dot(const vec3& v)
{
    return x * v.x + y * v.y + z * v.z;
}

vec3 vec3::operator+(const vec3& v)
{
    vec3 o;

    o.x = x + v.x;
    o.y = y + v.y;
    o.z = z + v.z;

    return o;
}

vec3 vec3::operator-(const vec3& v)
{
    vec3 o;

    o.x = x - v.x;
    o.y = y - v.y;
    o.z = z - v.z;

    return o;
}

vec3 vec3::operator*(const mat4x4& m)
{
    vec3 o;

    o.x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + m.m[3][0];
    o.y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + m.m[3][1];
    o.z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + m.m[3][2];
    float w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f) // Normalize by w
    {
        o.x /= w;
        o.y /= w;
        o.z /= w;
    }

    return o;
}

void mesh::rescale(float scaler)
{
    scale = { scaler, scaler, scaler };
}

void DrawLine(int r, int g, int b, int startX, int startY, int endX, int endY)
{
    int& px = startX; // positionX
    int& py = startY; // positionX
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

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
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

void DrawTriangle(int r, int g, int b, float x0, float y0, float x1, float y1, float x2, float y2)
{
    DrawLine(r, g, b, (int)x0, (int)y0, (int)x1, (int)y1);
    DrawLine(r, g, b, (int)x1, (int)y1, (int)x2, (int)y2);
    DrawLine(r, g, b, (int)x2, (int)y2, (int)x0, (int)y0);
}