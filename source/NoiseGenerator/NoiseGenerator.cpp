#include "NoiseGenerator.h"
#include "PerlinNoise.h"
#include <random>
#include <iostream>
#include <fstream>
#include <string>

NoiseGenerator::NoiseGenerator() 
    : 
    globalSeed(0),
    noise(globalSeed), 
    voronoi(globalSeed, vParam.spaceScale), 
    voronoiRiver(globalSeed, vParam.riverScale),
    fpParam(1.0f, 1.0f, 0.5f, 2.0f)
{
    //std::random_device rd;
    //unsigned int seed = rd();
    generateRandomOffsets(globalSeed);
}

std::vector<Biome> NoiseGenerator::generateBiomesMap(int width, int height, float offsetX, float offsetY)
{
    std::vector<Biome> biomes = setBiomes();
    std::vector<Biome> biomeMap;
    std::vector<float> continentalnessMap = continentalnessGeneration(width, height, offsetX, offsetY);
    std::vector<float> erosionMap = erosionGeneration(width, height, offsetX, offsetY);
    //std::vector<float> weirdnessMap = weirdnessGeneration(width, height, offsetX, offsetY);
    std::vector<float> heightMap = heightGeneration(width, height, offsetX, offsetY, continentalnessMap, erosionMap);
    std::vector<float> temperatureMap = temperatureGeneration(width, height, offsetX, offsetY, heightMap);
    std::vector<float> humidityMap = humidityGeneration(width, height, offsetX, offsetY, temperatureMap);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            for (const auto& biome : biomes) {
                if (biome.ContinentalnessRange.fits(continentalnessMap.at(index)) &&
                    biome.ErosionRange.fits(erosionMap.at(index)) &&
                    biome.HeightRange.fits(heightMap.at(index)) &&
                    biome.TemperatureRange.fits(temperatureMap.at(index)) &&
                    biome.HumidityRange.fits(humidityMap.at(index)) /*&&
                    biome.WeirdnessRange.fits(weirdnessMap.at(index))*/)
                {
                    biomeMap.push_back(biome);
                    break;
                }
            }
        }
    }
    return biomeMap;
}

std::vector<Biome> NoiseGenerator::setBiomes()
{
    std::vector<Biome> biomes;
    // =================================================================
   // 1. ВОДНАЯ ЛИНИЯ 
   // =================================================================
    biomes.push_back(Biome(
        TileType::Abyss, 
        Range(-hParam.max, -150.0f),
        Range(minFloat, maxFloat), 
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.oceanThreshold),
        Range(eParam.min, eParam.max),
        Range(wParam.min, wParam.max)
    ));
    biomes.push_back(Biome(
        TileType::Ocean, 
        Range(-150.0f, 0.0f), 
        Range(minFloat, maxFloat), 
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.oceanThreshold),
        Range(eParam.min, eParam.max),
        Range(wParam.min, wParam.max)
    ));
    // =================================================================
    // 2. АНОМАЛИИ И РЕДКИЕ ЗОНЫ 
    // =================================================================
    // Вулканы и Горящие земли проверяем до обычных гор и пустынь!
    biomes.push_back(Biome(
        TileType::Volcano, 
        Range(190.0f, hParam.max),
        Range(22.0f, maxFloat), 
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.75f, wParam.max)
    ));
    biomes.push_back(Biome(
        TileType::FireLand, 
        Range(0.0f, 160.0f),
        Range(50.0f, maxFloat), 
        Range(humParam.min, 20.0f),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.75f, wParam.max)
    ));
    biomes.push_back(Biome(
        TileType::GeyserLand, 
        Range(0.0f, 160.0f), 
        Range(40.0f, maxFloat), 
        Range(20.0f, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.75f, wParam.max)
    ));
    biomes.push_back(Biome(
        TileType::Swamp, 
        Range(30.0f, 40.0f), 
        Range(10.0f, 22.0f), 
        Range(45.0f, humParam.max),
        Range(0.15f, сParam.max),
        Range(eParam.min, eParam.max),
        Range(wParam.min, wParam.max)
    ));
    // =================================================================
    // 3. ЛИНИЯ ГОР (Mountains Line: Высота 190+)
    // =================================================================
    biomes.push_back(Biome(
        TileType::IceMountain, 
        Range(190.0f, hParam.max),
        Range(minFloat, 0.0f), 
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::RockyMountain, 
        Range(190.0f, hParam.max),
        Range(0.0f, 22.0f), 
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    // =================================================================
    // 4. ЛИНИЯ ХОЛМОВ (Hills Line: Высота 160 - 190)
    // =================================================================
    biomes.push_back(Biome(
        TileType::SnowyHill, 
        Range(160.0f, 190.0f), 
        Range(minFloat, 2.0f), 
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::DesertDunes, 
        Range(160.0f, 190.0f), 
        Range(22.0f, maxFloat), 
        Range(humParam.min, 30.0f),
        Range(сParam.min, сParam.max),
        Range(eParam.min, 0.2f),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::DryHill, 
        Range(160.0f, 190.0f), 
        Range(22.0f, maxFloat), 
        Range(humParam.min, 20.0f),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::MuddyHill, 
        Range(160.0f, 190.0f), 
        Range(0.0f, 40.0f), 
        Range(humParam.min, 60.0f),
        Range(сParam.min, сParam.max),
        Range(eParam.min, 0.2f),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::ForestHill, 
        Range(160.0f, 190.0f), 
        Range(2.0f, 22.0f), 
        Range(40.0f, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, 0.2f),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::GrassyHill, 
        Range(160.0f, 190.0f), 
        Range(2.0f, 22.0f), 
        Range(humParam.min, 40.0f),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max), //[-1;1] так как существует высокогорное плато в районе холмистой местности с высоким уровнем эрозии
        Range(0.0f, 0.75f)
    ));
    // =================================================================
    // 5. ЛИНИЯ БЕРЕГОВ (Shores Line: Высота 0 - 30)
    // =================================================================
    biomes.push_back(Biome(
        TileType::SnowyShore, 
        Range(0.0f, 30.0f), 
        Range(minFloat, 2.0f), 
        Range(humParam.min, humParam.max),
        Range(сParam.oceanThreshold, сParam.shoreThreshold),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::Shore, 
        Range(0.0f, 30.0f), 
        Range(2.0f, maxFloat), 
        Range(humParam.min, humParam.max),
        Range(сParam.oceanThreshold, сParam.shoreThreshold),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    // =================================================================
    // 6. ЛИНИЯ РАВНИН (Plains Line: Высота 0 - 160)
    // =================================================================
    // Холодные зоны
    biomes.push_back(Biome(
        TileType::IceLand, 
        Range(0.0f, 160.0f),
        Range(minFloat, 0.0f), 
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::Tundra, 
        Range(0.0f, 160.0f), 
        Range(0.0f, 6.0f), 
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    // Экстремально жаркие/сухие зоны (проверяем до обычной пустыни)
    biomes.push_back(Biome(
        TileType::DryLand, 
        Range(0.0f, 160.0f),
        Range(20.0f, maxFloat), 
        Range(humParam.min, 50.0f),
        Range(сParam.min, сParam.max),
        Range(0.2f, eParam.max),
        Range(0.0f, 0.75f)
    ));
    // Стандартные зоны умеренного климата
    biomes.push_back(Biome(
        TileType::Forest, 
        Range(0.0f, 160.0f), 
        Range(6.0f, 20.0f), 
        Range(42.0f, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, 0.2f),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::Plains, 
        Range(0.0f, 160.0f), 
        Range(6.0f, 20.0f), 
        Range(humParam.min, 42.0f),
        Range(сParam.min, сParam.max),
        Range(0.2f, eParam.max),
        Range(0.0f, 0.75f)
    ));
    // Жаркие тропические зоны
    biomes.push_back(Biome(
        TileType::RainForest, 
        Range(0.0f, 160.0f), 
        Range(20.0f, maxFloat), 
        Range(50.0f, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, 0.2f),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::Savannah, 
        Range(0.0f, 160.0f), 
        Range(20.0f, maxFloat), 
        Range(28.0f, 50.0f),
        Range(сParam.min, сParam.max),
        Range(0.2f, eParam.max),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::Desert, 
        Range(0.0f, 160.0f), 
        Range(20.0f, maxFloat), 
        Range(humParam.min, 28.0f),
        Range(сParam.min, сParam.max),
        Range(0.2f, eParam.max),
        Range(0.0f, 0.75f)
    ));
    // =================================================================
    // 7. ВРЕМЕННЫЕ БИОМЫ (аналоги нереализованных биомов)
    // =================================================================
    biomes.push_back(Biome(
        TileType::RainForest,
        Range(0.0f, 160.0f),
        Range(20.0f, maxFloat),
        Range(50.0f, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::Savannah,
        Range(0.0f, 160.0f),
        Range(20.0f, maxFloat),
        Range(humParam.min, 50.0f),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::Plains,
        Range(0.0f, 160.0f),
        Range(6.0f, 20.0f),
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::GrassyHill,
        Range(160.0f, 190.0f),
        Range(2.0f, 22.0f),
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));
    biomes.push_back(Biome(
        TileType::DryHill,
        Range(160.0f, 190.0f),
        Range(22.0f, maxFloat),
        Range(20.0f, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(0.0f, 0.75f)
    ));


    //замена внутриматериковых вод
    biomes.push_back(Biome(
        TileType::Ocean,
        Range(-50.0f, 0.0f),
        Range(minFloat, maxFloat),
        Range(humParam.min, humParam.max),
        Range(сParam.oceanThreshold, сParam.max),
        Range(eParam.min, eParam.max),
        Range(wParam.min, wParam.max)
    ));

    // =================================================================
    // 8. ЗАЩИТНЫЙ СЛОЙ (Аварийный биом)
    // =================================================================
    biomes.push_back(Biome(
        TileType::None, 
        Range(-hParam.max, hParam.max),
        Range(minFloat, maxFloat), 
        Range(humParam.min, humParam.max),
        Range(сParam.min, сParam.max),
        Range(eParam.min, eParam.max),
        Range(wParam.min, wParam.max)
    ));
    return biomes;
}

std::vector<float> NoiseGenerator::humidityGeneration(int width, int height, 
    float offsetX, float offsetY, const std::vector<float>& temperatureMap)
{
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    std::vector<float> humidityMap;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;

            float perlinNoise = perlinFiler(x, y, halfWidth, halfHeight, offsetX, offsetY, 1.0f, humParam.scale, humidityOffset);

            // переводим шум перлина в диапазон [0.0; 1.0]
            float humidityNoise = (perlinNoise + 1.0f) * 0.5f;

            // переводим в диапазон [0.0; 100.0f]
            float regionalRainFall = humidityNoise * humParam.max;

            // Накладываем сдвиг макро-региона на плавные осадки Перлина
            regionalRainFall += voronoiRegionFilter(x, y, offsetX, offsetY, 
                vParam.humWarpFrequency, vParam.humWarpAmplitude, vParam.maxHumOffset, true);

            //получаем температуру в текущей точке
            float currentTemperature = temperatureMap.at(index);
            float airCapacityModifier = 1.0f;

            //меняем влажность в соответствии с температурой
            if (currentTemperature <= humParam.LowestTemperature) {
                float coldDeficit = (humParam.LowestTemperature - currentTemperature) * humParam.coldDryPenalty;
                airCapacityModifier = std::max(0.1f, 1.0f - coldDeficit);
            }
            else if (currentTemperature >= humParam.HighestTemperature) {
                float heatSurplus = (currentTemperature - humParam.HighestTemperature) * humParam.hotMoistureBonus;
                airCapacityModifier = 1.0f + heatSurplus;
            }

            float finalHumidity = regionalRainFall * airCapacityModifier;
            float normalizedHumidity = std::clamp(finalHumidity, humParam.min, humParam.max);
            humidityMap.push_back(normalizedHumidity);
        }
    }
    return humidityMap;
}

std::vector<float> NoiseGenerator::temperatureGeneration(int width, int height,
    float offsetX, float offsetY, const std::vector<float>& heightMap)
{
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    std::vector<float> temperatureMap;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;

            float perlinNoise = perlinFiler(x, y, halfWidth, halfHeight, offsetX, offsetY, 1.0f, tempParam.scale, temperatureOffset);
            // Переводим шум в базовую температуру региона (например, от +10°C до +40°C)
            float regionalTemperature = tempParam.standart + (perlinNoise * tempParam.noiseInfluence);

            // Складываем плавную температуру Перлина и мощный региональный сдвиг Вороного
            regionalTemperature += voronoiRegionFilter(x, y, offsetX, offsetY, vParam.tempWarpFrequency, vParam.tempWarpAmplitude, vParam.maxTempOffset, false);

            // Получаем нормализованную высоту текущей точки [-1.0; 1.0]
            float normalizedHeight = heightMap.at(index) / hParam.max;

            // Рассчитываем падение температуры с высотой (чем выше, тем холоднее)
            // На уровне моря (0.0) штраф равен 0. На пика гор (1.0) вычитается полный noiseInfluenceOnTemperature
            float temperatureDrop = normalizedHeight * tempParam.lossByMaxHeight;

            // Итоговая температура точки в градусах Цельсия
            float finalTemperature = regionalTemperature - temperatureDrop;
            temperatureMap.push_back(finalTemperature);
        }
    }
    return temperatureMap;
}

std::vector<float> NoiseGenerator::erosionGeneration(int width, int height, float offsetX, float offsetY)
{
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    std::vector<float> erosionMap;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float noiseErosion = fractalPerlinFilter(x, y, halfWidth, halfHeight, offsetX, offsetY, eParam.scale, 3, erosionOffsets);

            float finalErosion = noiseErosion / empiricalMax;
            finalErosion = std::clamp(finalErosion, -1.0f, 1.0f);
            erosionMap.push_back(finalErosion);
        }
    }
    return erosionMap;
}

std::vector<float> NoiseGenerator::continentalnessGeneration(int width, int height, float offsetX, float offsetY)
{
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    std::vector<float> continentalnessMap;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            float perlinNoise = fractalPerlinFilter(x, y, halfWidth, halfHeight, offsetX, offsetY, сParam.scale, 3, continentalnessOffsets);
            // Нормализуем базовую континентальность в диапазон [-1.0f; 1.0f]
            float normalizedContinentalness = std::clamp((perlinNoise / empiricalMax), -1.0f, 1.0f);

            float globalX = static_cast<float>(x) + offsetX;
            float globalY = static_cast<float>(y) + offsetY;

            voronoiTectonicFilter(globalX, globalY, true, vParam.continentalnessInfluence, vParam.warpFrequency, vParam.warpAmplitude, normalizedContinentalness);

            float finalContinentalness = std::clamp(normalizedContinentalness, -1.0f, 1.0f);
            continentalnessMap.push_back(finalContinentalness);
        }
    }
    return continentalnessMap;
}


std::vector<float> NoiseGenerator::weirdnessGeneration(int width, int height, float offsetX, float offsetY)
{
    float halfWidth = width / 2.0f; 
    float halfHeight = height / 2.0f; 
    std::vector<float> weirdnessMap;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            float noiseWeirdness = fractalPerlinFilter(x, y, halfWidth, halfHeight, offsetX, offsetY, wParam.scale, 3, weirdnessOffsets);

            float positiveWeirdness = std::abs(noiseWeirdness / empiricalMax);
            float normalizedWeirdness = std::clamp(positiveWeirdness, 0.0f, 1.0f);
            weirdnessMap.push_back(normalizedWeirdness);
        }
    }
    return weirdnessMap;
}

std::vector<float> NoiseGenerator::heightGeneration(int width, int height, float offsetX, float offsetY, 
    const std::vector<float>& continentalnessMap, const std::vector<float>& erosionMap)
{
    float halfWidth = width / 2.0f; 
    float halfHeight = height / 2.0f;
    std::vector<float> heightMap;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;

            float noiseHeight = fractalPerlinFilter(x, y, halfWidth, halfHeight, offsetX, offsetY, hParam.scale, 5, octaveOffsets);

            //получаем нормализованную высоту и получаем соответсвующее значение высоты в диапазоне [-1.0f;1.0f] * maxheight
            float normalizeHeight = std::clamp(noiseHeight / empiricalMax, -1.0f, 1.0f);

            float currentErosion = erosionMap.at(index);
            float erosedHeightNoise = erosionFilter(currentErosion, normalizeHeight);
            float pvInfluence = peakAndValleyFilter(currentErosion);
            float currentContinentalness = continentalnessMap.at(index);
            float finalNormalizeHeight = continentalnessFilter(currentContinentalness, erosedHeightNoise, pvInfluence);

            float globalX = static_cast<float>(x) + offsetX;
            float globalY = static_cast<float>(y) + offsetY;
            voronoiTectonicFilter(globalX, globalY, (finalNormalizeHeight > сParam.oceanThreshold), vParam.heightBonus, 
                vParam.warpFrequency, vParam.warpAmplitude, finalNormalizeHeight);
            voronoiRiverFilter(globalX, globalY, finalNormalizeHeight);

            finalNormalizeHeight = std::clamp(finalNormalizeHeight, -1.0f, 1.0f) * hParam.max;
            heightMap.push_back(finalNormalizeHeight);
        }
    }
    return heightMap;
}


float NoiseGenerator::fractalPerlinFilter(int x, int y, int halfW, int halfH, float offsetX, float offsetY, float scale, int octaves, 
    const std::vector<Vector2>& offsets)
{
    //начальная амплитуда, частота и высота текущей координаты 
    fpParam.amplitude = 1.0f;
    fpParam.frequency = 1.0f;
    float noiseHeight = 0.0f;
    //проход по октавам
    for (int i = 0; i < octaves; i++) {
        float perlinNoise = perlinFiler(x, y, halfW, halfH, offsetX, offsetY, fpParam.frequency, scale, offsets[i]);
        noiseHeight += perlinNoise * fpParam.amplitude;
        fpParam.amplitude *= fpParam.persistence;
        fpParam.frequency *= fpParam.lacunarity;
    }
    return noiseHeight;
}

float NoiseGenerator::perlinFiler(int x, int y, float halfW, float halfH, float offsetX, float offsetY, 
    float frequency, float scale, const Vector2& offset)
{
    float sampleX = (static_cast<float>(x) - halfW + offsetX + offset.x) / scale * frequency;
    float sampleY = (static_cast<float>(y) - halfH + offsetY + offset.y) / scale * frequency;
    return noise.createNoise2d(sampleX, sampleY);
}

float NoiseGenerator::erosionFilter(float currentErosion, float normalizedHeight)
{
    float erosedHeightNoise = normalizedHeight;
    if (currentErosion > eParam.plainsThreshold) {
        float intensity = (currentErosion - eParam.plainsThreshold) * 1.5f;
        intensity = std::clamp(intensity, 0.0f, 1.0f);
        erosedHeightNoise = normalizedHeight + intensity * (0.0f - normalizedHeight);
    }
    return erosedHeightNoise;
}

float NoiseGenerator::peakAndValleyFilter(float currentErosion)
{
    return (std::abs(currentErosion) * eParam.pvAmplitude) - eParam.pvOffset;
}

float NoiseGenerator::continentalnessFilter(float currentContinentalness, float erosedHeightNoise, float PeakValleyInfluence)
{
    float modifiedHeight = 0.0f;
    if (currentContinentalness < сParam.oceanThreshold) {
        modifiedHeight = currentContinentalness * (1.0f + (erosedHeightNoise * сParam.oceanInfluence));
    }
    else if (currentContinentalness >= сParam.oceanThreshold && currentContinentalness < сParam.shoreThreshold) {
        float t = (currentContinentalness - сParam.oceanThreshold) / (сParam.shoreThreshold - сParam.oceanThreshold);
        float baseShore = сParam.shoreBaseFloor + (t * сParam.shoreHeightRange);
        modifiedHeight = baseShore + (erosedHeightNoise * сParam.shoreInfluence);
        //для того чтобы убрать плавную линию разделения берега и воды, резкий переход
        modifiedHeight = std::max(modifiedHeight, 0.0f); 
    }
    else {
        // СУША: Теперь базовый фундамент зависит от Continentalness, рельеф зависит от приглушенного Эрозией Перлина, а PV вырезает долины
        float baseLandFloor = сParam.landBaseFloor + (currentContinentalness * сParam.landHeightMultiplier);
        modifiedHeight = baseLandFloor + (erosedHeightNoise * сParam.landInfluence) + PeakValleyInfluence;
    }
    return modifiedHeight;
}

void NoiseGenerator::voronoiTectonicFilter(float globalX, float globalY, bool condition, float influence, float frequency, float amplitude, float& metric)
{
    float warpX = noise.createNoise2d(globalX * frequency, globalY * frequency) * amplitude;
    float warpY = noise.createNoise2d(globalY * frequency, globalX * frequency) * amplitude;

    float distToEdge = voronoi.getDistanceToEdge(globalX + warpX, globalY + warpY);
    
    // Если точка ландшафта лежит в зоне тектонического разлома и это суша (высота > 0.0f)
    if (distToEdge < vParam.zoneWidth && condition)
    {
        // Линейная интенсивность от 0.0 на краю до 1.0 строго на границе
        float intensity = 1.0f - (distToEdge / vParam.zoneWidth);
        // Сглаживаем профиль горы (делаем пик более острым, а подножье пологим)
        intensity = intensity * intensity * (3.0f - 2.0f * intensity);
        // Плавно вздымаем рельеф Перлина на стыке плит
        metric += intensity * influence;
    }
}

void NoiseGenerator::voronoiRiverFilter(float globalX, float globalY, float& normalizedHeight)
{
    if (normalizedHeight > сParam.oceanThreshold)
    {
        float riverWarpX = noise.createNoise2d(globalX * vParam.riverFrequency, globalY * vParam.riverFrequency) * vParam.riverAmplitude;
        float riverWarpY = noise.createNoise2d(globalY * vParam.riverFrequency, globalX * vParam.riverFrequency) * vParam.riverAmplitude;
        float distToRiverEdge = voronoiRiver.getDistanceToEdge(globalX + riverWarpX, globalY + riverWarpY);

        if (distToRiverEdge < vParam.riverValleyWidth)
        {
            float riverIntensity = std::pow(1.0f - (distToRiverEdge / vParam.riverValleyWidth), 2);
            if (distToRiverEdge < vParam.riverChannelWidth) {
                // Русло уводим ниже уровня моря (вода)
                normalizedHeight = -0.05f; 
            }
            else {
                normalizedHeight -= riverIntensity * vParam.riverDepthNormalized;
                normalizedHeight = std::max(normalizedHeight, -0.05f);
            }
        }
    }
}

float NoiseGenerator::voronoiRegionFilter(int x, int y, float offsetX, float offsetY,
    float frequency, float amplitude, float maxOffset, bool randReset)
{
    float globalX = static_cast<float>(x) + offsetX;
    float globalY = static_cast<float>(y) + offsetY;
    // Искажаем пространство шумом Перлина, чтобы границы метрики(температура, влажность) извивались
    float warpX = noise.createNoise2d(globalX * frequency, globalY * frequency) * amplitude;
    float warpY = noise.createNoise2d(globalY * frequency, globalX * frequency) * amplitude;

    // ID макро-региона для деформированной мировой координаты
    int regionID = voronoi.getRegionID(globalX + warpX, globalY + warpY);

    // Инициализируем mt19937 от ID этого региона и сида мира
    rng.seed(regionID ^ globalSeed);
    // Перемешиваем стейт генератора, чтобы сдвиг не был один в один
    // (берем второе псевдослучайное число из этой же последовательности)
    if (randReset) rng.discard(1);
    // Генерируем детерминированный сдвиг температуры для всей плиты Вороного
    // Из-за этого сдвига один макро-регион станет стабильно холодным, а другой — жарким
    std::uniform_real_distribution<float> offsetDist(-maxOffset, maxOffset);
    float regionMetricOffset = offsetDist(rng);

    return regionMetricOffset;
}

void NoiseGenerator::generateRandomOffsets(unsigned int seed)
{
    rng.seed(seed);
    std::uniform_real_distribution<float> distribution(-1000000.0f, 1000000.0f);
    for (size_t i = 0; i < 5; i++) {
        Vector2 vec;
        vec.x = distribution(rng);
        vec.y = distribution(rng);
        octaveOffsets.push_back(vec);
    }
    for (size_t i = 0; i < 3; i++) {
        Vector2 vec;
        vec.x = distribution(rng);
        vec.y = distribution(rng);
        continentalnessOffsets.push_back(vec);

        vec.x = distribution(rng);
        vec.y = distribution(rng);
        erosionOffsets.push_back(vec);

        vec.x = distribution(rng);
        vec.y = distribution(rng);
        weirdnessOffsets.push_back(vec);
    }
    temperatureOffset.x = distribution(rng);
    temperatureOffset.y = distribution(rng);

    humidityOffset.x = distribution(rng);
    humidityOffset.y = distribution(rng);
}


void NoiseGenerator::saveVectorBinary(const std::string& filename, const std::vector<float>& data)
{
    std::ios_base::sync_with_stdio(false);
    std::ofstream outFile(filename);
    if (!outFile) return;
    for (float value : data) {
        outFile << value << '\n';
    }
    outFile.close();
}