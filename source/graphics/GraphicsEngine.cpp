#include "GraphicsEngine.h"
#include "Window.h"
#include "vulkan/VulkanRenderer.h"
#include "graphics/GraphicsEngineVulkanImpl.h"
#include "thirdparty/imgui/imgui.h"
#include "logger/Assert.h"

namespace Graphics
{
	void CreateImGuiContext()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}

	GraphicsEngine* GraphicsEngine::m_Instance = nullptr;

	void GraphicsEngine::Create()
	{
		if(!m_Instance)
		{
			m_Instance = new GraphicsEngine;
		}
		else
			ASSERT(false, "GraphicsEngine already exists");
	}

	void GraphicsEngine::Destroy()
	{
		m_Instance->DestroyObjects();
		delete m_Instance;
		m_Instance = nullptr;
	}

	GraphicsEngine& GraphicsEngine::Get() { return *m_Instance; }

	bool GraphicsEngine::Init(const Window& window)
	{
		m_Window = &window;

		m_VulkanImpl = new GraphicsEngineVulkanImpl;
		m_VulkanImpl->Init(window);

		m_VulkanRenderer = new VulkanRenderer;

		m_VulkanRenderer->Init(m_VulkanImpl->GetRenderContext());

		return true;
	}

	void GraphicsEngine::DestroyObjects()
	{
		delete m_VulkanRenderer;
		m_VulkanRenderer = nullptr;

		m_VulkanImpl->Destroy();
		delete m_VulkanImpl;
		m_VulkanImpl = nullptr;
	}

	void GraphicsEngine::Update() { m_VulkanRenderer->DrawFrame(0.f); }

}; // namespace Graphics
