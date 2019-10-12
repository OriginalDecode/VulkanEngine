#pragma once

#include "GraphicsDevice.h"
#include <memory>
#include <functional>

namespace Graphics
{
	class ConstantBuffer;
}
//typedef void ( *FCreateConstantBuffer )( Graphics::ConstantBuffer* );
typedef std::function<void( Graphics::ConstantBuffer* )> FCreateConstantBuffer;

class Window;
namespace Graphics
{
	void CreateImGuiContext();
	class VulkanRenderer;
	class GraphicsEngine
	{
	public:
		GraphicsEngine();
		~GraphicsEngine();

		static void Create();
		static GraphicsEngine& Get();

		bool Init( const Window& window );
		void Present( float dt );

		static VulkanRenderer& GetDevice() { return *m_Instance->m_Device; }

		void RegisterCreateConstantBufferFunc( FCreateConstantBuffer fnc ) { m_CreateConstantBufferFnc = fnc; }

		const Window* GetWindow() const { return m_Window; }

	private:
		static std::unique_ptr<GraphicsEngine> m_Instance;
		std::unique_ptr<VulkanRenderer> m_Device;

		const Window* m_Window{nullptr};

		// std::function < void(ConstantBuffer*)> m

		FCreateConstantBuffer m_CreateConstantBufferFnc;

		void BeginFrame();
	};

}; // namespace Graphics
