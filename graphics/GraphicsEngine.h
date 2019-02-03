#pragma once

#ifdef _WIN32
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
#endif

class Window;
namespace Graphics
{
	ID3D11Device* GetDevice();
	class GraphicsEngine
	{
	public:
		GraphicsEngine() = default;
		~GraphicsEngine();
	
		void CreateDeviceAndSwapchain(const Window& window);
	
		void Present();

	private:
		void BeginFrame();

		ID3D11RenderTargetView* m_DefaultRenderTarget = nullptr;
		ID3D11DepthStencilView* m_DefaultDepthTarget = nullptr;

	};

}; //namespace Graphics