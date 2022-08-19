#include "render3d.h"

#include <algorithm>
#include "timer.h"

#define RENDER                                            \
    SDL_UpdateTexture(screen, nullptr, frame, WIDTH * 4); \
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);   \
    SDL_RenderPresent(renderer);

inline int getheight(const vec2 (&v)[3], int &offset)
{
    int max = 0;
    int min = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (v[i].y > v[max].y)
            max = i;

        // TODO Try: else if
        if (v[i].y < v[min].y)
            min = i;
    }

    offset = v[min].y;
    return v[max].y - v[min].y + 1;
}

void render3d::DrawTriangle(color c, vec2 v0, vec2 v1, vec2 v2)
{
    DrawLine(c, v0, v1);
    DrawLine(c, v1, v2);
    DrawLine(c, v2, v0);
}

void render3d::FillTriangle(color c, vec2 v0, vec2 v1, vec2 v2)
{
    Timer::perf __FillTriangle("FillTriangle(color, vec2, vec2, vec2)");

    std::vector<vec2> triframe;

    {
        Timer::perf __TriangleFrame("Triangle frame");

        DrawLine(c, v0, v1, triframe);
        DrawLine(c, v1, v2, triframe);
        DrawLine(c, v2, v0, triframe);
    }

    // Scanline Algorithm O(n log n)

    int offset;
    std::vector<std::vector<vec2 *>> all_x_on_y(getheight({v0, v1, v2}, offset)); // all_x_on_y[y][x] = index

    {
        Timer::perf __TriangleFrame("Scanline");

        for (vec2 &v : triframe)
        {
            all_x_on_y[v.y - offset].push_back(&v);
        }

        for (auto &xlist : all_x_on_y)
        {
            std::sort(xlist.begin(), xlist.end(), [](vec2 *&a, vec2 *&b)
                      { return a->x < b->x; });
        }
    }

    for (auto &xlist : all_x_on_y)
    {
        // TODO Maybe its faster to find the position
        DrawVertical(c, xlist[0], xlist.back());
        // RENDER
    }
}