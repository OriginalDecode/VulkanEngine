#pragma once
#include <Core/Types.h>
#include <Core/Defines.h>

#ifdef _WIN32
#include <d3d11.h>
#endif

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
	
	enum ETextureFormat
	{
		NO_FORMAT 			= BIT(0),
		RGBA32_FLOAT 		= BIT(1),
		RGBA32_UINT 		= BIT(2),
		RGBA32_SINT 		= BIT(3),

		RGB32_FLOAT 		= BIT(4),
		RGB32_UINT 			= BIT(5),
		RGB32_SINT 			= BIT(6),

		RG32_FLOAT 			= BIT(7),
		RG32_UINT 			= BIT(8),

		RGBA16_FLOAT 		= BIT(9),
		RGBA16_UINT 		= BIT(10),
		RGBA16_SINT 		= BIT(11),

		RGBA8_UINT 			= BIT(12),
		RGBA8_SINT 			= BIT(13),

		R32_TYPELESS 		= BIT(14),
		R32_FLOAT 			= BIT(15),
		R32_UINT 			= BIT(16),
		DEPTH_32_FLOAT		= BIT(17),

		RGBA8_UNORM 		= BIT(18),
		RGB10A2_TYPELESS 	= BIT(19),
		RGBA8_TYPELESS 		= BIT(20),
		sRGBA8 				= BIT(21),
	};

	enum ETopology
	{
		TRIANGLE_LIST,
		POINT_LIST,
		LINE_LIST,
		_4_CONTROL_POINT_PATCHLIST,
	};

}; //namespace Graphics