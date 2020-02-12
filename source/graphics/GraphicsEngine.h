#pragma once

class Window;
namespace Graphics
{
	void CreateImGuiContext();
	class VulkanRenderer;
	class GraphicsEngineVulkanImpl;
	class GraphicsEngine
	{
	public:
		static void Create();
		static void Destroy();
		static GraphicsEngine& Get();

		bool Init(const Window& window);

		const Window* GetWindow() const { return m_Window; }

		void Update();

	private:
		void DestroyObjects();

		GraphicsEngine() = default;
		~GraphicsEngine() = default;
		static GraphicsEngine* m_Instance;

		GraphicsEngineVulkanImpl* m_VulkanImpl = nullptr;
		VulkanRenderer* m_VulkanRenderer = nullptr;

		const Window* m_Window = nullptr;
	};

}; // namespace Graphics
