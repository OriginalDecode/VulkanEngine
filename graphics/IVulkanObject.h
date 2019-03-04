#pragma once
#include "Core/Defines.h"
#include "Core/Types.h"

#define VK_FAILED( x ) x != VK_SUCCESS

namespace Graphics
{

	class IVulkanObject
	{
	public:

		IVulkanObject() = default;
		virtual ~IVulkanObject() = default;

		virtual void Init() = 0;
		virtual void Release() = 0;

	};


}; //namespace Graphics
