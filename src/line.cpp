#include "render3d.h"
#include "timer.h"

// O(n)
inline size_t bresenham(vec2 begin, vec2 end, vec2 *&pos)
{
    Timer::perf __besenham("besenham");

    vec2 p;
    p.x = begin.x;                                           // positionX
    p.y = begin.y;                                           // positionY
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

    size_t count;

    if (dx < dy)
    {
        count = (size_t)dy + 1ULL;
        pos = (vec2 *)calloc(count, sizeof(vec2));
        pos[0] = p;

        p.y += dirY;

        float error = dy * 0.5f;
        for (size_t i = 1; i < count; ++i, p.y += dirY)
        {
            error -= dx;
            if (error > 0.0f)
            {
                pos[i] = p;
            }
            else
            {
                p.x += dirX;
                pos[i] = p;
                error += dy;
            }
        }
    }
    else
    {
        count = (size_t)dx + 1ULL;
        pos = (vec2 *)calloc(count, sizeof(vec2));
        pos[0] = p;

        p.x += dirX;

        float error = dx * 0.5f;
        for (size_t i = 1; i < count; ++i, p.x += dirX)
        {
            error -= dy;
            if (error > 0.0f)
            {
                pos[i] = p;
            }
            else
            {
                p.y += dirY;
                pos[i] = p;
                error += dx;
            }
        }
    }

    return count;
}

inline void render3d::setpixel(const int x, const int y, const color c)
{
    const int offset = 4 * y * WIDTH + 4 * x;
    frame[offset] = c.b;
    frame[offset + 1] = c.g;
    frame[offset + 2] = c.r;
    frame[offset + 3] = c.a;
}

void render3d::DrawLine(color c, vec2 begin, vec2 end)
{
    Timer::perf profile("DrawLine(color, vec2, vec2)");

    vec2 *bresen;
    size_t count = bresenham(begin, end, bresen);

    for (size_t i = 0; i < count; ++i)
    {
        setpixel(bresen[i].x, bresen[i].y, c);
    }

    free(bresen);
}

void render3d::DrawLine(const color &c, const vec2 &begin, const vec2 &end, std::vector<vec2> &triframe)
{
    Timer::perf profile("DrawLine(color, vec2, vec2, vec2[])");

    vec2 *bresen;
    size_t count = bresenham(begin, end, bresen);

    {
        Timer::perf profile("Draw");
        for (size_t i = 0; i < count; ++i)
        {
            setpixel(bresen[i].x, bresen[i].y, c);
            triframe.push_back(bresen[i]);
        }
    }
}

void render3d::DrawVertical(const color &c, const vec2 *begin, const vec2 *end)
{
    Timer::perf per("DrawVertical()");

    int len = end->x - begin->x;
    for (int x = 1; x < len; ++x)
    {
        setpixel(begin->x + x, begin->y, c);
    }
}