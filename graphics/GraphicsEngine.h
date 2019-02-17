#pragma once

#include "GraphicsDevice.h"

class Window;
namespace Graphics
{
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

		GraphicsDevice m_Device;
	};

}; //namespace Graphics