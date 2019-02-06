#pragma once

#include "GraphicsDevice.h"

#ifdef _WIN32
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Buffer;
#endif

class Window;
namespace Graphics
{
	void Release(ID3D11Buffer* buffer);

	GraphicsDevice& GetDevice();

	class GraphicsEngine
	{
	public:
	
		static void Create();
		static void Destroy();
		static GraphicsEngine* Get();

		void Init(const Window& window);
		void Present();

		static GraphicsDevice& GetDevice();
	private:
		static GraphicsEngine* s_Instance;
		GraphicsEngine() = default;
		~GraphicsEngine();
		void BeginFrame();

		ID3D11RenderTargetView* m_DefaultRenderTarget = nullptr;
		ID3D11DepthStencilView* m_DefaultDepthTarget = nullptr;

		GraphicsDevice m_Device;


	};






}; //namespace Graphics