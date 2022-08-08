#pragma once

#include "SDL.h"

void INIT(SDL_Window* window, SDL_Renderer* renderer, int width, int height);

struct mat4x4
{
    float m[4][4] = {};
};

struct vec3
{
    float x, y, z;

    float lenght();
    void normalize();
    float dot(const vec3& v);

    vec3 operator+(const vec3& v);
    vec3 operator-(const vec3& v);
    vec3 operator*(const mat4x4& m);
};

struct tri
{
    vec3 p[3]; // vertices (face)
};

struct mesh
{
    std::vector<tri> tris;

    vec3 position = { 0.0f, 0.0f, 0.0f };
    vec3 origin = { 0.0f, 0.0f, 0.0f };
    vec3 scale = { 1.0f, 1.0f, 1.0f };

    void rescale(float scaler);
};

struct vec2
{
    int x, y;
};

struct color
{
    int r, g, b;
};

struct color2
{
    vec2 pos;
    color c;
};

void DrawLine(color c, vec2 begin, vec2 end);
void DrawLine(color c0, color c1, vec2 begin, vec2 end);
void DrawTriangle(color c, vec2 v0, vec2 v1, vec2 v2);
void FillTriangle(color c0, color c1, color c2, vec2 v0, vec2 v1, vec2 v2);