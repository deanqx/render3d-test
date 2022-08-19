#include "render3d.h"
#include <sstream>
#include <math.h>

float vec2::dot(const vec2& v) const
{
    return x * v.x + y * v.y;
}

mat4x4 mat4x4::operator*(const mat4x4& i) const
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

float vec3::lenght() const
{
    return sqrtf(x * x + y * y + z * z);
}
void vec3::normalize()
{
    float l = 1 / sqrtf(x * x + y * y + z * z);
    if (l > 0.0f)
    {
        x *= l; y *= l; z *= l;
    }
}
vec3 vec3::normalized() const
{
    vec3 o = *this;

    float l = 1 / sqrtf(x * x + y * y + z * z);
    if (l > 0.0f)
    {
        o.x *= l; o.y *= l; o.z *= l;
    }

    return o;
}
void vec3::cross(const vec3& line1, const vec3& line2)
{
    x = line1.y * line2.z - line1.z * line2.y;
    y = line1.z * line2.x - line1.x * line2.z;
    z = line1.x * line2.y - line1.y * line2.x;
}
float vec3::dot(const vec3& v) const
{
    return x * v.x + y * v.y + z * v.z;
}

vec3 vec3::operator+(const vec3& v) const
{
    vec3 o;

    o.x = x + v.x;
    o.y = y + v.y;
    o.z = z + v.z;

    return o;
}
vec3 vec3::operator-(const vec3& v) const
{
    vec3 o;

    o.x = x - v.x;
    o.y = y - v.y;
    o.z = z - v.z;

    return o;
}
vec3 vec3::operator*(const mat4x4& m) const
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

void mesh::rescale(float scaler)
{
    scale = { scaler, scaler, scaler };
}
bool mesh::loadObjectFile(std::ifstream& stream)
{
    if (stream.fail())
        return 1;

    std::vector<vec3> verts;

    while (!stream.eof())
    {
        char line[128];
        stream.getline(line, 128);

        std::stringstream ss;
        ss << line;

        char type;

        if (line[0] == 'v')
        {
            vec3 v;
            ss >> type >> v.x >> v.y >> v.z;
            verts.push_back(v);
        }

        if (line[0] == 'f')
        {
            int f[3];
            ss >> type >> f[0] >> f[1] >> f[2];
            tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
        }
    }

    return 0;
}