#pragma once

#include "SDL.h"

void INIT(SDL_Window* window, SDL_Renderer* renderer);

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

void DrawLine(int r, int g, int b, int startX, int startY, int endX, int endY);
void DrawTriangle(int r, int g, int b, float x0, float y0, float x1, float y1, float x2, float y2);