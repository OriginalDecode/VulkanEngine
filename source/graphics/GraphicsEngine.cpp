#include "GraphicsEngine.h"
#include "Window.h"
#include "vulkan/VulkanRenderer.h"
#include "thirdparty/imgui/imgui.h"

#include <cassert>

namespace Graphics
{
	void CreateImGuiContext()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}

	GraphicsEngine* GraphicsEngine::m_Instance = nullptr;

	void GraphicsEngine::Create() { m_Instance = new GraphicsEngine; }

	void GraphicsEngine::Destroy()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}

	GraphicsEngine& GraphicsEngine::Get() { return *m_Instance; }

	bool GraphicsEngine::Init(const Window& window)
	{
		m_Window = &window;

		return true;
	}

}; // namespace Graphics
