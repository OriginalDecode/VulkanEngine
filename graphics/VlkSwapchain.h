#pragma once

#include "Core/Defines.h"
#include "IGfxObject.h"

LPE_DEFINE_HANDLE( VkSwapchainKHR );

namespace Graphics
{
	class VlkDevice;
	class VlkPhysicalDevice;
	class Window;
	class VlkSwapchain final : public IGfxObject
	{
	public:
     VlkSwapchain() = default; 
		~VlkSwapchain() override;
        void Release(IGfxDevice* device) override;

		void Init( const VlkDevice& device, const VlkPhysicalDevice& physicalDevice, const Window& window );

	private:
		//void FillCreateInfo


		VkSwapchainKHR m_Swapchain;
	};


}; //namespace Graphics