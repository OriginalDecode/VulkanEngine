#pragma once

#include "GraphicsDevice.h"
#include <memory>

class Window;
namespace Graphics
{
	class vkGraphicsDevice;
	class GraphicsEngine
	{
	public:
		GraphicsEngine();
		~GraphicsEngine();
	
		static void Create();
		static GraphicsEngine& Get();

		bool Init(const Window& window);
		void Present();

	private:
		static std::unique_ptr<GraphicsEngine> m_Instance;

		void BeginFrame();

		std::unique_ptr<vkGraphicsDevice> m_Device;

	};

}; //namespace Graphics