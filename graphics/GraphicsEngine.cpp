#include "GraphicsEngine.h"

#include <cassert>
#include <d3d11.h>

#include "Window.h"



constexpr float BLACK[4] = { 0.f, 0.f, 0.f, 0.f };

namespace Graphics
{
	GraphicsEngine* GraphicsEngine::s_Instance = nullptr;

	GraphicsEngine::~GraphicsEngine()
	{
		m_DefaultRenderTarget->Release();
		m_DefaultDepthTarget->Release();
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

	Graphics::GraphicsEngine* GraphicsEngine::Get()
	{
		return s_Instance;
	}

	void GraphicsEngine::Init(const Window& window)
	{
		m_Device.CreateDevice(window, &m_DefaultRenderTarget, &m_DefaultDepthTarget);
		ID3D11DeviceContext* context = Graphics::GetContext();
		context->OMSetRenderTargets(1, &m_DefaultRenderTarget, m_DefaultDepthTarget);
		context->Release();
	}

	void GraphicsEngine::Present()
	{
		HRESULT hr = Graphics::Present(1, 0);
		switch (hr)
		{
			case DXGI_ERROR_DEVICE_HUNG:
				OutputDebugString("device hung\n");
				break;

			case DXGI_ERROR_DEVICE_REMOVED:
				OutputDebugString("removed\n");
				break;

			case DXGI_ERROR_DEVICE_RESET:
				OutputDebugString("reset\n");
				break;

			case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
				OutputDebugString("internal_error\n");
				break;

			case DXGI_ERROR_INVALID_CALL:
				OutputDebugString("invalid_call\n");
				break;
		};

		BeginFrame();

	}


	GraphicsDevice& GraphicsEngine::GetDevice()
	{
		return s_Instance->m_Device;
	}

	void GraphicsEngine::BeginFrame()
	{
		ID3D11DeviceContext* context = Graphics::GetContext();
		context->ClearRenderTargetView(m_DefaultRenderTarget, BLACK);
		context->ClearDepthStencilView(m_DefaultDepthTarget, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		context->Release();
	}

	void Release(ID3D11Buffer* buffer)
	{
		buffer->Release();
	}


}; // namespace Graphics