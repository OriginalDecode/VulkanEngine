// -E main -T ps_6_0

struct VSOutput 
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 normal : NORMAL;
    float4 lightDir : LIGHT;
};


float4 main(VSOutput input) : SV_Target 
{
    float3 normal = normalize(input.normal.xyz);
    float3 output = dot(normal, -input.lightDir.xyz);
    return saturate(input.color * float4(1,1,0,1) * float4(output, 1)) + 1 * 0.42;
}