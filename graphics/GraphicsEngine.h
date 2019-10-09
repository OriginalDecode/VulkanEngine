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
	class vkGraphicsDevice;
	class GraphicsEngine
	{
	public:
		GraphicsEngine();
		~GraphicsEngine();

		static void Create();
		static GraphicsEngine& Get();

		bool Init( const Window& window );
		void Present( float dt );

		static vkGraphicsDevice& GetDevice() { return *m_Instance->m_Device; }

		void RegisterCreateConstantBufferFunc( FCreateConstantBuffer fnc ) { m_CreateConstantBufferFnc = fnc; }

	private:
		static std::unique_ptr<GraphicsEngine> m_Instance;
		std::unique_ptr<vkGraphicsDevice> m_Device;

		// std::function < void(ConstantBuffer*)> m

		FCreateConstantBuffer m_CreateConstantBufferFnc;

		void BeginFrame();
	};

}; // namespace Graphics
