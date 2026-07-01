cbuffer WorldBuffer : register(b1)
{
    matrix Transform;
};

cbuffer LightBuffer : register(b2)
{
    matrix lightSpaceMatrix;
};

struct VS_INPUT
{
    float3 pos : POSITION;
};

float4 main(VS_INPUT input) : SV_POSITION
{
    float4 worldPos = mul(float4(input.pos, 1.0f), Transform);
    return mul(worldPos, lightSpaceMatrix);
}