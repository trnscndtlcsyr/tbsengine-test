#include "PerlinNoise.h"

#include <cmath>
#include <numeric>
#include <random>
#include <algorithm>

float PerlinNoise::createNoise2d(float x, float y)
{
    //get global square coordinate
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;

    //get local coordinate of point inside square
    x -= std::floor(x);
    y -= std::floor(y);

    //get fade for x y
    float u = fade(x);
    float v = fade(y);

    //get hash for each points from 4 corners
    int aa = p[p[X] + Y];
    int ab = p[p[X] + Y + 1];
    int ba = p[p[X + 1] + Y];
    int bb = p[p[X + 1] + Y + 1];

    //linear interpolation by fade(x) between gradients of left bottom corner and right bottom corner
    // and also same thing for gradients of right top and left top corners
    float x1 = lerp(u, grad(aa, x, y), grad(ba, x - 1.0f, y));
    float x2 = lerp(u, grad(ab, x, y - 1.0f), grad(bb, x - 1.0f, y - 1.0f));

    //final noise linear interpolation between 2 values of interpolared corner values
    return lerp(v, x1, x2);
}

float PerlinNoise::grad(int hash, float xf, float yf) const
{
    //get direction of vector by hash and 8 possible directions
    switch (hash & 0x7)
    {
    case 0x0: return  xf + yf;
    case 0x1: return  xf;
    case 0x2: return  xf - yf;
    case 0x3: return -yf;
    case 0x4: return -xf - yf;
    case 0x5: return -xf;
    case 0x6: return -xf + yf;
    case 0x7: return  yf;
    default:  return  float(0.0);
    }
}

float PerlinNoise::lerp(float t, float a, float b) const
{
    return a + t * (b - a);
}
float PerlinNoise::invLerp(float t, float a, float b) const
{
    return (t - a) / (b - a);
}
float PerlinNoise::fade(float t) const
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

void PerlinNoise::reseed()
{
    std::vector<int> permutation(256);
    std::iota(permutation.begin(), permutation.end(), 0);
    std::mt19937 engine(seed);
    std::shuffle(permutation.begin(), permutation.end(), engine);
    for (int i = 0; i < 256; ++i)
    {
        p[i] = permutation[i];
        p[i + 256] = permutation[i];
    }
}
