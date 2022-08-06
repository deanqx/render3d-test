#define SDL_MAIN_HANDLED

#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include "SDL.h"

struct mat4x4
{
    float m[4][4] = {};
};

struct vec3
{
    float x, y, z;

    vec3 operator*(const mat4x4& m)
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
};

struct tri
{
    vec3 verts[3]; // vertices
};

struct mesh
{
    float scaleX = 0.5f;
    float scaleY = 0.5f;
    std::vector<tri> tris;
};

SDL_Renderer* renderer = nullptr;

void DrawLine(int r, int g, int b, int startX, int startY, int endX, int endY)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    int px = (int)startX; // positionX
    int py = (int)startY; // positionX
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
                py += dirX;
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

class render3d
{
private:
    const int WIDTH;
    const int HEIGHT;

    mesh meshCube;
    mat4x4 matProj; // matProjection
    float fTheta = 0.0f;

public:
    render3d(const int width, const int height) : WIDTH(width), HEIGHT(height)
    {
        meshCube.scaleX = 0.5f * (float)WIDTH;
        meshCube.scaleY = 0.5f * (float)HEIGHT;
        meshCube.tris = {

            // SOUTH
            {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},

            // EAST
            {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},

            // NORTH
            {1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},

            // WEST
            {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

            // TOP
            {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},

            // BOTTOM
            {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},

        };

        float pZnear = 0.1f;
        float pZfar = 1000.0f;
        float pFov = 90.0f;
        float pAspectRatio = (float)HEIGHT / (float)WIDTH;
        float pFovScale = 1.0f / tanf(pFov * 0.5f); // Degree
        float pZScale = pZfar / (pZfar - pZnear);
        float pCameraOffset = -pZnear * pZScale; // (-pZfar * pZnear) / (pZfar - pZnear)

        matProj.m[0][0] = pAspectRatio * pFovScale;
        matProj.m[1][1] = pFovScale;
        matProj.m[2][2] = pZScale;
        matProj.m[3][2] = pCameraOffset;
        matProj.m[2][3] = 1;
    }

    bool Update(float deltaTime)
    {
        mat4x4 matRotZ, matRotX;
        fTheta += 1.0f * deltaTime;

        // Rotation Z
        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // Rotation X
        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fTheta * 0.5f);
        matRotX.m[1][2] = sinf(fTheta * 0.5f);
        matRotX.m[2][1] = -sinf(fTheta * 0.5f);
        matRotX.m[2][2] = cosf(fTheta * 0.5f);
        matRotX.m[3][3] = 1;

        for (tri t : meshCube.tris)
        {
            tri triProj, triTrans, triRotatedZX;  // triProjected, triTranslated

            triRotatedZX.verts[0] = (t.verts[0] * matRotZ) * matRotX;
            triRotatedZX.verts[1] = (t.verts[1] * matRotZ) * matRotX;
            triRotatedZX.verts[2] = (t.verts[2] * matRotZ) * matRotX;

            // Translation
            triTrans = triRotatedZX;
            triTrans.verts[0].z = triRotatedZX.verts[0].z + 3.0f;
            triTrans.verts[1].z = triRotatedZX.verts[1].z + 3.0f;
            triTrans.verts[2].z = triRotatedZX.verts[2].z + 3.0f;

            // Projection
            triProj.verts[0] = triTrans.verts[0] * matProj;
            triProj.verts[1] = triTrans.verts[1] * matProj;
            triProj.verts[2] = triTrans.verts[2] * matProj;

            // Position
            triProj.verts[0].x += 1.0f;
            triProj.verts[1].x += 1.0f;
            triProj.verts[2].x += 1.0f;
            triProj.verts[0].y += 1.0f;
            triProj.verts[1].y += 1.0f;
            triProj.verts[2].y += 1.0f;

            // Scale
            triProj.verts[0].x *= meshCube.scaleX;
            triProj.verts[1].x *= meshCube.scaleX;
            triProj.verts[2].x *= meshCube.scaleX;
            triProj.verts[0].y *= meshCube.scaleY;
            triProj.verts[1].y *= meshCube.scaleY;
            triProj.verts[2].y *= meshCube.scaleY;

            DrawTriangle(255, 255, 255,
                triProj.verts[0].x, triProj.verts[0].y,
                triProj.verts[1].x, triProj.verts[1].y,
                triProj.verts[2].x, triProj.verts[2].y);
        }

        return true;
    }
};

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 960
#define SCREEN_ZOOM 4
#define WIDTH (SCREEN_WIDTH / SCREEN_ZOOM)
#define HEIGHT (SCREEN_HEIGHT / SCREEN_ZOOM)

int main(int argc, char* argv[])
{
    SDL_Window* window = nullptr;

    if (SDL_Init( SDL_INIT_VIDEO ) < 0)
        return 2;
    window = SDL_CreateWindow("TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (!window)
        return 3;

    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetScale(renderer, SCREEN_ZOOM, SCREEN_ZOOM);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    render3d engine(WIDTH, HEIGHT);

    auto lastFrame = std::chrono::high_resolution_clock::now();
    while (true)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        std::chrono::duration<float> deltaTime = std::chrono::high_resolution_clock::now() - lastFrame;
        lastFrame = std::chrono::high_resolution_clock::now();
        engine.Update(deltaTime.count());

        SDL_RenderPresent(renderer);
    }

    system("pause");

    return 0;
}