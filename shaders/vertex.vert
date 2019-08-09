

[[vk::push_constant]] cbuffer pcBuf { row_major float4x4 world; };

cbuffer viewProjection : register (b0)
{
    row_major float4x4 viewProjection;
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


float4 Mul( float4 position, float4x4 mat )
{
    float4 temp = position;

    temp.x = dot(position, mat._m00_m01_m02_m03);
    temp.y = dot(position, mat._m10_m11_m12_m13);
    temp.z = dot(position, mat._m20_m21_m22_m23);
    temp.w = dot(position, mat._m30_m31_m32_m33);

    return temp;
};


VSOutput main(VSInput input, uint vertex_id : SV_VertexID) 
{
    VSOutput output = (VSOutput)0;

    output.position = Mul(input.position, world);
    // output.position = mul(output.position, viewProjection);
    //output.position = mul(output.position, Projection);
    // output.position = input.position;
    output.color = input.color; //float4(1,1,1,1);
    return output;
}