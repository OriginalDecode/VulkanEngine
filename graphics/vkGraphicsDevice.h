#pragma once

#include "Core/utilities/utilities.h"
#include "Core/Defines.h"

#include <memory>
#include <vector>
class Window;
//struct VkWin32SurfaceCreateInfoKHR;

LPE_DEFINE_HANDLE(VkCommandPool);
LPE_DEFINE_HANDLE(VkCommandBuffer);

namespace Graphics
{
    class VlkInstance;
    class VlkPhysicalDevice;
    class VlkDevice;
	class VlkSwapchain;
    class vkGraphicsDevice
    {
    public:
        vkGraphicsDevice();
        ~vkGraphicsDevice();

        bool Init( const Window& window );

        void Present();
        void DrawFrame();


		VlkInstance& GetVlkInstance() { return *m_Instance; }
		VlkDevice& GetVlkDevice() { return *m_LogicalDevice; }

    private:
  
        std::unique_ptr<VlkInstance> m_Instance;
        std::unique_ptr<VlkPhysicalDevice> m_PhysicalDevice;
        std::unique_ptr<VlkDevice> m_LogicalDevice;
        std::unique_ptr<VlkSwapchain> m_Swapchain;

		std::vector<VkCommandBuffer> m_CmdBuffers;
		VkCommandPool m_CmdPool = nullptr;

		uint32 m_Index = 0;

    };

}; // namespace Graphics