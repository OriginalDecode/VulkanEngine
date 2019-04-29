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
		void Present(float dt);

		static vkGraphicsDevice& GetDevice() { return *m_Instance->m_Device; }

	private:
		static std::unique_ptr<GraphicsEngine> m_Instance;
		std::unique_ptr<vkGraphicsDevice> m_Device;

		void BeginFrame();


	};

}; //namespace Graphics