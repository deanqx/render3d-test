#include <vector>
#include <fstream>

struct mat4x4
{
    float m[4][4] = {}; // m[x][y]

    static mat4x4 eye(); // Create Identity Matrix
    static mat4x4 rotationX(float angleRad);
    static mat4x4 rotationY(float angleRad);
    static mat4x4 rotationZ(float angleRad);
    static mat4x4 translation(float x, float y, float z);
    static mat4x4 projection(float aspectratio, float fov, float viewdistance, float znear = 0.1f);

    mat4x4 operator*(const mat4x4 &m) const;
};

struct color
{
    uint8_t r, g, b;
    uint8_t a = 255;
};

struct vec2
{
    int x, y;

    float dot(const vec2 &v) const;
};

struct vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    float lenght() const;
    void normalize();
    vec3 normalized() const;
    void cross(const vec3 &line1, const vec3 &line2);
    float dot(const vec3 &v) const;

    vec3 operator+(const vec3 &v) const;
    vec3 operator-(const vec3 &v) const;
    vec3 operator*(const mat4x4 &m) const;
    void operator+=(const vec3 &v);
    void operator/=(const float &w);
};

struct tri
{
    vec3 p[3]; // vertices (face)
    color c;
};

struct mesh
{
    std::vector<tri> tris;

    vec3 origin = {0.0f, 0.0f, 0.0f};
    vec3 position = {0.0f, 0.0f, 0.0f};
    mat4x4 rotation;
    vec3 scale = {1.0f, 1.0f, 1.0f};

    void rescale(float scaler);
    bool loadObjectFile(std::ifstream &stream);
};