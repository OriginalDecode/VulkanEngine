#include "GraphicsEngine.h"
#include "Window.h"

#include <cassert>

#ifdef _WIN32
#include <d3d11.h>
#include <Windows.h>
#endif

#include "vkGraphicsDevice.h"

constexpr float BLACK[4] = { 0.f, 0.f, 0.f, 0.f };

namespace Graphics
{
	GraphicsEngine* GraphicsEngine::s_Instance = nullptr;
	GraphicsEngine::~GraphicsEngine() 
	{
		delete m_Device;
		m_Device = nullptr;
	}

	void GraphicsEngine::Create()
	{
		if (s_Instance)
		{
			assert(!"GraphicsEngine::Create() called twice or more");
			return;
		}

		s_Instance = new GraphicsEngine;
	}

	void GraphicsEngine::Destroy()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}

	GraphicsEngine* GraphicsEngine::Get()
	{
		return s_Instance;
	}

	void GraphicsEngine::Init(const Window& /**/)
	{
		m_Device = new vkGraphicsDevice;
	}

	void GraphicsEngine::Present()
	{
		BeginFrame();

	}

	void GraphicsEngine::BeginFrame()
	{
	}


}; // namespace Graphics