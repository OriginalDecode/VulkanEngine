#pragma once

#include "GraphicsDevice.h"

class Window;
namespace Graphics
{
	class vkGraphicsDevice;
	class GraphicsEngine
	{
	public:
	
		static void Create();
		static void Destroy();
		static GraphicsEngine* Get();

		void Init(const Window& window);
		void Present();

	private:
		static GraphicsEngine* s_Instance;
		GraphicsEngine() = default;
		~GraphicsEngine();
		void BeginFrame();

		vkGraphicsDevice* m_Device = nullptr;

	};

}; //namespace Graphics