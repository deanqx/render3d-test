#include <vector>
#include <math.h>
#include <iostream>
#include "SDL.h"
#include "gmath.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

int WIDTH;
int HEIGHT;

Timer* t2;

void INIT(SDL_Window* win, SDL_Renderer* ren, int w, int h, Timer* ti)
{
    window = win;
    renderer = ren;
    WIDTH = w;
    HEIGHT = h;
    t2 = ti;
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
    Timer::perf performance(t2, "besenham");


    vec2 p;
    p.x = begin.x; // positionX
    p.y = begin.y; // positionY
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
            if (error > 0.0f)
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
            if (error > 0.0f)
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

void DrawLine(color c, vec2 begin, vec2 end)
{
    Timer::perf performance(t2, "DrawLine(color, vec2, vec2)");


    int& px = begin.x; // positionX
    int& py = begin.y; // positionY
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

        float error = dx * 0.5f;
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

// @return colors[y][x]
void DrawLine(color c0, color c1, vec2 begin, vec2 end, std::vector<std::vector<color2*>>& colors)
{
    std::vector<vec2> bresen = bresenham(begin, end);
    const int last = bresen.size() - 1;

    Timer::perf performance(t2, "DrawLine(color, color, vec2, vec2)");

    SDL_SetRenderDrawColor(renderer, c0.r, c0.g, c0.b, 255);
    SDL_RenderDrawPoint(renderer, bresen[0].x, bresen[0].y);
    colors[bresen[0].y][bresen[0].x] = new color2{ { bresen[0].x, bresen[0].y }, { c0.r, c0.g, c0.b } };
    SDL_SetRenderDrawColor(renderer, c1.r, c1.g, c1.b, 255);
    SDL_RenderDrawPoint(renderer, bresen[last].x, bresen[last].y);
    colors[bresen[last].y][bresen[last].x] = new color2{ { bresen[last].x, bresen[last].y }, { c1.r, c1.g, c1.b } };

    const float total = (float)bresen.size() - 1.0f;
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
}

void DrawVerticalLine(color c0, color c1, vec2 begin, vec2 end)
{
    Timer::perf performance(t2, "DrawVerticalLine");

    const float total = (float)(end.x - begin.x);
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
        SDL_RenderDrawPoint(renderer, begin.x + B, begin.y);
    }
}

void DrawLine(color c0, color c1, vec2 begin, vec2 end)
{
    std::vector<vec2> bresen = bresenham(begin, end);
    const int last = bresen.size() - 1;

    SDL_SetRenderDrawColor(renderer, c0.r, c0.g, c0.b, 255);
    SDL_RenderDrawPoint(renderer, bresen[0].x, bresen[0].y);
    SDL_SetRenderDrawColor(renderer, c1.r, c1.g, c1.b, 255);
    SDL_RenderDrawPoint(renderer, bresen[last].x, bresen[last].y);

    const float total = (float)bresen.size() - 1.0f;
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
    Timer::perf performance(t2, "FillTriangle");

    std::vector<std::vector<color2*>> screen(HEIGHT, std::vector<color2*>(WIDTH));
    DrawLine(c0, c1, v0, v1, screen);
    DrawLine(c1, c2, v1, v2, screen);
    DrawLine(c2, c0, v2, v0, screen);

    // Scanline Algorithm

    std::vector<std::vector<color2*>> all_x_on_y(HEIGHT); // all_x_on_y[y][x]

    for (int y = 0; y < HEIGHT; ++y)
    {
        all_x_on_y[y].reserve(2);

        for (int x = 0; x < WIDTH; ++x)
        {
            if (screen[y][x] != nullptr)
            {
                all_x_on_y[y].push_back(screen[y][x]);
            }
        }

        if (all_x_on_y[y].size() == 1)
        {
            all_x_on_y[y].push_back(all_x_on_y[y][0]);
        }
    }

    color2* x0 = nullptr;
    color2* x1 = nullptr;
    for (int y = 0; y < HEIGHT; ++y)
    {
        if (all_x_on_y[y].size() > 0)
        {
            x0 = all_x_on_y[y][0];
            x1 = all_x_on_y[y][all_x_on_y[y].size() - 1];

            DrawVerticalLine(x0->c, x1->c, x0->pos, x1->pos);
        }
    }

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            delete screen[y][x];
        }
    }
}