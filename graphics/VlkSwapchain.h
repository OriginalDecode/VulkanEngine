#pragma once

#include "Core/Defines.h"
#include "IGfxObject.h"

LPE_DEFINE_HANDLE( VkSwapchainKHR );

namespace Graphics
{
	class VlkDevice;
	class VlkSwapchain final : public IGfxObject
	{
	public:
     VlkSwapchain() = default; 
		~VlkSwapchain() override;
        void Release(IGfxDevice* device) override;

		void Init( VlkDevice* device );

	private:
		VkSwapchainKHR m_Swapchain;
	};


}; //namespace Graphics