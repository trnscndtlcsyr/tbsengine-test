struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float4 col : COLOR;
    float4 lightSpacePos : TEXCOORD0;
};

Texture2D shadowMap : register(t1);
SamplerState shadowSampler : register(s1);

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 projCoords = input.lightSpacePos.xyz / input.lightSpacePos.w;
    float2 shadowTexCoord = float2(projCoords.x * 0.5f + 0.5f, -projCoords.y * 0.5f + 0.5f);
    float currentDepth = projCoords.z;
    float bias = 0.001f;
    float closestDepth = shadowMap.Sample(shadowSampler, shadowTexCoord).r;
    float shadow = (currentDepth - bias > closestDepth) ? 0.3f : 1.0f;
    return input.col * shadow;
}