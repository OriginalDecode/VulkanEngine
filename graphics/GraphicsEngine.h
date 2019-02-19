#pragma once

#include "GraphicsDevice.h"

class Window;
namespace Graphics
{
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

	};

}; //namespace Graphics