#include "vkGraphicsDevice.h"

#include <vulkan/vulkan.h>
#include <cassert>
VkInstance pVulkanInstance = nullptr;
namespace Graphics
{
    vkGraphicsDevice::vkGraphicsDevice()
    {

        /*
        VkStructureType    sType;
        const void*        pNext;
        const char*        pApplicationName;
        uint32_t           applicationVersion;
        const char*        pEngineName;
        uint32_t           engineVersion;
        uint32_t           apiVersion;
        */
        VkApplicationInfo appInfo = {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            "Hello World!",
            VK_MAKE_VERSION(1,0,0),
            "Engine",
            VK_MAKE_VERSION(1,0,0),
            VK_API_VERSION_1_1    
        };


        /*
        VkStructureType             sType;
        const void*                 pNext;
        VkInstanceCreateFlags       flags;
        const VkApplicationInfo*    pApplicationInfo;
        uint32_t                    enabledLayerCount;
        const char* const*          ppEnabledLayerNames;
        uint32_t                    enabledExtensionCount;
        const char* const*          ppEnabledExtensionNames;
        */
        VkInstanceCreateInfo createInfo {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr,
            0, 
            &appInfo,
            0,
            nullptr,
            0,
            nullptr
        };

        VkResult result = vkCreateInstance(&createInfo, nullptr /*allocator*/, &pVulkanInstance);
        assert(result == VK_SUCCESS && "Failed to create vulkan instance!");
    }

    vkGraphicsDevice::~vkGraphicsDevice()
    {
        //
    }

}; //namespace Graphics