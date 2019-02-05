#pragma once
#include <Core/Types.h>
#include <Core/Defines.h>

#include <d3d11.h>
#define ARRSIZE(x) sizeof(x) / sizeof(x[0])

namespace Graphics
{

	enum ShaderType : uint8
	{
		EShaderType_VERTEX		= BIT(0),
		EShaderType_FRAGMENT	= BIT(1),
		EShaderType_GEOMETRY	= BIT(2),
		EShaderType_HULL		= BIT(3),
		EShaderType_DOMAIN		= BIT(4),
		EShaderType_COMPUTE		= BIT(5)
	};

	enum EBindFlag
	{
		NONE					= BIT(0),
		BIND_VERTEX_BUFFER		= BIT(1),
		BIND_INDEX_BUFFER		= BIT(2),
		BIND_CONSTANT_BUFFER	= BIT(3),
		BIND_SHADER_RESOURCE	= BIT(4),
		BIND_STREAM_OUTPUT		= BIT(5),
		BIND_RENDER_TARGET		= BIT(6),
		BIND_DEPTH_STENCIL		= BIT(7),
		BIND_UNORDERED_ACCESS	= BIT(8),
		BIND_DECODER			= BIT(9),
		BIND_VIDEO_ENCODER		= BIT(10)
	};

	enum EUsage
	{
		DEFAULT_USAGE			= BIT(0),
		IMMUTABLE_USAGE			= BIT(1),
		DYNAMIC_USAGE			= BIT(2),
		STAGING_USAGE			= BIT(3),
	};

	enum ECPUAccessFlag
	{
		NO_ACCESS_FLAG			= BIT(0),
		READ					= BIT(1),
		WRITE					= BIT(2),
	};

	enum ETopology
	{
		TRIANGLE_LIST,
		POINT_LIST,
		LINE_LIST,
		_4_CONTROL_POINT_PATCHLIST,
	};

	
	enum ETextureFormat
	{
		NO_FORMAT = 1 << 0,
		RGBA32_FLOAT = 1 << 1,
		RGBA32_UINT = 1 << 2,
		RGBA32_SINT = 1 << 3,

		RGB32_FLOAT = 1 << 4,
		RGB32_UINT = 1 << 5,
		RGB32_SINT = 1 << 6,

		RG32_FLOAT = 1 << 7,
		RG32_UINT = 1 << 8,

		RGBA16_FLOAT = 1 << 9,
		RGBA16_UINT = 1 << 10,
		RGBA16_SINT = 1 << 11,

		RGBA8_UINT = 1 << 12,
		RGBA8_SINT = 1 << 13,

		R32_TYPELESS = 1 << 14,
		R32_FLOAT = 1 << 15,
		R32_UINT = 1 << 16,
		DEPTH_32_FLOAT = 1 << 17,

		RGBA8_UNORM = 1 << 18,
		RGB10A2_TYPELESS = 1 << 19,
		RGBA8_TYPELESS = 1 << 20,
		sRGBA8 = 1 << 21,
	};


	D3D11_USAGE GetUsage(int32 usage);

	uint32 GetBindFlag(int32 binding);

	uint32 GetCPUAccessFlag(int32 flags);

	D3D11_PRIMITIVE_TOPOLOGY GetTopology(ETopology topology);

}; //namespace Graphics