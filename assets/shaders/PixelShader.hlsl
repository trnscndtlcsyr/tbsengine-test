struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float4 col : COLOR;
    float3 normal : NORMAL; 
    float4 lightSpacePos : TEXCOORD0;
};

cbuffer LightBuffer : register(b0)
{
    float3 lightDir; // {0.5, -1.0, 0.5}
    float padding;
};

Texture2D shadowMap : register(t1);
SamplerState shadowSampler : register(s1);

float4 main(VS_OUTPUT input) : SV_TARGET
{
    //РАСЧЕТ ОБЪЕМА (Shading)
    float3 N = normalize(input.normal);
    float3 L = normalize(-lightDir); // Направление К источнику
    float diffuseIntensity = saturate(dot(N, L)); // Насколько ярко освещен склон

    //РАСЧЕТ ТЕНИ (Shadow Mapping)
    float3 projCoords = input.lightSpacePos.xyz / input.lightSpacePos.w;
    float2 shadowTexCoord = float2(projCoords.x * 0.5f + 0.5f, -projCoords.y * 0.5f + 0.5f);
    
    float currentDepth = projCoords.z;
    float closestDepth = shadowMap.Sample(shadowSampler, shadowTexCoord).r;
    
    float bias = 0.1f;
    // Если в тени — 0.0 (нет света), если на свету — 1.0 (есть свет)
    float shadow = (currentDepth - bias > closestDepth) ? 0.0f : 1.0f;

    //ИТОГОВЫЙ ЦВЕТ
    float4 ambient = input.col * 0.15f; // Постоянный тусклый свет везде
    
    // Прямой свет (diffuse) работает только там, где нет тени (shadow)
    float4 directLight = (input.col * diffuseIntensity) * shadow;

    return ambient + directLight;
}