#include "lilm.h"
#include <math.h>

mat4x4 mat4x4::eye()
{
    mat4x4 o;
    o.m[0][0] = 1.0f;
    o.m[1][1] = 1.0f;
    o.m[2][2] = 1.0f;
    o.m[3][3] = 1.0f;
    return o;
}
mat4x4 mat4x4::rotationX(float angleRad)
{
    mat4x4 o;
    o.m[0][0] = 1.0f;
    o.m[1][1] = cosf(angleRad);
    o.m[1][2] = sinf(angleRad);
    o.m[2][1] = -sinf(angleRad);
    o.m[2][2] = cosf(angleRad);
    o.m[3][3] = 1.0f;
    return o;
}
mat4x4 mat4x4::rotationY(float angleRad)
{
    mat4x4 o;
    o.m[0][0] = cosf(angleRad);
    o.m[2][0] = sinf(angleRad);
    o.m[1][1] = 1.0f;
    o.m[0][2] = -sinf(angleRad);
    o.m[2][2] = cosf(angleRad);
    o.m[3][3] = 1.0f;
    return o;
}
mat4x4 mat4x4::rotationZ(float angleRad)
{
    mat4x4 o;
    o.m[0][0] = cosf(angleRad);
    o.m[0][1] = sinf(angleRad);
    o.m[1][0] = -sinf(angleRad);
    o.m[1][1] = cosf(angleRad);
    o.m[2][2] = 1.0f;
    o.m[3][3] = 1.0f;
    return o;
}
mat4x4 mat4x4::translation(float x, float y, float z)
{
    mat4x4 o;
    o.m[0][0] = 1.0f;
    o.m[1][1] = 1.0f;
    o.m[2][2] = 1.0f;
    o.m[3][3] = 1.0f;
    o.m[3][0] = x;
    o.m[3][1] = y;
    o.m[3][2] = z;
    return o;
}
mat4x4 mat4x4::projection(float aspectratio, float fov, float viewdistance, float znear)
{
    const float fovscale = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f); // Radian
    const float zscale = viewdistance / (viewdistance - znear);
    const float cameraoffset = -znear * zscale; // (-pZfar * pZnear) / (pZfar - pZnear)

    mat4x4 o;
    o.m[0][0] = aspectratio * fovscale;
    o.m[1][1] = fovscale;
    o.m[2][2] = zscale;
    o.m[3][2] = cameraoffset;
    o.m[2][3] = 1.0f;
    return o;
}
mat4x4 mat4x4::operator*(const mat4x4 &i) const
{
    mat4x4 o;

    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            for (int j = 0; j < 4; ++j)
            {
                o.m[x][y] += m[j][y] * i.m[x][j];
            }
        }
    }

    return o;
}