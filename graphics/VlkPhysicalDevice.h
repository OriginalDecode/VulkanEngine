#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

#include "IGfxObject.h"

#include <vector>
#include <vulkan/vulkan_core.h>

LPE_DEFINE_HANDLE( VkPhysicalDevice );
LPE_DEFINE_HANDLE( VkDevice );

struct VkDeviceCreateInfo;
namespace Graphics
{
	class VlkInstance;
	class VlkSurface;

	struct QueueProperties
	{
		uint32 familyIndex = 0;
		uint32 presentFamily = 0;
	};


	class VlkPhysicalDevice
	{
	public:
		VlkPhysicalDevice();
		~VlkPhysicalDevice();

		void Init( VlkInstance* instance );

		uint32 GetQueueFamilyIndex() const { return m_QueueFamilyIndex; }


		VkDevice CreateDevice( const VkDeviceCreateInfo& createInfo ) const;


		void GetSurfaceInfo(VkSurfaceKHR pSurface, bool* canPresent, 
							uint32* formatCount, std::vector<VkSurfaceFormatKHR>* formats, 
							uint32* presentCount, std::vector<VkPresentModeKHR>* presentModes, 
							VkSurfaceCapabilitiesKHR* capabilities );

	private:
		bool SurfaceCanPresent( VkSurfaceKHR pSurface ) const;
		uint32 GetSurfacePresentModeCount( VkSurfaceKHR pSurface ) const;
		void GetSurfacePresentModes( VkSurfaceKHR pSurface, uint32 presentModeCount, VkPresentModeKHR* presentModes ) const;
		void GetSurfaceFormats( VkSurfaceKHR pSurface, uint32 formatCount, VkSurfaceFormatKHR* formats ) const;
		uint32 GetSurfaceFormatCount( VkSurfaceKHR pSurface ) const;
		VkSurfaceCapabilitiesKHR GetSurfaceCapabilities( VkSurfaceKHR pSurface ) const;





		VkPhysicalDevice m_PhysicalDevice = nullptr;
		uint32 m_QueueFamilyIndex = 0;

		std::vector<VkQueueFamilyProperties> m_QueueProperties;
	};

}; //namespace Graphics