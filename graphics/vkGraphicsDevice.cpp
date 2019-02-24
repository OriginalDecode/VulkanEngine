#include "vkGraphicsDevice.h"

#include <vulkan/vulkan.h>
#include <cassert>

#include "Utilities.h"

#include <vector>

VkInstance pVulkanInstance = nullptr;

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
    VkDebugReportFlagsEXT       /*flags*/,
    VkDebugReportObjectTypeEXT  /*objectType*/,
    uint64_t                    /*object*/,
    size_t                      /*location*/,
    int32_t                     /*messageCode*/,
    const char*                 /*pLayerPrefix*/,
    const char*                 /*pMessage*/,
    void*                       /*pUserData*/)
{
    // OutputDebugStringA(pMessage);
    // OutputDebugStringA("\n");
    return VK_FALSE;
}

std::vector<const char*> GetDebugInstanceNames()
{
    uint32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> instance_layers;
    instance_layers.reserve(layer_count);

    vkEnumerateInstanceLayerProperties(&layer_count, instance_layers.data());
    
    std::vector<const char*> result;
    for(const auto& layer : instance_layers)
    {
        if( strcmp(layer.layerName, "VK_LAYER_LUNARG_standard_validation") == 0)
            result.push_back("VK_LAYER_LUNARG_standard_validation");
    }

    return result;
}

std::vector<const char*> GetDebugInstanceExtNames()
{
    
    uint32 extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> instance_extensions;
    instance_extensions.reserve(extension_count);

    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, instance_extensions.data());

    std::vector<const char*> result;
    result.reserve(extension_count);

    for(const auto& ext : instance_extensions)
    {
        if( strcmp( ext.extensionName, "VK_EXT_debug_report") == 0 )
            result.push_back("VK_EXT_debug_report");
    }

    return result;
}



namespace Graphics
{
    vkGraphicsDevice::vkGraphicsDevice()
    {
        VkApplicationInfo app_info = {
            VK_STRUCTURE_TYPE_APPLICATION_INFO, /* VkStructureType              sType */
            nullptr,                            /* const void*                  pNext */
            "Hello World!",                     /* const char*                  pApplicationName */
            VK_MAKE_VERSION(1,0,0),             /* uint32_t                     applicationVersion */
            "Engine",                           /* const char*                  pEngineName */
            VK_MAKE_VERSION(1,0,0),             /* uint32_t                     engineVersion */
            VK_API_VERSION_1_1                  /* uint32_t                     apiVersion */
        };
        
        std::vector<const char*> instance_extensions = {
             "VK_KHR_surface", 
             "VK_KHR_win32_surface"
        };

        std::vector<const char*> instance_layers;
#ifdef _DEBUG
        std::vector<const char*> debug_instance_layers = GetDebugInstanceNames();
        instance_layers.insert(instance_layers.end(), debug_instance_layers.begin(), debug_instance_layers.end());
        // instance_layers.swap(debug_instance_layers);
#endif

        VkInstanceCreateFlags create_flags = 0;
        VkInstanceCreateInfo create_info {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, /* VkStructureType             sType; */
            nullptr,                                /* const void*                 pNext; */
            create_flags,                           /* VkInstanceCreateFlags       flags; */
            &app_info,                              /* const VkApplicationInfo*    pApplicationInfo; */
            (uint32_t)instance_layers.size(),       /* uint32_t                    enabledLayerCount; */
            instance_layers.data(),                 /* const char* const*          ppEnabledLayerNames; */
            (uint32_t)instance_extensions.size(),   /* uint32_t                    enabledExtensionCount; */
            instance_extensions.data()              /* const char* const*          ppEnabledExtensionNames; */
        };

        VkResult result = vkCreateInstance(&create_info, nullptr /*allocator*/, &pVulkanInstance);
        assert(result == VK_SUCCESS && "Failed to create vulkan instance!");
    }

    vkGraphicsDevice::~vkGraphicsDevice()
    {
        //
    }

    void vkGraphicsDevice::CreateDevice()
    {
        uint32 device_count = 0;
        vkEnumeratePhysicalDevices(nullptr/*instance*/, &device_count, nullptr);


    }


}; //namespace Graphics