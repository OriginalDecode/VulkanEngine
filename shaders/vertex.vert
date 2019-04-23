
float2 positions[] = {
    float2(1.0, 0.0),
    float2(0.0, 1.0),
    float2(-1.0, 0.0)
};

float3 color[] = {
    float3(1,0,0),
    float3(0,1,0),
    float3(0,0,1)
};

struct VSOutput 
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput main(uint vertex_id : SV_VertexID) {
    
    VSOutput output = (VSOutput)0;

    output.position = float4(positions[vertex_id], 0.0, 1.0);
    output.color = float4(color[vertex_id], 1.0);

    return output;
}