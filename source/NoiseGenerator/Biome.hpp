#pragma once
#include "../TIleMap/GameTypes.hpp"

class Range
{
public:
    Range() = default;
    Range(float min, float max) : Min{ min }, Max{ max } {}
    ~Range() = default;
    bool fits(float val) const { return Min <= val && val <= Max; }
public:
    float Min, Max;
};

class Biome
{
public:
    Biome(
        TileType type, 
        Range heightR, 
        Range tempR, 
        Range HumidityR, 
        Range continentalnessR, 
        Range erosionR, 
        Range weirdnessR)
        :
        Type{ type },
        HeightRange{ heightR },
        TemperatureRange{ tempR },
        HumidityRange{ HumidityR },
        ContinentalnessRange{ continentalnessR },
        ErosionRange{ erosionR },
        WeirdnessRange{ weirdnessR }
    {}
public:
    TileType Type;
    Range HeightRange, TemperatureRange, HumidityRange;
    Range ContinentalnessRange, ErosionRange, WeirdnessRange;

};

