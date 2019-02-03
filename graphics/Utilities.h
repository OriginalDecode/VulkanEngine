#pragma once
#include <CommonLib/Types.hpp>
#include <CommonLib/Defines.h>

enum ShaderType : uint8
{
	EShaderType_VERTEX		= BIT(0),
	EShaderType_FRAGMENT	= BIT(1),
	EShaderType_GEOMETRY	= BIT(2),
	EShaderType_HULL		= BIT(3),
	EShaderType_DOMAIN		= BIT(4),
	EShaderType_COMPUTE		= BIT(5)
};
