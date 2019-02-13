#include "ShaderFactory.h"

#include "GraphicsDevice.h"
#include "GraphicsEngine.h"
#include "Shader.h"

// #include "DL_Debug/DL_Debug.h"
#include "gamesystems/ResourceCache.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#include <cstdio>
#include <cassert>


namespace Graphics
{
	Shader* ShaderFactory::CompileShader(const char* filepath, const char* entrypoint, ShaderType shaderType, ID3D10Blob** blobOut)
	{
		FILE* hFile = nullptr;
		errno_t err = fopen_s(&hFile, filepath, "rb");
		
		if (err != 0)
		{
			assert(!"Failed to open file!");
		}

		const int32 length = fseek(hFile, 0, SEEK_END);
		fseek(hFile, 0, SEEK_SET);

		uint8* data = new uint8[length];
		
		fread(data, 1, length, hFile);
		fclose(hFile);

		Shader* shader = CompileShaderInternal(data, length, shaderType, entrypoint, filepath, blobOut);

#ifndef _DEBUG
		delete data;
		data = nullptr;
#endif

		return shader;
	}

	Shader* ShaderFactory::CompileShader(const uint8* const pData, int32 dataSize, ShaderType shaderType, const char* entrypoint, const char* source, ID3D10Blob** blobOut)
	{
		return CompileShaderInternal(pData, dataSize, shaderType, entrypoint, source, blobOut);
	}

	Shader* ShaderFactory::CompileShaderInternal(const uint8* const pData, int32 dataSize, ShaderType shaderType, const char* entrypoint, const char* source, ID3D10Blob** blobOut)
	{
		
		if (IResource* shader = ResourceCache::Get()->IsCached(source))
		{
			return static_cast<Shader*>(shader);
		}

		uint32 shaderFlags = 0;
#ifdef _DEBUG
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
		shaderFlags |= D3D10_SHADER_ENABLE_STRICTNESS; 
#else
		shaderFlags |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

		const D3D_SHADER_MACRO defines[] = {
#ifdef _DEBUG
			"_DEBUG", "1",
#else
			"_FINAL", "1",
#endif
			0, 0
		};
		uint32 shaderFlags2 = 0;

		std::string featureLevel;
        
        switch(shaderType)
        {
             case EShaderType_VERTEX:
                featureLevel = "vs"; 
                break;
            case EShaderType_FRAGMENT:
                featureLevel = "ps";
                break;
            case EShaderType_GEOMETRY:
                featureLevel = "gs";
                break;
            case EShaderType_DOMAIN:
                featureLevel = "ds";
                break;
            case EShaderType_HULL:
                featureLevel = "hs";
                break;
            case EShaderType_COMPUTE:
                featureLevel = "cs";
                break;
        }

		featureLevel += "_5_0";
		
		ID3D10Blob *dataBlob, *outMessage;
		HRESULT hr = D3DCompile(
			pData, 
			dataSize, 
			source, 
			defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, 
			entrypoint, 
			featureLevel.c_str(),
			shaderFlags, 
			shaderFlags2,
			&dataBlob, 
			&outMessage);

		if (outMessage)
		{
			char temp[SHRT_MAX];
			sprintf_s(temp, "%s has generated warnings!\n%s", "shader", (char*)outMessage->GetBufferPointer());
			OutputDebugStringA(temp);
		}

		assert(!FAILED(hr) && "Failed to create shader");

		blobOut = &dataBlob;
		Shader* shader = nullptr;
        void* shaderData = Graphics::GraphicsEngine::GetDevice().CreateShader(dataBlob, shaderType);

        switch(shaderType)
        {
            case EShaderType_VERTEX: 
			    shader = new Shader( [&](void* pShader, ID3D11DeviceContext* ctx) { ctx->VSSetShader(static_cast<ID3D11VertexShader*>(pShader), nullptr, 0); }, shaderData);
                break;
            case EShaderType_FRAGMENT:
			    shader = new Shader([&](void* pShader, ID3D11DeviceContext* ctx) { ctx->PSSetShader(static_cast<ID3D11PixelShader*>(pShader), nullptr, 0); }, shaderData);
                break;
            case EShaderType_GEOMETRY:
			    shader = new Shader([&](void* pShader, ID3D11DeviceContext* ctx) { ctx->GSSetShader(static_cast<ID3D11GeometryShader*>(pShader), nullptr, 0); }, shaderData);
                break;
            case EShaderType_DOMAIN:
    			shader = new Shader([&](void* pShader, ID3D11DeviceContext* ctx) { ctx->DSSetShader(static_cast<ID3D11DomainShader*>(pShader), nullptr, 0); }, shaderData);
                break;
            case EShaderType_HULL:
	    		shader = new Shader([&](void* pShader, ID3D11DeviceContext* ctx) { ctx->HSSetShader(static_cast<ID3D11HullShader*>(pShader), nullptr, 0); }, shaderData);
                break;
            case EShaderType_COMPUTE:
		    	shader = new Shader([&](void* pShader, ID3D11DeviceContext* ctx) { ctx->CSSetShader(static_cast<ID3D11ComputeShader*>(pShader), nullptr, 0); }, shaderData);
                break;
        }
		
		ResourceCache::Get()->Cache(shader, source);

		return shader;

	}

}; //namespace Graphics