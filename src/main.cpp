#define SDL_MAIN_HANDLED

#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include <sstream>
#include "gmath.h"
#include "timer.h"
#include "SDL.h"

Timer* t;

class render3d
{
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    const int WIDTH;
    const int HEIGHT;

    mesh meshCube;
    mat4x4 matProj; // matProjection
    float fTheta = 0.0f;

    vec3 pCamera = { 0.0f, 0.0f, 0.0f };

public:
    std::vector<SDL_Event> events;

    render3d(SDL_Window* window, SDL_Renderer* renderer, const int width, const int height) : window(window), renderer(renderer), WIDTH(width), HEIGHT(height)
    {
        meshCube.rescale((float)WIDTH * 0.5f);
        meshCube.origin = { 0.0f, 0.0f, 0.0f };
        meshCube.position = { 0.0f, 0.0f, 3.0f };
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
        float pFovScale = 1.0f / tanf(pFov * 0.5f / 180.0f * 3.14159f); // Radian
        float pZScale = pZfar / (pZfar - pZnear);
        float pCameraOffset = -pZnear * pZScale; // (-pZfar * pZnear) / (pZfar - pZnear)

        matProj.m[0][0] = pAspectRatio * pFovScale;
        matProj.m[1][1] = pFovScale;
        matProj.m[2][2] = pZScale;
        matProj.m[3][2] = pCameraOffset;
        matProj.m[2][3] = 1;
    }

    bool HandleEvents()
    {
        Timer::perf performance(t, "HandleEvents");

        std::vector<SDL_Event>().swap(events);
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return 1;
            case SDL_WINDOWEVENT_MINIMIZED:
                printf("minimize");
                break;
            default:
                events.push_back(event);
                break;
            }
        }

        return 0;
    }

    bool Update(float deltaTime)
    {
        Timer::perf performance(t, "Update");

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

            triRotatedZX.p[0] = t.p[0] * matRotZ * matRotX;
            triRotatedZX.p[1] = t.p[1] * matRotZ * matRotX;
            triRotatedZX.p[2] = t.p[2] * matRotZ * matRotX;

            // Position
            triTrans = triRotatedZX;
            triTrans.p[0] = triRotatedZX.p[0] + meshCube.position;
            triTrans.p[1] = triRotatedZX.p[1] + meshCube.position;
            triTrans.p[2] = triRotatedZX.p[2] + meshCube.position;

            vec3 normal, line1, line2;
            line1 = triTrans.p[1] - triTrans.p[0];
            line2 = triTrans.p[2] - triTrans.p[0];

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;
            normal.normalize();

            if (normal.dot(triTrans.p[0] - pCamera) < 0.0f)
            {
                // Project triangles from 3D --> 2D
                triProj.p[0] = triTrans.p[0] * matProj;
                triProj.p[1] = triTrans.p[1] * matProj;
                triProj.p[2] = triTrans.p[2] * matProj;

                // Scale into view
                triProj.p[0].x += 1.0f; triProj.p[0].y += 1.0f;
                triProj.p[1].x += 1.0f; triProj.p[1].y += 1.0f;
                triProj.p[2].x += 1.0f; triProj.p[2].y += 1.0f;
                triProj.p[0].x *= 0.5f * (float)WIDTH;
                triProj.p[0].y *= 0.5f * (float)HEIGHT;
                triProj.p[1].x *= 0.5f * (float)WIDTH;
                triProj.p[1].y *= 0.5f * (float)HEIGHT;
                triProj.p[2].x *= 0.5f * (float)WIDTH;
                triProj.p[2].y *= 0.5f * (float)HEIGHT;

                // Optimization: Draw only if in view
                if (WIDTH > triProj.p[0].x && HEIGHT > triProj.p[0].y
                    && WIDTH > triProj.p[1].x && HEIGHT > triProj.p[1].y
                    && WIDTH > triProj.p[2].x && HEIGHT > triProj.p[2].y)
                {
                    // TODO Temp
                    if (triProj.p[0].x < 0.0f) triProj.p[0].x = 0.0f; if (triProj.p[0].y < 0.0f) triProj.p[0].y = 0.0f;
                    if (triProj.p[1].x < 0.0f) triProj.p[1].x = 0.0f; if (triProj.p[1].y < 0.0f) triProj.p[1].y = 0.0f;
                    if (triProj.p[2].x < 0.0f) triProj.p[2].x = 0.0f; if (triProj.p[2].y < 0.0f) triProj.p[2].y = 0.0f;

                    FillTriangle({ 0, 0, 255 }, { 255, 0, 0 }, { 0, 255, 0 },
                        { (int)triProj.p[0].x, (int)triProj.p[0].y },
                        { (int)triProj.p[1].x, (int)triProj.p[1].y },
                        { (int)triProj.p[2].x, (int)triProj.p[2].y });
                }
            }
        }

        return 0;
    }

    void Quit()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

#define TITLE "render3d-test"
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 960
#define RESOLUTION 1
#define WIDTH (SCREEN_WIDTH / RESOLUTION)
#define HEIGHT (SCREEN_HEIGHT / RESOLUTION)

int main(int argc, char* argv[])
{
    t = new Timer;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 2;
    SDL_Window* window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (!window)
        return 3;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetScale(renderer, RESOLUTION, RESOLUTION);

    INIT(window, renderer, WIDTH, HEIGHT, t);
    render3d engine(window, renderer, WIDTH, HEIGHT);

    auto displayedFPS = std::chrono::high_resolution_clock::now();
    auto lastFrame = displayedFPS;
    int profile = 0;
    for (int frame = 1; true; ++frame)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (engine.HandleEvents())
            break;

        std::chrono::duration<float> deltaTime = std::chrono::high_resolution_clock::now() - lastFrame;
        lastFrame = std::chrono::high_resolution_clock::now();
        if (engine.Update((float)deltaTime.count()))
            break;

        std::chrono::duration<float> lastDisplayedFPS = std::chrono::high_resolution_clock::now() - displayedFPS;
        if (1.0f <= lastDisplayedFPS.count())
        {
            displayedFPS = std::chrono::high_resolution_clock::now();
            float fps = 1.0f / deltaTime.count();

            std::stringstream title;
            title << TITLE << " - " << frame << " FPS - " << (int)fps << " lowFPS";

            SDL_SetWindowTitle(window, title.str().c_str());
            title << '\n';
            printf(title.str().c_str());
            frame = 0;

            if (profile == 20)
            {
                t->print({ "Update", "HandleEvents", "FillTriangle", "DrawVerticalLine" });
                break;
            }

            ++profile;
        }

        SDL_RenderPresent(renderer);
    }
    engine.Quit();

    delete t;
    return 0;
}