#include <iostream>
#include <vector>
#include "olcConsoleGameEngine.h"

struct mat4x4
{
    float m[4][4] = {};
};

struct vec3
{
    float x, y, z;

    vec3 &operator*(const mat4x4 &m)
    {
        vec3 *o = new vec3;

        o->x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + m.m[3][0];
        o->y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + m.m[3][1];
        o->z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + m.m[3][2];
        float w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + m.m[3][3];

        if (w != 0.0f) // Normalize by w
        {
            o->x /= w;
            o->y /= w;
            o->z /= w;
        }

        return *o;
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

class render3d : public olcConsoleGameEngine
{
public:
    render3d()
    {
        m_sAppName = L"render3d-test";
    }

private:
    mesh meshCube;
    mat4x4 matProj; // matProjection

public:
    bool OnUserCreate() override
    {
        meshCube.scaleX = 0.5f * (float)ScreenWidth();
        meshCube.scaleY = 0.5f * (float)ScreenHeight();
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
        float pAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
        float pFovScale = 1.0f / std::tanf(pFov * 0.5f); // Degree
        float pZScale = pZfar / (pZfar - pZnear);
        float pCameraOffset = -pZnear * pZScale; // (-pZfar * pZnear) / (pZfar - pZnear)

        matProj.m[0][0] = pAspectRatio * pFovScale;
        matProj.m[1][1] = pFovScale;
        matProj.m[2][2] = pZScale;
        matProj.m[3][2] = pCameraOffset;
        matProj.m[2][3] = 1;

        return 1;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

        for (tri t : meshCube.tris)
        {
            tri triTrans = t; // triTranslated
            triTrans.verts[0].z = t.verts[0].z + 3.0f;
            triTrans.verts[1].z = t.verts[1].z + 3.0f;
            triTrans.verts[2].z = t.verts[2].z + 3.0f;
            
            tri triProj; // triProjected

            triProj.verts[0] = triTrans.verts[0] * matProj;
            triProj.verts[1] = triTrans.verts[1] * matProj;
            triProj.verts[2] = triTrans.verts[2] * matProj;

            triProj.verts[0].x += 1.0f;
            triProj.verts[1].x += 1.0f;
            triProj.verts[2].x += 1.0f;
            triProj.verts[0].y += 1.0f;
            triProj.verts[1].y += 1.0f;
            triProj.verts[2].y += 1.0f;

            triProj.verts[0].x *= meshCube.scaleX;
            triProj.verts[1].x *= meshCube.scaleX;
            triProj.verts[2].x *= meshCube.scaleX;
            triProj.verts[0].y *= meshCube.scaleY;
            triProj.verts[1].y *= meshCube.scaleY;
            triProj.verts[2].y *= meshCube.scaleY;

            DrawTriangle(triProj.verts[0].x, triProj.verts[0].y,
                         triProj.verts[1].x, triProj.verts[1].y,
                         triProj.verts[2].x, triProj.verts[2].y, PIXEL_SOLID, FG_WHITE);
        }

        return 1;
    }
};

int main()
{
    render3d demo;

    if (demo.ConstructConsole(256, 240, 4, 4))
    {
        demo.Start();
    }

    return 0;
}