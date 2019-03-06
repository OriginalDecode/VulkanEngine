#pragma once
#include "IGfxObject.h"

LPE_DEFINE_HANDLE(VkDevice);

namespace Graphics
{
	class vkDevice final : public IGfxObject
	{
	public:
		vkDevice() = default;
		~vkDevice();

		virtual void Init() override;
		virtual void Release() override;
	private:
		VkDevice m_Device = nullptr;
	};

}; //namespace Graphics