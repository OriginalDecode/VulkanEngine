#include "vkGraphicsDevice.h"

#include "vkInstance.h"
#include "vkPhysicalDevice.h"

#include "Utilities.h"
#include "Window.h"

#include <cassert>
#include <windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>


VkDebugReportCallbackEXT _debugCallback;
VkClearValue _clearColor = { 0.f, 0.f, 0.f, 0.f };

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
    VkDebugReportFlagsEXT /* flags */,
    VkDebugReportObjectTypeEXT /* objectType */,
    uint64_t /* object */,
    size_t /* location */,
    int32_t /* messageCode */,
    const char* /* pLayerPrefix */,
    const char* pMessage,
    void* /* pUserData */ )
{
    assert( "warning" );
    OutputDebugStringA( pMessage );
    OutputDebugStringA( "\n" );
    return VK_FALSE;
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
    }

    vkGraphicsDevice::~vkGraphicsDevice()
    {
        vkDestroySwapchainKHR( m_Device, m_Swapchain, nullptr );
        vkDestroySurfaceKHR( m_Instance->get(), m_Backbuffer, nullptr );

        for( auto fb : m_FrameBuffers )
        {
            vkDestroyFramebuffer( m_Device, fb, nullptr );
        }

        for( auto image : m_Images )
        {
            vkDestroyImage( m_Device, image, nullptr );
        }

        for( auto view : m_ImageViews )
        {
            vkDestroyImageView( m_Device, view, nullptr );
        }

		vkDestroySemaphore(m_Device, m_RendererFinished, nullptr);
		vkDestroySemaphore(m_Device, m_ImageAvailable, nullptr);

        vkDestroyCommandPool( m_Device, m_CommandPool, nullptr );

        vkDestroyDevice( m_Device, nullptr );
        //vkDestroyInstance( m_Instance, nullptr );
    }

    bool vkGraphicsDevice::Init( const Window& window )
    {

		m_Instance = std::make_unique<vkInstance>();
		m_Instance->Init();

		m_PhysicalDevice = std::make_unique<vkPhysicalDevice>();
		m_PhysicalDevice->Init(m_Instance.get());

        if( !CreateDeviceAndQueue() )
            return false;

        SetupDebugCallback();

        CreateSwapchain( window );

        m_RenderPass = CreateRenderPass();
		CreateImageViews();
        CreateFramebuffers( window );

		CreateCommandPool();
		CreateCommandBuffers();

		CreateSemaphores();
        return true;
    }

	void vkGraphicsDevice::Present()
	{
		
	
	}

	void vkGraphicsDevice::DrawFrame()
	{
		uint32_t imageIndex;
		vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, nullptr, nullptr, &imageIndex);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphore[] = { m_ImageAvailable };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[imageIndex];

		VkSemaphore signal[] = { m_RendererFinished };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signal;

		VkResult result = vkQueueSubmit(m_Queue, 1, &submitInfo, nullptr);
		assert(!VK_FAILED(result));

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		//renderPassInfo.dependencyCount = 1;
		//renderPassInfo.pDependencies = &dependency;

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signal;

		VkSwapchainKHR swapchains[] = { m_Swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;

		presentInfo.pResults = nullptr;

		vkQueuePresentKHR(m_Queue, &presentInfo);

	}

    

    bool vkGraphicsDevice::CreateDeviceAndQueue()
    {
        uint32_t queueFamilyIndex = 0;
        VkPhysicalDevice physical_device;
        if( !GetPhysicalDevice( &queueFamilyIndex, &physical_device, m_Instance->get() ) )
        {
            assert( !"Failed to get physical device!" );
            return false;
        }

        m_PhysicalDevice = physical_device;

        VkDeviceQueueCreateFlags queue_flags = 0;

        const float queue_priorities[] = { 1.f };

        VkDeviceQueueCreateInfo device_queue_create_info = {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, /* VkStructureType                  sType; */
            nullptr,                                    /* const void*                      pNext; */
            queue_flags,                                /* VkDeviceQueueCreateFlags         flags; */
            queueFamilyIndex,                           /* uint32_t                         queueFamilyIndex; */
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

        VkResult result = vkCreateDevice( m_PhysicalDevice, &device_create_info, nullptr, &m_Device );
        assert( !VK_FAILED( result ) && "Failed to create Vulkan device!" );

        vkGetDeviceQueue( m_Device, queueFamilyIndex, 0, &m_Queue );
        assert( !VK_FAILED( result ) && "Failed to create Vulkan queue!" );

        return true;
    }

    void vkGraphicsDevice::CreateSwapchain( const Window& window )
    {
        VkSurfaceCapabilitiesKHR surface_capabilities;
        VkSurfaceKHR surface = CreateSurface( window.GetHandle() );

        VkBool32 presentSupported = VK_FALSE;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR( m_PhysicalDevice, 0, surface, &presentSupported );
        if( VK_FAILED( result ) )
        {
            assert( presentSupported && "Failed to support present on current device. Bad graphics card?" );
            return;
        }

        result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_PhysicalDevice, surface, &surface_capabilities );
        assert( !VK_FAILED( result ) && "Failed to get surface capabilities!" );

        uint32 present_mode_count = 0;
        result = vkGetPhysicalDeviceSurfacePresentModesKHR( m_PhysicalDevice, surface, &present_mode_count, nullptr );
        assert( !VK_FAILED( result ) );
        std::vector<VkPresentModeKHR> present_modes{ present_mode_count };
        result = vkGetPhysicalDeviceSurfacePresentModesKHR( m_PhysicalDevice, surface, &present_mode_count, present_modes.data() );
        assert( !VK_FAILED( result ) );

        //validate size of swapchain vs window
        m_Extent = surface_capabilities.currentExtent;
        assert( (int32)m_Extent.width == (int32)window.GetInnerSize().m_Width );
        assert( (int32)m_Extent.height == (int32)window.GetInnerSize().m_Height );

        uint32 swapchain_image_count = 2; //backbuffer count, atleast 2;
        assert( swapchain_image_count >= surface_capabilities.minImageCount );

        if( surface_capabilities.maxImageCount != 0 )
            assert( swapchain_image_count < surface_capabilities.maxImageCount );

        // set transform flags
        VkSurfaceTransformFlagBitsKHR surface_transform_flags;
        if( surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR )
            surface_transform_flags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        else
            surface_transform_flags = surface_capabilities.currentTransform;

        // get the nof formats for the surface
        uint32 surface_format_count = 0;
        result = vkGetPhysicalDeviceSurfaceFormatsKHR( m_PhysicalDevice, surface, &surface_format_count, nullptr );
        assert( !VK_FAILED( result ) );

        std::vector<VkSurfaceFormatKHR> surface_formats{ surface_format_count };
        vkGetPhysicalDeviceSurfaceFormatsKHR( m_PhysicalDevice, surface, &surface_format_count, surface_formats.data() );
        assert( !VK_FAILED( result ) );

        // Set the format of the swapchain
        VkFormat swapchain_format;
        if( surface_format_count == 1 && surface_formats.front().format == VK_FORMAT_UNDEFINED )
            swapchain_format = VK_FORMAT_R8G8B8A8_UNORM;
        else
            swapchain_format = surface_formats.front().format;

        // Get the colorspace of the swapchain
        VkColorSpaceKHR swapchainColorspace;
        swapchainColorspace = surface_formats.front().colorSpace;

        VkSwapchainCreateInfoKHR swapchainCreateInfo{
            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, /*VkStructureType                  sType;*/
            nullptr,                                     /*const void*                      pNext;*/
            0,                                           /*VkSwapchainCreateFlagsKHR        flags;*/
            surface,                                     /*VkSurfaceKHR                     surface;*/
            surface_capabilities.minImageCount,          /*uint32_t                         minImageCount;*/
            swapchain_format,                            /*VkFormat                         imageFormat;*/
            swapchainColorspace,                         /*VkColorSpaceKHR                  imageColorSpace;*/
            m_Extent,                                    /*VkExtent2D                       imageExtent;*/
            1,                                           /*uint32_t                         imageArrayLayers;*/
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,         /*VkImageUsageFlags                imageUsage;*/
            VK_SHARING_MODE_EXCLUSIVE,                   /*VkSharingMode                    imageSharingMode;*/
            0,                                           /*uint32_t                         queueFamilyIndexCount;*/
            nullptr,                                     /*const uint32_t*                  pQueueFamilyIndices;*/
            surface_transform_flags,                     /*VkSurfaceTransformFlagBitsKHR    preTransform;*/
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,           /*VkCompositeAlphaFlagBitsKHR      compositeAlpha;*/
            VK_PRESENT_MODE_FIFO_KHR,                    /*VkPresentModeKHR                 presentMode;*/
            VK_TRUE,                                     /*VkBool32                         clipped;*/
            nullptr                                      /*VkSwapchainKHR                   oldSwapchain;*/
        };

        result = vkCreateSwapchainKHR( m_Device, &swapchainCreateInfo, nullptr /*allocator*/, &m_Swapchain );
        assert( !VK_FAILED( result ) );
        m_Format = swapchain_format;
        m_Backbuffer = surface;

        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR( m_Device, m_Swapchain, &imageCount, nullptr );
        m_Images.resize( imageCount );
        vkGetSwapchainImagesKHR( m_Device, m_Swapchain, &imageCount, m_Images.data() );
    }

    void vkGraphicsDevice::SetupDebugCallback()
    {
        auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr( m_Instance->get(), "vkCreateDebugReportCallbackEXT" );

        if( !func )
            return;

        VkDebugReportFlagsEXT flags =
            VK_DEBUG_REPORT_ERROR_BIT_EXT |
            VK_DEBUG_REPORT_WARNING_BIT_EXT |
            VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

        VkDebugReportCallbackCreateInfoEXT callbackCreateInfo{
            VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT, /* VkStructureType                 sType; */
            nullptr,                                                 /* const void*                     pNext; */
            flags,                                                   /* VkDebugReportFlagsEXT           flags; */
            &DebugReportCallback,                                    /* PFN_vkDebugReportCallbackEXT    pfnCallback; */
            nullptr                                                  /* void*                           pUserData;   */
        };

        VkResult result = func( m_Instance->get(), &callbackCreateInfo, nullptr /*allocator*/, &_debugCallback );
        assert( !VK_FAILED( result ) );
    }

    VkSurfaceKHR vkGraphicsDevice::CreateSurface( HWindow windowHandle )
    {
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.hwnd = windowHandle;
        surfaceCreateInfo.hinstance = ::GetModuleHandle( nullptr );

        VkSurfaceKHR surface = nullptr;
        VkResult result = vkCreateWin32SurfaceKHR( m_Instance->get(), &surfaceCreateInfo, nullptr, &surface );
        assert( !VK_FAILED( result ) );

        return surface;
    }

    void vkGraphicsDevice::CreateFramebuffers( const Window& window )
    {
        m_FrameBuffers.resize( m_ImageViews.size() );
        for( size_t i = 0; i < m_ImageViews.size(); ++i )
        {
            VkImageView attatchment[] = {
                m_ImageViews[i]
            };

            VkFramebufferCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            createInfo.renderPass = m_RenderPass;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = attatchment;
            createInfo.width = (uint32_t)window.GetInnerSize().m_Width;
            createInfo.height = (uint32_t)window.GetInnerSize().m_Height;
            createInfo.layers = 1;

            VkResult result = vkCreateFramebuffer( m_Device, &createInfo, nullptr /*allocator*/, &m_FrameBuffers[i] );
            if( VK_FAILED( result ) )
            {
                assert( !"Failed to create framebuffer!" );
                return;
            }
        }
    }

    VkRenderPass vkGraphicsDevice::CreateRenderPass()
    {
        VkAttachmentDescription attachmentDescription = {};
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.format = static_cast<VkFormat>( m_Format );
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        VkAttachmentReference attachmentReference = {};
        attachmentReference.attachment = 0;
        attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.inputAttachmentCount = 0;
        subpassDescription.pColorAttachments = &attachmentReference;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        renderPassCreateInfo.pAttachments = &attachmentDescription;

        VkRenderPass render_pass = nullptr;
        vkCreateRenderPass( m_Device, &renderPassCreateInfo, nullptr, &render_pass );

        return render_pass;
    }

    void vkGraphicsDevice::CreateImageViews()
    {
        m_ImageViews.resize( m_Images.size() );

        for( size_t i = 0; i < m_ImageViews.size(); ++i )
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_Images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = static_cast<VkFormat>( m_Format );

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            VkImageSubresourceRange& resource = createInfo.subresourceRange;
            resource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            resource.baseMipLevel = 0;
            resource.levelCount = 1;
            resource.baseArrayLayer = 0;
            resource.layerCount = 1;

            VkResult result = vkCreateImageView( m_Device, &createInfo, nullptr /*allocator*/, &m_ImageViews[i] );
            assert( !VK_FAILED( result ) && "Failed to create image view" );
        }
    }

    void vkGraphicsDevice::CreateCommandPool()
    {
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = 0;
        createInfo.flags = 0;

        VkResult result = vkCreateCommandPool( m_Device, &createInfo, nullptr, &m_CommandPool );
        assert( !VK_FAILED( result ) && "Failed to create command pool" );
    }

    void vkGraphicsDevice::CreateCommandBuffers()
    {
        m_CommandBuffers.resize( m_FrameBuffers.size() );

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

        VkResult result = vkAllocateCommandBuffers( m_Device, &allocInfo, m_CommandBuffers.data() );
        assert( !VK_FAILED( result ) && "Failed to allocated command buffers!" );

		for (size_t i = 0; i < m_CommandBuffers.size(); ++i)
		{
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			begin_info.pInheritanceInfo = nullptr;

			result = vkBeginCommandBuffer(m_CommandBuffers[i], &begin_info);
			assert(!VK_FAILED(result) && "Failed to being command buffer!");

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_RenderPass;
			renderPassInfo.framebuffer = m_FrameBuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = m_Extent;

			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &_clearColor;

			vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			//vkCmdBindPipeline
			//vkCmdDraw
			vkCmdEndRenderPass(m_CommandBuffers[i]);
		}


    }

    void vkGraphicsDevice::CreatePipeline()
    {
        // https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Shader_modules
        // https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
    }

	void vkGraphicsDevice::CreateSemaphores()
	{
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkResult result = vkCreateSemaphore(m_Device, &info, nullptr/*allocator*/, &m_ImageAvailable);
		assert(!VK_FAILED(result) && "Failed to create imageAvailable semaphore");

		result = vkCreateSemaphore(m_Device, &info, nullptr/*allocator*/, &m_RendererFinished);
		assert(!VK_FAILED(result) && "Failed to create rendererFinished semaphore");

	}

}; //namespace Graphics