#include "vkDevice.h"
#include "vkPhysicalDevice.h"
namespace Graphics
{
    const char* debugLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
    const char* deviceExt[] = { "VK_KHR_swapchain" };

    vkDevice::~vkDevice()
    {
        Release();
    }

    void vkDevice::Release()
    {
    }

    void vkDevice::Init( const vkPhysicalDevice& physicalDevice )
    {

        //queue create info
        const float queue_priorities[] = { 1.f };
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = physicalDevice.GetQueueFamilyIndex();
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = queue_priorities;

        //Physical device features
        VkPhysicalDeviceFeatures enabled_features = {};
        enabled_features.shaderClipDistance = true;

        // device create info
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pQueueCreateInfos = &device_queue_create_info;
#ifdef _DEBUG
        createInfo.enabledLayerCount = ARRSIZE( debugLayers );
        createInfo.ppEnabledLayerNames = debugLayers;
#endif
        createInfo.enabledExtensionCount = ARRSIZE( deviceExt );
        createInfo.ppEnabledExtensionNames = deviceExt;
        createInfo.pEnabledFeatures = &enabled_features;

        VkResult result = vkCreateDevice( m_PhysicalDevice, &device_create_info, nullptr, &m_Device );
        assert( result == VK_SUCCESS && "Failed to create Vulkan device!" );

        vkGetDeviceQueue( m_Device, queueFamilyIndex, 0, &m_Queue );
        assert( result == VK_SUCCESS && "Failed to create Vulkan queue!" );
    }
}; //namespace Graphics