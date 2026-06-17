#pragma once
#include <vector>
#include <string>

#include "Biome.hpp"
#include "PerlinNoise.hpp"
#include "VoronoiDiagram.hpp"

class NoiseGenerator
{
    struct Vector2
    { 
        Vector2() 
            : 
            x{ 0.0f }, 
            y{ 0.0f } 
        {}
        float x, y; 
    };

    struct FractalPerlinParam
    {
        FractalPerlinParam(float frequency, float amplitude, float persistence, float lacunarity)
            :
            frequency{ frequency },
            amplitude{ amplitude },
            persistence{ persistence },
            lacunarity{ lacunarity }
        {}
    public:
        float frequency;
        float amplitude;
        float persistence;
        float lacunarity;
    };

    struct VoronoiFilterParam
    {
    public:
        //TECTONIC PARAMETER
        float spaceScale = 50.0f;     // Масштаб сетки Вороного (ваш скейл)
        float zoneWidth = 30.0f;       // Ширина горного хребта на стыке плит
        float heightBonus = 0.25f;     // Высота подъема горы на самом стыке плит                          
        float warpFrequency = 0.11f;   // влияет на шумность контура континентов (появление дыр), чем выше тем более шумный контур
        float warpAmplitude = 30.0f;   // влияет на общий контур континентов, чем выше тем более четкий контур
                                              
        //TEMPERATURE PARAMETER       
        float tempWarpFrequency = 0.02f;    // Частота извилистости температурных границ
        float tempWarpAmplitude = 25.0f;     // Сила искажения температурных границ
        float maxTempOffset = 14.0f;   // Максимальный сдвиг температуры для региона (+/- °C)

        //HUMIDITY PARAMETER
        float humWarpFrequency = 0.02f;   // Частота извилистости границ влажности
        float humWarpAmplitude = 25.0f;    // Сила искажения границ влажности
        float maxHumOffset = 20.0f;  // Максимальный сдвиг влажности для региона (+/- %)

        //CONTINENTALNESS PARAMETER
        float continentalnessInfluence = 0.5f; // Сила влияния разломов Вороного на форму берегов

        //RIVER PARAMETER
        float riverScale = 18.0f;            // Частота рек (чем меньше, тем больше рек)
        float riverFrequency = 0.04f;         // Извилистость русла
        float riverAmplitude = 20.0f;         // Сила изгибов реки
        float riverChannelWidth = 2.0f;       // Ширина самого глубокого русла (в тайлах)
        float riverValleyWidth = 5.0f;       // Ширина речной долины (плавный спуск к воде)
        float riverDepthNormalized = 0.15f;   // Глубина врезания реки
    };

    struct HeightParam
    {
    public:
        float max = 255.0f;                  // Максимальная высота мира
        float scale = 20.0f;
    };

    struct TemperatureParam
    {
    public:
        float scale = 30.0f;           // Огромный масштаб, чтобы климат менялся очень плавно
        float standart = 20.0f;          // Базовая температура на уровне моря (в градусах Цельсия)
        float lossByMaxHeight = 10.0f;  // Сколько градусов теряется на максимальной вершине мира
        float noiseInfluence = 15.0f; // Амплитуда колебания климатических зон (+/- градусов)
    };

    struct HumidityParam
    {
    public:
        float scale = 30.0f;              // множитель масштаба влажности
        float coldDryPenalty = 0.04f;              // Сила падения влажности на каждый градус мороза
        float hotMoistureBonus = 0.02f;            // Прирост потенциальной влажности на каждый градус жары
        float HighestTemperature = 25.0f;  // Порог жаркого тропического климата (humidityHighestTemperature)
        float LowestTemperature = 0.0f;    // Точка замерзания воды 
        float min = 0.0f;                  // минимальный процент влажности
        float max = 100.0f;                // максимальный процент влажности
    };

    struct ContinentalnessParam
    {
    public:
        float scale = 30.0f;       // Гигантский масштаб для огромных океанов и материков
        float min = -1.0f;
        float max = 1.0f;

        float oceanThreshold = 0.0f;        // Все, что ниже — глубокий океан
        float shoreThreshold = 0.2f;    // Все, что ниже (и выше океана) — пляжи/берег

        float shoreHeightMultiplier = 50.0f;  // Множитель высоты для береговой зоны
        float landHeightMultiplier = 0.35f;

        float oceanInfluence = 0.2f;   // Сила влияния деталей Перлина на дне океана
        float shoreInfluence = 0.1f;   // Сила влияния деталей Перлина на пляжах
        float landInfluence = 0.3f;   // Сила влияния деталей Перлина на суше

        float shoreBaseFloor = 0.0f;  //влияет на плавный переход к берегу, создает область высот где океан и берег пересекаются, 0.0f -> резкий переход
        float landBaseFloor = 0.25f;
        float shoreHeightRange = 0.06f; //0.2f
    };

    struct ErosionParam
    {
    public:
        float scale = 30.0f;          // Масштаб плавного чередования гор и равнин
        float plainsThreshold = 0.2f;  // Порог, выше которого горы начинают сглаживаться в равнины
        float min = -1.0f;
        float max = 1.0f;

        //PEAK&WALLEY PARAMETER
        float pvAmplitude = 0.15f;
        float pvOffset = 0.05f;
    };

    struct WeirdnessParam
    {
    public:
        float scale = 300.0f;        // Масштаб для редких аномалий ландшафта
        float min = 0.0f;
        float max = 1.0f;
    };

public:
    NoiseGenerator();
    std::vector<Biome> setBiomes();
    std::vector<Biome> generateBiomesMap(
        int width, int height, float offsetX, float offsetY);
private:
    std::vector<float> humidityGeneration(
        int width, int height, float offsetX, float offsetY, const std::vector<float>& temperatureMap);
    std::vector<float> temperatureGeneration(
        int width, int height, float offsetX, float offsetY, const std::vector<float>& heightMap);
    std::vector<float> weirdnessGeneration(
        int width, int height, float offsetX, float offsetY);
    std::vector<float> heightGeneration(
        int width, int height, float offsetX, float offsetY, const std::vector<float>& continentalnessMap, const std::vector<float>& erosionMap);
    std::vector<float> erosionGeneration(
        int width, int height, float offsetX, float offsetY);
    std::vector<float> continentalnessGeneration(
        int width, int height, float offsetX, float offsetY);

    float fractalPerlinFilter(
        int x, int y, int halfW, int halfH, float offsetX, float offsetY, float scale, int octaves, const std::vector<Vector2>& offsets);
    float perlinFiler(
        int x, int y, float halfW, float halfH, float offsetX, float offsetY, float frequency, float scale, const Vector2& offset);

    void voronoiTectonicFilter(
        float globalX, float globalY, bool condition, float influence, float frequency, float amplitude, float& metric);
    void voronoiRiverFilter(
        float globalX, float globalY, float& normalizedHeight);
    float voronoiRegionFilter(
        int x, int y, float offsetX, float offsetY, float warpFrequency, float warpAmplitude, float maxOffset, bool randReset);

    float erosionFilter(float currentErosion, float normalizedHeight);
    float peakAndValleyFilter(float currentErosion);
    float continentalnessFilter(float currentContinentalness, float erosedHeightNoise, float PeakValleyInfluence);

    void generateRandomOffsets(unsigned int seed);
    //DEBUG
    void saveVectorBinary(const std::string& filename, const std::vector<float>& data);
private:
    unsigned int globalSeed;

    const double maxFloat = 3.40282347E+38;
    const double minFloat = -maxFloat;

    const float empiricalMax = 0.5f;


    FractalPerlinParam fpParam;
    const VoronoiFilterParam vParam;
    const TemperatureParam tempParam;
    const HumidityParam humParam;
    const HeightParam hParam;
    const ContinentalnessParam сParam;
    const ErosionParam eParam;
    const WeirdnessParam wParam;

    PerlinNoise noise;
    VoronoiDiagram voronoi;
    VoronoiDiagram voronoiRiver;

    std::mt19937 rng;
    std::vector<Vector2> octaveOffsets;
    std::vector<Vector2> continentalnessOffsets;
    std::vector<Vector2> erosionOffsets;
    std::vector<Vector2> weirdnessOffsets;
    Vector2 temperatureOffset;
    Vector2 humidityOffset;
};

