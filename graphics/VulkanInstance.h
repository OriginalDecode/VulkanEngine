#pragma once
#include "IVulkanObject.h"

LPE_DEFINE_HANDLE(VkInstance);

namespace Graphics
{
	class VulkanInstance final : public IVulkanObject
	{
	public:
		VulkanInstance();
		~VulkanInstance();

		virtual void Init() override;
		virtual void Release() override;

		VkInstance get() { return m_Instance; }
	private:

		VkInstance m_Instance = nullptr;
		

	};

}; //namespace Graphics