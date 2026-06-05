#pragma once

class PerlinNoise
{
public:
    PerlinNoise() = default;
    PerlinNoise(unsigned int seed) 
        : 
        seed(seed)
    {
        reseed();
    }
    float createNoise2d(float x, float y);
private:
    float grad(int hash, float xf, float yf) const;
    float lerp(float t, float a, float b) const;
    float invLerp(float t, float a, float b) const;
    float fade(float t) const;
    void reseed();
private:
    unsigned int seed;
    int p[512];
};

