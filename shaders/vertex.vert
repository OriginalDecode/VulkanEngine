
float2 positions[] = {
    float2(1.0, 0.0),
    float2(0.0, 1.0),
    float2(-1.0, 0.0)
};

float4 main(uint vertex_id : SV_VertexID) : SV_Position {
    return float4(positions[vertex_id], 0.0, 1.0);
}