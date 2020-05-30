// -E main -T vs_6_0

struct PushConstants
{
    row_major float4x4 world; 
};

[[vk::push_constant]] PushConstants pc;

cbuffer viewProjection : register (b0)
{
    row_major float4x4 viewProj;
    float4 lightDir; 
};

struct VSInput 
{
    float4 position : POSITION;
    float4 color : COLOR;
    float4 normal : NORMAL;
};

struct VSOutput 
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 normal : NORMAL;
    float4 lightDir : LIGHT;
};

VSOutput main(VSInput input, uint vertex_id : SV_VertexID) 
{
    VSOutput output = (VSOutput)0;

    output.position = mul(input.position, pc.world);
    output.position = mul(output.position, viewProj);
    output.lightDir = lightDir;

    // output.normal = mul(input.normal, pc.world);
    output.normal = input.normal;
    output.color = input.color; //float4(1,1,1,1);
    return output;
}