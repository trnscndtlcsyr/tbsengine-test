cbuffer CameraBuffer : register(b0)
{
    matrix ViewProjection;
};

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
    float4 col : COLOR;
};

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float4 col : COLOR;
    float4 lightSpacePos : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 worldPos = mul(float4(input.pos, 1.0f), Transform);
    output.pos = mul(worldPos, ViewProjection);
    output.lightSpacePos = mul(worldPos, lightSpaceMatrix);
    output.col = input.col;
    return output;
}