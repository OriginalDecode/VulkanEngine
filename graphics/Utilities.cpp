#include "Utilities.h"

namespace Graphics
{
#ifdef _WIN32
	D3D11_USAGE GetUsage(int32 usage)
	{
		if (usage == DEFAULT_USAGE)
			return D3D11_USAGE_DEFAULT;
		if (usage == IMMUTABLE_USAGE)
			return D3D11_USAGE_IMMUTABLE;
		if (usage == DYNAMIC_USAGE)
			return D3D11_USAGE_DYNAMIC;
		if (usage == STAGING_USAGE)
			return D3D11_USAGE_STAGING;

		return D3D11_USAGE_DEFAULT;
	}

	uint32 GetBindFlag(int32 binding)
	{
		uint32 output = 0;

		if (binding & Graphics::BIND_VERTEX_BUFFER)
			output |= D3D11_BIND_VERTEX_BUFFER;
		if (binding & Graphics::BIND_INDEX_BUFFER)
			output |= D3D11_BIND_INDEX_BUFFER;
		if (binding & Graphics::BIND_CONSTANT_BUFFER)
			output |= D3D11_BIND_CONSTANT_BUFFER;
		if (binding & Graphics::BIND_SHADER_RESOURCE)
			output |= D3D11_BIND_SHADER_RESOURCE;
		if (binding & Graphics::BIND_STREAM_OUTPUT)
			output |= D3D11_BIND_STREAM_OUTPUT;
		if (binding & Graphics::BIND_RENDER_TARGET)
			output |= D3D11_BIND_RENDER_TARGET;
		if (binding & Graphics::BIND_DEPTH_STENCIL)
			output |= D3D11_BIND_DEPTH_STENCIL;
		if (binding & Graphics::BIND_UNORDERED_ACCESS)
			output |= D3D11_BIND_UNORDERED_ACCESS;
		if (binding & Graphics::BIND_DECODER)
			output |= D3D11_BIND_DECODER;
		if (binding & Graphics::BIND_VIDEO_ENCODER)
			output |= D3D11_BIND_VIDEO_ENCODER;


		return output;
	}

	uint32 GetCPUAccessFlag(int32 flags)
	{
		uint32 output = 0;

		if (flags & ECPUAccessFlag::READ)
			output |= D3D11_CPU_ACCESS_READ;
		if (flags& ECPUAccessFlag::WRITE)
			output |= D3D11_CPU_ACCESS_WRITE;

		return output;
	}

	D3D11_PRIMITIVE_TOPOLOGY GetTopology(ETopology topology)
	{
		if (topology == ETopology::TRIANGLE_LIST)
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		if (topology == ETopology::_4_CONTROL_POINT_PATCHLIST)
			return D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
		if (topology == ETopology::POINT_LIST)
			return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		if (topology == ETopology::LINE_LIST)
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
#endif

}; //namespace Graphics