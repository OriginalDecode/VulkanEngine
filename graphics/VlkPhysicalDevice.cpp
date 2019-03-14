#include "VlkPhysicalDevice.h"

#include "VlkInstance.h"
#include "VlkSurface.h"

#include <cassert>
#include <vector>

namespace Graphics
{
	VlkPhysicalDevice::~VlkPhysicalDevice()
	{
	}

	void VlkPhysicalDevice::Init( const VlkInstance& instance )
	{
		uint32 device_count = 0;
		VkResult result = vkEnumeratePhysicalDevices( instance.get(), &device_count, nullptr );
		assert( result == VK_SUCCESS && "Failed to enumerate device!" );

		std::vector<VkPhysicalDevice> devices{ device_count };
		result = vkEnumeratePhysicalDevices( instance.get(), &device_count, devices.data() );
		assert( result == VK_SUCCESS && "Failed to enumerate device!" );

		for( auto device : devices )
		{
			uint32 property_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties( device, &property_count, nullptr );

			m_QueueProperties.reserve( property_count );
			vkGetPhysicalDeviceQueueFamilyProperties( device, &property_count, m_QueueProperties.data() );

			for( size_t i = 0; i < m_QueueProperties.size(); ++i )
			{
				const VkQueueFamilyProperties& property = m_QueueProperties[i];
				if( property.queueFlags & VK_QUEUE_GRAPHICS_BIT )
				{
					if( !m_PhysicalDevice )
						m_PhysicalDevice = device;

					m_QueueFamilyIndex = (uint32)i;

					break;
				}
			}
		}

		assert( m_PhysicalDevice );
	}

	VkDevice VlkPhysicalDevice::CreateDevice( const VkDeviceCreateInfo& createInfo ) const
	{
		VkDevice device = nullptr;
		VkResult result = vkCreateDevice( m_PhysicalDevice, &createInfo, nullptr, &device );
		assert( result == VK_SUCCESS && "Failed to create device!" );
		assert( device );
		return device;
	}

	bool VlkPhysicalDevice::SurfaceCanPresent( VkSurfaceKHR pSurface ) const
	{
		VkBool32 present_supported = VK_FALSE;
		VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR( m_PhysicalDevice, m_QueueFamilyIndex, pSurface, &present_supported );
		if( result != VK_SUCCESS )
		{
			assert( !"Surface does not support presenting!" );
			return false;
		}
		return true;
	}

	VkSurfaceCapabilitiesKHR VlkPhysicalDevice::GetSurfaceCapabilities( const VlkSurface& surface ) const
	{
		return surface.GetCapabilities( m_PhysicalDevice );
	}

	uint32 VlkPhysicalDevice::GetSurfacePresentModeCount( const VlkSurface& surface ) const
	{
		return surface.GetPresentModeCount( m_PhysicalDevice );
	}

	void VlkPhysicalDevice::GetSurfacePresentModes( const VlkSurface& surface, uint32 modeCount, VkPresentModeKHR* presentModes ) const
	{
		surface.GetPresentModes( m_PhysicalDevice, modeCount, presentModes );
	}

}; //namespace Graphics