#include <math.h>
#include <iostream>
#include <sstream>
#include "render3d.h"
#include "timer.h"

#define RENDER SDL_UpdateTexture(screen, nullptr, frame, WIDTH * 4); SDL_RenderCopy(renderer, screen, nullptr, nullptr); SDL_RenderPresent(renderer);

mat4x4 mat4x4::operator*(const mat4x4& i)
{
    mat4x4 o;

    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            for (int j = 0; j < 4; ++j)
            {
                o.m[x][y] += m[j][y] * i.m[x][j];
            }
        }
    }

    // o.m[0][0] += m[0][0] * i.m[0][0];
    // o.m[0][0] += m[1][0] * i.m[0][1];
    // o.m[0][0] += m[2][0] * i.m[0][2];

    // o.m[1][0] += m[0][0] * i.m[1][0];
    // o.m[1][0] += m[1][0] * i.m[1][1];
    // o.m[1][0] += m[2][0] * i.m[1][2];

    // o.m[0][1] += m[0][1] * i.m[0][0];
    // o.m[0][1] += m[1][1] * i.m[0][1];
    // o.m[0][1] += m[2][1] * i.m[0][2];

    return o;
}

// TODO Add const and inline
float vec3::lenght()
{
    return sqrtf(x * x + y * y + z * z);
}

void vec3::normalize()
{
    float l = 1 / sqrtf(x * x + y * y + z * z);
    if (l > 0.0f)
    {
        x *= l; y *= l; z *= l;
    }
}

vec3 vec3::normalized()
{
    vec3 o = *this;

    float l = 1 / sqrtf(x * x + y * y + z * z);
    if (l > 0.0f)
    {
        o.x *= l; o.y *= l; o.z *= l;
    }

    return o;
}

void vec3::cross(const vec3& line1, const vec3& line2)
{
    x = line1.y * line2.z - line1.z * line2.y;
    y = line1.z * line2.x - line1.x * line2.z;
    z = line1.x * line2.y - line1.y * line2.x;
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

bool mesh::loadObjectFile(std::ifstream& stream)
{
    if (stream.fail())
        return 1;

    std::vector<vec3> verts;

    while (!stream.eof())
    {
        char line[128];
        stream.getline(line, 128);

        std::stringstream ss;
        ss << line;

        char type;

        if (line[0] == 'v')
        {
            vec3 v;
            ss >> type >> v.x >> v.y >> v.z;
            verts.push_back(v);
        }

        if (line[0] == 'f')
        {
            int f[3];
            ss >> type >> f[0] >> f[1] >> f[2];
            tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
        }
    }

    return 0;
}

float vec2::dot(const vec2& v)
{
    return x * v.x + y * v.y;
}

inline void color::convert(uint8_t* frame, const int& WIDTH, int x, int y)
{
    const unsigned offset = 4 * y * WIDTH + 4 * x;
    b = frame[offset];
    g = frame[offset + 1];
    r = frame[offset + 2];
    a = frame[offset + 3];
}

std::vector<vec2> bresenham(vec2 begin, vec2 end)
{
    Timer::perf __besenham("besenham");


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

void render3d::DrawVerticalLine(color c0, color c1, vec2 begin, vec2 end)
{
    Timer::perf per("DrawVerticalLine");

    const float len = (float)end.x - begin.x;
    const float t = 1.0f / len;
    float A = len - 1.0f;
    int B = 1;
    float p0, p1;
    uint8_t r, g, b;
    for (; A > 0.0f; --A, ++B)
    {
        p0 = A * t;
        p1 = (float)B * t;
        r = (uint8_t)(p0 * (float)c0.r + p1 * (float)c1.r);
        g = (uint8_t)(p0 * (float)c0.g + p1 * (float)c1.g);
        b = (uint8_t)(p0 * (float)c0.b + p1 * (float)c1.b);

        setpixel(begin.x + B, begin.y, { r, g, b });
    }
}

inline void render3d::setpixel(const int x, const int y, const color c)
{
    setpixel(frame, x, y, c);
}

inline void render3d::setpixel(uint8_t* frame, const int x, const int y, const color c)
{
    const int offset = 4 * y * WIDTH + 4 * x;
    frame[offset] = c.b;
    frame[offset + 1] = c.g;
    frame[offset + 2] = c.r;
    frame[offset + 3] = c.a;
}

void render3d::DrawLine(color c, vec2 begin, vec2 end)
{
    Timer::perf __DrawLine1("DrawLine(color, vec2, vec2)");


    std::vector<vec2> bresen = bresenham(begin, end);

    for (int i = 0; i < bresen.size(); ++i)
    {
        setpixel(bresen[i].x, bresen[i].y, c);
    }
}

void render3d::DrawLine(color c0, color c1, vec2 begin, vec2 end)
{
    Timer::perf profile("DrawLine(c0, c1, begin, end)");

    std::vector<vec2> bresen = bresenham(begin, end);
    const int last = bresen.size() - 1;

    setpixel(bresen[0].x, bresen[0].y, c0);
    setpixel(bresen[last].x, bresen[last].y, c1);

    const float bresen_t = (float)bresen.size() - 1.0f;
    const float t = 1.0f / bresen_t;
    float A = bresen_t - 1.0f;
    int B = 1;
    float p0, p1;
    for (; A > 0.0f; --A, ++B)
    {
        color inter;

        p0 = A * t;
        p1 = (float)B * t;
        inter.r = (uint8_t)(p0 * (float)c0.r + p1 * (float)c1.r);
        inter.g = (uint8_t)(p0 * (float)c0.g + p1 * (float)c1.g);
        inter.b = (uint8_t)(p0 * (float)c0.b + p1 * (float)c1.b);

        setpixel(bresen[B].x, bresen[B].y, inter);
    }
}

void render3d::DrawLine(uint8_t* local, color c0, color c1, vec2 begin, vec2 end)
{
    Timer::perf profile("DrawLine(local, c0, c1, begin, end)");

    std::vector<vec2> bresen = bresenham(begin, end);
    const int last = bresen.size() - 1;

    {
        Timer::perf profile("Interpolation");

        setpixel(local, bresen[0].x, bresen[0].y, c0);
        setpixel(bresen[0].x, bresen[0].y, c0);
        setpixel(local, bresen[last].x, bresen[last].y, c1);
        setpixel(bresen[last].x, bresen[last].y, c1);

        const float bresen_t = (float)bresen.size() - 1.0f;
        const float t = 1.0f / bresen_t;
        float A = bresen_t - 1.0f;
        int B = 1;
        float p0, p1;
        for (; A > 0.0f; --A, ++B)
        {
            color inter;

            p0 = A * t;
            p1 = (float)B * t;
            inter.r = (uint8_t)(p0 * (float)c0.r + p1 * (float)c1.r);
            inter.g = (uint8_t)(p0 * (float)c0.g + p1 * (float)c1.g);
            inter.b = (uint8_t)(p0 * (float)c0.b + p1 * (float)c1.b);

            setpixel(local, bresen[B].x, bresen[B].y, inter);
            setpixel(bresen[B].x, bresen[B].y, inter);
        }
    }
}

void render3d::DrawTriangle(color c, vec2 v0, vec2 v1, vec2 v2)
{
    DrawLine(c, v0, v1);
    DrawLine(c, v1, v2);
    DrawLine(c, v2, v0);
}

void render3d::FillTriangle(color c0, color c1, color c2, vec2 v0, vec2 v1, vec2 v2)
{
    Timer::perf __FillTriangle("FillTriangle()");

    uint8_t* local = (uint8_t*)calloc(WIDTH * HEIGHT * 4, sizeof(uint8_t)); // frame[x][y] = b, g, r, a

    {
        Timer::perf __TriangleFrame("Triangle frame");

        DrawLine(local, c0, c1, v0, v1);
        DrawLine(local, c1, c2, v1, v2);
        DrawLine(local, c2, c0, v2, v0);
    }

    {
        Timer::perf __TriangleFrame("Scanline");

        // Scanline Algorithm

        std::vector<std::vector<int>> all_x_on_y(HEIGHT); // all_x_on_y[y][x]

        for (int y = 0; y < HEIGHT; ++y)
        {
            for (int x = 0; x < WIDTH; ++x)
            {
                const unsigned offset = 4 * y * WIDTH + 4 * x;
                if (local[offset + 3] > 0)
                {
                    all_x_on_y[y].reserve(2);
                    all_x_on_y[y].push_back(x);
                }
            }
        }

        int x0, x1;
        for (int y = 0; y < HEIGHT; ++y)
        {
            if (all_x_on_y[y].size() > 0)
            {
                x0 = all_x_on_y[y][0]; // TODO Get position
                x1 = all_x_on_y[y][all_x_on_y[y].size() - 1];

                color c0;
                color c1;
                c0.convert(local, WIDTH, x0, y);
                c1.convert(local, WIDTH, x1, y);

                DrawVerticalLine(c0, c1, { x0, y }, { x1, y });
                // RENDER
            }
        }
    }

    free(local);
}