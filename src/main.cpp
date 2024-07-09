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
        meshCube.position = {0.0f, 0.0f, 5.0f}; 

        // std::ifstream file("./res/AK 47.obj");
        std::ifstream file("./res/cube.obj");
        if (meshCube.loadObjectFile(file))
            return 1;

        file.close();

        return 0;
    }

    float fTheta = 0.0f;
    bool Update(float deltaTime) override
    {
        Timer::perf __Update("Update");

        fTheta += 1.0f * deltaTime;
        mat4x4 matRotX = mat4x4::rotationX(fTheta * 0.5f);
        mat4x4 matRotZ = mat4x4::rotationZ(fTheta);
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