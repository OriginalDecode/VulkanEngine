#include "VlkPhysicalDevice.h"

#include "VlkInstance.h"
#include "VlkSurface.h"

#include <cassert>
#include <vector>

namespace Graphics
{

	VlkPhysicalDevice::VlkPhysicalDevice() = default;
	VlkPhysicalDevice::~VlkPhysicalDevice() = default;

	void VlkPhysicalDevice::Init( VlkInstance* instance )
	{
		std::vector<VkPhysicalDevice> devices;
		instance->GetPhysicalDevices(devices);

		for( auto device : devices )
		{
			uint32 property_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties( device, &property_count, nullptr );

			m_QueueProperties.resize(property_count);
			vkGetPhysicalDeviceQueueFamilyProperties( device, &property_count, m_QueueProperties.data());

			QueueProperties queueProp = FindFamilyIndices( nullptr );
			if (queueProp.queueIndex > -1)
			{
				m_PhysicalDevice = device;
				break;
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

	VkSurfaceCapabilitiesKHR VlkPhysicalDevice::GetSurfaceCapabilities( VkSurfaceKHR pSurface ) const
	{
		VkSurfaceCapabilitiesKHR capabilities = {};
		VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_PhysicalDevice, pSurface, &capabilities );
		assert( result == VK_SUCCESS );
		return capabilities;
	}

	bool VlkPhysicalDevice::SurfaceCanPresent( VkSurfaceKHR pSurface ) const
	{
		VkBool32 present_supported = VK_FALSE;
		VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR( m_PhysicalDevice, m_QueueFamilyIndex, pSurface, &present_supported );
		assert( result == VK_SUCCESS && "Failed to get present_supported!" );
		return present_supported != VK_FALSE;
	}

	void VlkPhysicalDevice::GetSurfaceInfo( VkSurfaceKHR pSurface, bool* canPresent, 
											uint32* formatCount, std::vector<VkSurfaceFormatKHR>* formats, 
											uint32* presentCount, std::vector<VkPresentModeKHR>* presentModes, 
											VkSurfaceCapabilitiesKHR* capabilities )
	{

		if(canPresent)
			*canPresent = SurfaceCanPresent( pSurface );

		if (formatCount)
		{
			*formatCount = GetSurfaceFormatCount( pSurface );
			if (formats)
			{
				formats->resize(*formatCount);
				GetSurfaceFormats( pSurface, *formatCount, formats->data() );
			}
		}

		if (presentCount)
		{
			*presentCount = GetSurfacePresentModeCount( pSurface );
			if (presentModes)
			{
				presentModes->resize(*presentCount);
				GetSurfacePresentModes( pSurface, *presentCount, presentModes->data() );
			}
		}

		if(capabilities)
			*capabilities = GetSurfaceCapabilities( pSurface );

	}

	QueueProperties VlkPhysicalDevice::FindFamilyIndices( VlkSurface* pSurface )
	{
		QueueProperties properties = { };

		for( size_t i = 0; i < m_QueueProperties.size(); ++i )
		{
			const VkQueueFamilyProperties& property = m_QueueProperties[i];
			if (pSurface && property.queueCount > 0 && pSurface->CanPresent())
			{
				properties.familyIndex = (int32)i;
			}
		
			if( property.queueCount > 0 && property.queueFlags & VK_QUEUE_GRAPHICS_BIT )
			{
				properties.queueIndex = (int32)i;
				break;
			}
		}
		return properties;
	}

	uint32 VlkPhysicalDevice::GetSurfacePresentModeCount( VkSurfaceKHR pSurface ) const
	{
		uint32 presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR( m_PhysicalDevice, pSurface, &presentModeCount, nullptr );
		return presentModeCount;
	}

	void VlkPhysicalDevice::GetSurfacePresentModes( VkSurfaceKHR pSurface, uint32 presentModeCount, VkPresentModeKHR* presentModes ) const
	{
		vkGetPhysicalDeviceSurfacePresentModesKHR( m_PhysicalDevice, pSurface, &presentModeCount, presentModes );
	}

	void VlkPhysicalDevice::GetSurfaceFormats( VkSurfaceKHR pSurface, uint32 formatCount, VkSurfaceFormatKHR* formats ) const
	{
		vkGetPhysicalDeviceSurfaceFormatsKHR( m_PhysicalDevice, pSurface, &formatCount, formats );
	}

	uint32 VlkPhysicalDevice::GetSurfaceFormatCount( VkSurfaceKHR pSurface ) const
	{
		uint32 formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR( m_PhysicalDevice, pSurface, &formatCount, nullptr );
		return formatCount;
	}

}; //namespace Graphics