#define SDL_MAIN_HANDLED

#include <math.h>
#include <fstream>
#include "render3d.h"
#include "timer.h"

class Engine : public render3d
{
public:
    using render3d::render3d;

    mesh meshCube;

    bool Start() override
    {
        objects.push_back(&meshCube);
        meshCube.rescale((float)WIDTH * 0.5f);
        meshCube.origin = {0.0f, 0.0f, 0.0f};
        meshCube.position = {0.0f, 0.0f, 10.0f};

        std::ifstream file("./res/AK 47.obj");
        // std::ifstream file("./res/cube.obj");
        if (meshCube.loadObjectFile(file))
            return 1;

        file.close();

        return 0;
    }

    float fTheta = 0.0f;
    bool Update(float deltaTime) override
    {
        Timer::perf __Update("Update");

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

        meshCube.rotation = matRotX * matRotZ;

        return 0;
    }
};

int main(int argc, char *argv[])
{
    Timer::init();

    Engine engine(90.0f, 1000.0f, "render3d-test", 1024, 960, 1);
    engine.GameLoop();

    Timer::printp({});
    Timer::quit();

    return 0;
}