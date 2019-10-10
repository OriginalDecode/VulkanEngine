

[[vk::push_constant]] cbuffer pcBuf { row_major float4x4 world; };

cbuffer viewProjection : register (b0)
{
    row_major float4x4 view;
    row_major float4x4 projection;
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

    output.position = mul(input.position, world);
    
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    
    // output.position = mul(output.position, viewProjection);
    output.color = input.color; //float4(1,1,1,1);
    return output;
}