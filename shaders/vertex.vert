
cbuffer mvp : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 View;
    row_major float4x4 Projection;
};

struct VSInput 
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VSOutput 
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput main(VSInput input, uint vertex_id : SV_VertexID) 
{
    VSOutput output = (VSOutput)0;

    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    // output.position = input.position;
    output.color = input.color; //float4(1,1,1,1);
    return output;
}