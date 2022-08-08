#include <vector>
#include <math.h>
#include <iostream>
#include "SDL.h"
#include "gmath.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

int WIDTH;
int HEIGHT;

void INIT(SDL_Window* win, SDL_Renderer* ren, int w, int h)
{
    window = win;
    renderer = ren;
    WIDTH = w;
    HEIGHT = h;
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

std::vector<vec2> bresenham(vec2 begin, vec2 end)
{
    vec2 p;
    p.x = begin.x; // positionX
    p.y = begin.y; // positionX
    float dx = (float)(std::abs(end.x) - std::abs(begin.x)); // deltaX
    float dy = (float)(std::abs(end.y) - std::abs(begin.y)); // deltaY

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

        float error = dx * 0.5f;
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

// @return colors[y][x]
void DrawLine(color c0, color c1, vec2 begin, vec2 end, int& height)
{
    int top;
    int bottom;
    if (begin.y > end.y) bottom = begin.y;
    if (end.y > begin.y) bottom = end.y;
    if (begin.y < end.y) top = begin.y;
    if (end.y < begin.y) top = end.y;

    height = bottom - top + 1;

    std::vector<std::vector<color2*>> colors(HEIGHT, std::vector<color2*>(WIDTH));
    std::vector<vec2> bresen = bresenham(begin, end);

    float total = (float)bresen.size() - 1.0f;
    float A = total - 1.0f;
    int B = 1;
    float p0;
    float p1;
    for (int r, g, b; A > 0.0f; --A, ++B)
    {
        p0 = A / total;
        p1 = B / total;
        r = (int)(p0 * (float)c0.r + p1 * (float)c1.r);
        g = (int)(p0 * (float)c0.g + p1 * (float)c1.g);
        b = (int)(p0 * (float)c0.b + p1 * (float)c1.b);

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawPoint(renderer, bresen[B].x, bresen[B].y);
        colors[bresen[B].y][bresen[B].x] = new color2{ { bresen[B].x, bresen[B].y }, { r, g, b } };
    }

    return colors;
}

void DrawLine(color c, vec2 begin, vec2 end)
{
    int& px = begin.x; // positionX
    int& py = begin.y; // positionX
    float dx = (float)(std::abs(end.x) - std::abs(begin.x)); // deltaX, deltaY
    float dy = (float)(std::abs(end.y) - std::abs(begin.y));

    int dirX, dirY; // directionX, directionY
    if (dx >= 0.0f)
        dirX = 1;
    else
        dirX = -1;
    if (dy >= 0.0f)
        dirY = 1;
    else
        dirY = -1;

    dx = std::abs(dx);
    dy = std::abs(dy);

    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
    SDL_RenderDrawPoint(renderer, px, py);

    if (dx < dy)
    {
        py += dirY;

        float error = dy * 0.5f;
        for (float i = 0; i < dy; ++i, py += dirY)
        {
            error -= dx;
            if (error >= 0.0f)
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

        float error = dx * 0.5f;
        for (float i = 0; i < dx; ++i, px += dirX)
        {
            error -= dy;
            if (error >= 0.0f)
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

void DrawTriangle(color c, vec2 v0, vec2 v1, vec2 v2)
{
    DrawLine(c, v0, v1);
    DrawLine(c, v1, v2);
    DrawLine(c, v2, v0);
}

void FillTriangle(color c0, color c1, color c2, vec2 v0, vec2 v1, vec2 v2)
{
    int height0, height1, height2;
    std::vector<std::vector<color2*>> colors(HEIGHT, std::vector<color2*>(WIDTH));
    DrawLine(c0, c0, v0, v1, height0); // TODO Delete
    DrawLine(c1, c1, v1, v2, height1);
    DrawLine(c2, c2, v2, v0, height2);

    // Scanline Algorithm

    int height;
    if (height0 > height1 && height0 > height2) height = height0;
    if (height1 > height2 && height1 > height0) height = height1;
    if (height2 > height0 && height2 > height1) height = height2;

    std::vector<std::vector<color2*>> all_x_on_y(HEIGHT); // all_x_on_y[y][x]

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            if (colors0[y][x] != nullptr)
            {
                all_x_on_y[y].push_back(colors0[y][x]);
            }
            else
                if (colors1[y][x] != nullptr)
                {
                    all_x_on_y[y].push_back(colors1[y][x]);
                }
                else
                    if (colors2[y][x] != nullptr)
                    {
                        all_x_on_y[y].push_back(colors2[y][x]);
                    }
        }
    }

    for (int y = 0; y < HEIGHT; ++y)
    {
        if (all_x_on_y[y].size() > 0)
        {
            color2* last1 = all_x_on_y[y][all_x_on_y[y].size() - 2];
            color2* last2 = all_x_on_y[y][all_x_on_y[y].size() - 1];

            int h;
            DrawLine(last1->c, last2->c, last1->pos, last2->pos, h);
        }
    }
}