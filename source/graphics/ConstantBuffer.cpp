// #include "ConstantBuffer.h"

// #include "GraphicsEngine.h"
// #include "Utilities.h"

// #include <d3d11.h>

// ConstantBuffer::~ConstantBuffer()
// {
// 	m_Buffer->Release();
// }

// void ConstantBuffer::Bind(const uint32 index, const uint32 shader_binding)
// {
// 	const uint32 index_array[6] = { index, index, index, index, index, index };
// 	Bind(index_array, shader_binding);
// }

// void ConstantBuffer::Bind(const uint32 index[], const uint32 shader_binding)
// {
// 	ID3D11Device* device = Graphics::GetDevice();
// 	ID3D11DeviceContext* context = nullptr;
// 	device->GetImmediateContext(&context);
// 	D3D11_MAPPED_SUBRESOURCE subresource;
// 	ZeroMemory(&subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

// 	HRESULT hr = context->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
// 	assert(!FAILED(hr) && "Failed to map buffer to graphics memory!");


// 	memcpy(&static_cast<uint8*>(subresource.pData)[0], &m_Variables[0], m_BufferSize);

// 	context->Unmap(m_Buffer, 0);

// 	if (shader_binding & EShaderType_VERTEX)
// 		context->VSSetConstantBuffers(index[0], 1, &m_Buffer);

// 	if (shader_binding & EShaderType_FRAGMENT)
// 		context->PSSetConstantBuffers(index[1], 1, &m_Buffer);

// 	if (shader_binding & EShaderType_GEOMETRY)
// 		context->GSSetConstantBuffers(index[2], 1, &m_Buffer);

// 	if (shader_binding & EShaderType_HULL)
// 		context->HSSetConstantBuffers(index[3], 1, &m_Buffer);

// 	if (shader_binding & EShaderType_DOMAIN)
// 		context->DSSetConstantBuffers(index[4], 1, &m_Buffer);

// 	if (shader_binding & EShaderType_COMPUTE)
// 		context->CSSetConstantBuffers(index[5], 1, &m_Buffer);

// 	context->Release();

// }

