#pragma once

#include "GraphicsDevice.h"

class Window;
namespace Graphics
{
	void CreateImGuiContext();
	class VulkanRenderer;
	class GraphicsEngine
	{
	public:
		static void Create();
		static void Destroy();
		static GraphicsEngine& Get();

		bool Init(const Window& window);

		const Window* GetWindow() const { return m_Window; }

	private:
		GraphicsEngine() = default;
		~GraphicsEngine() = default;
		static GraphicsEngine* m_Instance;

		const Window* m_Window{};
	};

}; // namespace Graphics
