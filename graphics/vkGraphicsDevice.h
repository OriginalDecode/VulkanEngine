#pragma once

#include "Core/utilities/utilities.h"
#include "Core/Defines.h"

#include <memory>

class Window;
//struct VkWin32SurfaceCreateInfoKHR;
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

    };

}; // namespace Graphics