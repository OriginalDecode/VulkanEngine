#pragma once
#include "Core/Types.h"
#include "Utilities.h"
struct ID3D10Blob;
namespace Graphics
{
	class Shader;
	class ShaderFactory
	{
	public:
		ShaderFactory() = default;
		~ShaderFactory() { }

		Shader* CompileShader(const char* filepath, const char* entrypoint, ShaderType shaderType, ID3D10Blob** blobOut);
		Shader* CompileShader(const uint8* const pData, int32 dataSize, ShaderType shaderType, const char* entrypoint, const char* source, ID3D10Blob** blobOut);
	private:
		Shader* CompileShaderInternal(const uint8* const pData, int32 dataSize, ShaderType shaderType, const char* entrypoint, const char* source, ID3D10Blob** blobOut);
	};

}; //namespace Graphics