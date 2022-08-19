#pragma once

#include <vector>
#include <fstream>
#include "SDL.h"

struct color
{
    uint8_t r, g, b;
    uint8_t a = 255;
};

struct vec2
{
    int x, y;

    float dot(const vec2 &v) const;
};

struct mat4x4
{
    float m[4][4] = {}; // m[x][y]

    mat4x4 operator*(const mat4x4 &m) const;
};

struct vec3
{
    float x, y, z;

    float lenght() const;
    void normalize();
    vec3 normalized() const;
    void cross(const vec3 &line1, const vec3 &line2);
    float dot(const vec3 &v) const;

    vec3 operator+(const vec3 &v) const;
    vec3 operator-(const vec3 &v) const;
    vec3 operator*(const mat4x4 &m) const;
};

struct tri
{
    vec3 p[3]; // vertices (face)
    color c;
};

struct mesh
{
    std::vector<tri> tris;

    vec3 origin = {0.0f, 0.0f, 0.0f};
    vec3 position = {0.0f, 0.0f, 0.0f};
    mat4x4 rotation;
    vec3 scale = {1.0f, 1.0f, 1.0f};

    void rescale(float scaler);
    bool loadObjectFile(std::ifstream &stream);
};

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