#include "vkGraphicsDevice.h"

#include <cassert>
#include <vulkan/vulkan.h>

#include "Utilities.h"

#include <vector>

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
    VkDebugReportFlagsEXT /* flags */,
    VkDebugReportObjectTypeEXT /* objectType */,
    uint64_t /* object */,
    size_t /* location */,
    int32_t /* messageCode */,
    const char* /* pLayerPrefix */,
    const char* /* pMessage */,
    void* /* pUserData */ )
{
    // OutputDebugStringA(pMessage);
    // OutputDebugStringA("\n");
    return VK_FALSE;
}

std::vector<const char*> GetDebugInstanceNames()
{
    uint32 layer_count = 0;
    vkEnumerateInstanceLayerProperties( &layer_count, nullptr );
    std::vector<VkLayerProperties> instance_layers{ layer_count };

    vkEnumerateInstanceLayerProperties( &layer_count, instance_layers.data() );

    std::vector<const char*> result;
    for( const auto& layer : instance_layers )
    {
        if( strcmp( layer.layerName, "VK_LAYER_LUNARG_standard_validation" ) == 0 )
            result.push_back( "VK_LAYER_LUNARG_standard_validation" );
    }

    return result;
}

std::vector<const char*> GetDebugInstanceExtNames()
{
    uint32 extension_count = 0;
    vkEnumerateInstanceExtensionProperties( nullptr, &extension_count, nullptr );
    std::vector<VkExtensionProperties> instance_extensions{ extension_count };

    vkEnumerateInstanceExtensionProperties( nullptr, &extension_count, instance_extensions.data() );

    std::vector<const char*> result{ extension_count };
    for( const auto& ext : instance_extensions )
    {
        if( strcmp( ext.extensionName, "VK_EXT_debug_report" ) == 0 )
            result.push_back( "VK_EXT_debug_report" );
    }

    return result;
}

std::vector<const char*> GetDebugDeviceLayerNames( VkPhysicalDevice device )
{
    uint32 layer_count = 0;
    vkEnumerateDeviceLayerProperties( device, &layer_count, nullptr );

    std::vector<VkLayerProperties> device_layers{ layer_count };
    vkEnumerateDeviceLayerProperties( device, &layer_count, device_layers.data() );

    std::vector<const char*> result;
    for( const auto& p : device_layers )
    {
        if( strcmp( p.layerName, "VK_LAYER_LUNARG_standard_validation" ) == 0 )
            result.push_back( "VK_LAYER_LUNARG_standard_validation" );
    }

    return result;
}

namespace Graphics
{
    vkGraphicsDevice::vkGraphicsDevice()
    {

        CreateInstance();
        CreateDevice();
    }

    vkGraphicsDevice::~vkGraphicsDevice()
    {
        //
    }

    void vkGraphicsDevice::CreateInstance()
    {
        VkApplicationInfo app_info = {
            VK_STRUCTURE_TYPE_APPLICATION_INFO, /* VkStructureType              sType */
            nullptr,                            /* const void*                  pNext */
            "Hello World!",                     /* const char*                  pApplicationName */
            VK_MAKE_VERSION( 1, 0, 0 ),         /* uint32_t                     applicationVersion */
            "Engine",                           /* const char*                  pEngineName */
            VK_MAKE_VERSION( 1, 0, 0 ),         /* uint32_t                     engineVersion */
            VK_API_VERSION_1_1                  /* uint32_t                     apiVersion */
        };

        std::vector<const char*> instance_extensions = {
            "VK_KHR_surface",
            "VK_KHR_win32_surface"
        };

        std::vector<const char*> instance_layers;
#ifdef _DEBUG
        std::vector<const char*> debug_instance_layers = GetDebugInstanceNames();
        instance_layers.insert( instance_layers.end(), debug_instance_layers.begin(), debug_instance_layers.end() );
        // instance_layers.swap(debug_instance_layers);
#endif

        VkInstanceCreateFlags create_flags = 0;
        VkInstanceCreateInfo create_info{
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, /* VkStructureType             sType; */
            nullptr,                                /* const void*                 pNext; */
            create_flags,                           /* VkInstanceCreateFlags       flags; */
            &app_info,                              /* const VkApplicationInfo*    pApplicationInfo; */
            (uint32_t)instance_layers.size(),       /* uint32_t                    enabledLayerCount; */
            instance_layers.data(),                 /* const char* const*          ppEnabledLayerNames; */
            (uint32_t)instance_extensions.size(),   /* uint32_t                    enabledExtensionCount; */
            instance_extensions.data()              /* const char* const*          ppEnabledExtensionNames; */
        };

        VkResult result = vkCreateInstance( &create_info, nullptr /*allocator*/, &m_Instance );
        assert( result == VK_SUCCESS && "Failed to create vulkan instance!" );
    }

    void vkGraphicsDevice::CreateDevice()
    {
        uint32 device_count = 0;
        VkResult result = vkEnumeratePhysicalDevices( m_Instance, &device_count, nullptr );
        assert( result == VK_SUCCESS && "Failed to enumerate devices" );

        std::vector<VkPhysicalDevice> devices{ device_count };
        result = vkEnumeratePhysicalDevices( m_Instance, &device_count, devices.data() );
        assert( result == VK_SUCCESS && "Failed to enumerate devices" );

        /*
            * Find a physical device with a graphics queue
        */
        uint32_t output_graphics_device_index = 0;
        for( auto device : devices )
        {
            uint32 property_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties( device, &property_count, nullptr );

            std::vector<VkQueueFamilyProperties> queue_properties{ property_count };
            vkGetPhysicalDeviceQueueFamilyProperties( device, &property_count, queue_properties.data() );

            for( size_t i = 0; i < queue_properties.size(); ++i )
            {
                const VkQueueFamilyProperties& property = queue_properties[i];
                if( property.queueFlags & VK_QUEUE_GRAPHICS_BIT )
                {
                    if( !m_PhysicalDevice )
                        m_PhysicalDevice = device;

                    output_graphics_device_index = (uint32_t)i;
                    return;
                }
            }
        }

        assert( m_PhysicalDevice );
        VkDeviceQueueCreateFlags queue_flags = 0;

        const float queue_priorities[] = { 1.f };

        VkDeviceQueueCreateInfo device_queue_create_info = {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, /* VkStructureType                  sType; */
            nullptr,                                    /* const void*                      pNext; */
            queue_flags,                                /* VkDeviceQueueCreateFlags         flags; */
            output_graphics_device_index,               /* uint32_t                         queueFamilyIndex; */
            1,                                          /* uint32_t                         queueCount; */
            queue_priorities                            /* const float*                     pQueuePriorities;  */
        };

        std::vector<const char*> device_layers;
#ifdef _DEBUG
        std::vector<const char*> device_debug_names = GetDebugDeviceLayerNames( m_PhysicalDevice );
        device_layers.insert( device_layers.end(), device_debug_names.begin(), device_debug_names.end() );
#endif

        VkPhysicalDeviceFeatures enabled_features;
        memset( &enabled_features, 0, sizeof( enabled_features ) );
        enabled_features.shaderClipDistance = true;

        std::vector<const char*> device_extensions = {
            "VK_KHR_swapchain"
        };

        VkDeviceCreateFlags device_create_flags = 0;
        VkDeviceCreateInfo device_create_info = {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, /* VkStructureType                    sType; */
            nullptr,                              /* const void*                        pNext; */
            device_create_flags,                  /* VkDeviceCreateFlags                flags; */
            1,                                    /* uint32_t                           queueCreateInfoCount; */
            &device_queue_create_info,            /* const VkDeviceQueueCreateInfo*     pQueueCreateInfos; */
            (uint32_t)device_layers.size(),       /* uint32_t                           enabledLayerCount; */
            device_layers.data(),                 /* const char* const*                 ppEnabledLayerNames; */
            (uint32_t)device_extensions.size(),   /* uint32_t                           enabledExtensionCount; */
            device_extensions.data(),             /* const char* const*                 ppEnabledExtensionNames; */
            &enabled_features                     /* const VkPhysicalDeviceFeatures*    pEnabledFeatures;         */
        };

        result = vkCreateDevice( m_PhysicalDevice, &device_create_info, nullptr, &m_Device );
        assert( result != VK_SUCCESS && "Failed to create vulkan device!" );

        vkGetDeviceQueue( m_Device, output_graphics_device_index, 0, &m_Queue );
        assert( result != VK_SUCCESS && "Failed to create vulkan queue!" );
    }

    void vkGraphicsDevice::CreateSwapchain()
    {
        
    }



}; //namespace Graphics