#include "render3d.h"

#include <math.h>
#include <array>
#include "timer.h"

bool render3d::HandleEvents()
{
    Timer::perf __HandleEvents("HandleEvents");

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

void render3d::Render(float deltaTime)
{
    Timer::perf __Render("Render");


    for (mesh* obj : objects)
    {
        for (tri t : obj->tris)
        {
            tri triProj, triTrans, triRotatedZX, temp;  // triProjected, triTranslated

            // Rotation
            triRotatedZX.p[0] = t.p[0] * obj->rotation;
            triRotatedZX.p[1] = t.p[1] * obj->rotation;
            triRotatedZX.p[2] = t.p[2] * obj->rotation;

            // Position
            triTrans = triRotatedZX;
            triTrans.p[0] = triRotatedZX.p[0] + obj->position;
            triTrans.p[1] = triRotatedZX.p[1] + obj->position;
            triTrans.p[2] = triRotatedZX.p[2] + obj->position;

            // Normal
            vec3 normal;
            normal.cross(triTrans.p[1] - triTrans.p[0], triTrans.p[2] - triTrans.p[0]);
            normal.normalize();

            if (normal.dot(triTrans.p[0].normalized() - pCamera) < 0.0f)
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

                // Optimization: Draw only if in field of view
                if (WIDTH > triProj.p[0].x && HEIGHT > triProj.p[0].y
                    && WIDTH > triProj.p[1].x && HEIGHT > triProj.p[1].y
                    && WIDTH > triProj.p[2].x && HEIGHT > triProj.p[2].y)
                {
                    // TODO Temp
                    if (triProj.p[0].x < 0.0f) triProj.p[0].x = 0.0f; if (triProj.p[0].y < 0.0f) triProj.p[0].y = 0.0f;
                    if (triProj.p[1].x < 0.0f) triProj.p[1].x = 0.0f; if (triProj.p[1].y < 0.0f) triProj.p[1].y = 0.0f;
                    if (triProj.p[2].x < 0.0f) triProj.p[2].x = 0.0f; if (triProj.p[2].y < 0.0f) triProj.p[2].y = 0.0f;

                    vec3 sun{ 0.0f, 0.0f, -1.0f };
                    sun.normalize();

                    float brightness = fabsf(normal.dot(sun));

                    FillTriangle(
                        { (uint8_t)(255.0f * brightness), (uint8_t)(255.0f * brightness), (uint8_t)(255.0f * brightness) },
                        { (uint8_t)(255.0f * brightness), (uint8_t)(255.0f * brightness), (uint8_t)(255.0f * brightness) },
                        { (uint8_t)(255.0f * brightness), (uint8_t)(255.0f * brightness), (uint8_t)(255.0f * brightness) },
                        { (int)triProj.p[0].x, (int)triProj.p[0].y },
                        { (int)triProj.p[1].x, (int)triProj.p[1].y },
                        { (int)triProj.p[2].x, (int)triProj.p[2].y });

                    // FillTriangle(
                    //     { 255, 0, 0 },
                    //     { 0, 255, 0 },
                    //     { 0, 0, 255 },
                    //     { (int)triProj.p[0].x, (int)triProj.p[0].y },
                    //     { (int)triProj.p[1].x, (int)triProj.p[1].y },
                    //     { (int)triProj.p[2].x, (int)triProj.p[2].y });

                    // DrawTriangle({ 255, 255, 0 },
                    //     { (int)triProj.p[0].x, (int)triProj.p[0].y },
                    //     { (int)triProj.p[1].x, (int)triProj.p[1].y },
                    //     { (int)triProj.p[2].x, (int)triProj.p[2].y });
                }
            }
        }
    }

    SDL_UpdateTexture(screen, nullptr, frame, WIDTH * 4);
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

render3d::render3d(const float fov, const float viewDistance, const char* title, const int width, const int height, const int sizing) : TITLE(title), WIDTH(width / sizing), HEIGHT(height / sizing)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        return;

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!window)
        return;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    float pZnear = 0.1f;
    float pZfar = viewDistance;
    float pFov = fov;
    float pAspectRatio = (float)WIDTH / (float)HEIGHT;
    float pFovScale = 1.0f / tanf(pFov * 0.5f / 180.0f * 3.14159f); // Radian
    float pZScale = pZfar / (pZfar - pZnear);
    float pCameraOffset = -pZnear * pZScale; // (-pZfar * pZnear) / (pZfar - pZnear)

    matProj.m[0][0] = pAspectRatio * pFovScale;
    matProj.m[1][1] = pFovScale;
    matProj.m[2][2] = pZScale;
    matProj.m[3][2] = pCameraOffset;
    matProj.m[2][3] = 1;
}

void render3d::GameLoop()
{
    if (Start())
        return;

    auto displayedFPS = std::chrono::high_resolution_clock::now();
    auto lastFrame = displayedFPS;
    int profile = 0;
    for (int count = 0; true; ++count)
    {
        frame = (uint8_t*)calloc(WIDTH * HEIGHT * 4, sizeof(uint8_t));

        if (HandleEvents())
            break;

        std::chrono::duration<float> deltaTime = std::chrono::high_resolution_clock::now() - lastFrame;
        lastFrame = std::chrono::high_resolution_clock::now();
        if (Update(deltaTime.count()))
            break;

        Render(deltaTime.count());

        std::chrono::duration<float> lastDisplayedFPS = std::chrono::high_resolution_clock::now() - displayedFPS;
        if (1.0f <= lastDisplayedFPS.count())
        {
            displayedFPS = std::chrono::high_resolution_clock::now();
            float fps = 1.0f / deltaTime.count();

            std::stringstream ss;
            ss << TITLE << " - " << count << " FPS - " << (int)fps << " ranFPS";
            SDL_SetWindowTitle(window, ss.str().c_str());

            count = -1;

#ifndef RELEASE
            ss << '\n';
            printf(ss.str().c_str());

            if (profile == 20)
                break;

            ++profile;
#endif
        }

        free(frame);
    }
}

render3d::~render3d()
{
    SDL_DestroyWindow(window);
    SDL_Quit();

    delete TITLE;
}

bool render3d::Start()
{
    return 0;
}

bool render3d::Update(float deltaTime)
{
    return 0;
}