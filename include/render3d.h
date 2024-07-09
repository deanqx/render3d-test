#pragma once

#include "SDL.h"
#include "lilm.h"

class render3d
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *screen = nullptr;
    uint8_t *frame; // frame[x][y] = b, g, r, a

    inline void setpixel(const int x, const int y, const color c);

    void DrawLine(color c, vec2 begin, vec2 end);
    void DrawLine(const color& c, const vec2& begin, const vec2& end, std::vector<vec2> &triframe);
    void DrawVertical(const color& c, const vec2* begin, const vec2* end);

    void DrawTriangle(color c, vec2 v0, vec2 v1, vec2 v2);
    void FillTriangle(color c, vec2 v0, vec2 v1, vec2 v2);

    mat4x4 matProj; // matProjection

    bool HandleEvents();
    void Render(float deltaTime);

public:
    const int WIDTH, HEIGHT;
    const char *TITLE;

    std::vector<mesh *> objects;
    std::vector<SDL_Event> events;

    vec3 pCamera = {0.0f, 0.0f, 0.0f};

    render3d(const float fov, const float viewDistance, const char *title, const int width, const int height, const int scale);
    void GameLoop();
    ~render3d();

    virtual bool Start();
    virtual bool Update(float deltaTime);
};