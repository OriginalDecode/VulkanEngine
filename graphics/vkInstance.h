#pragma once
#include <Core/Defines.h>

LPE_DEFINE_HANDLE(VkInstance);

namespace Graphics
{
	class vkInstance
	{
	public:
		vkInstance() = default;
		~vkInstance();

		void Init();
		void Release();

		VkInstance get() { return m_Instance; }
	private:

		VkInstance m_Instance = nullptr;
		

	};

}; //namespace Graphics