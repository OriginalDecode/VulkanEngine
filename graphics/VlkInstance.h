#pragma once
#include <Core/Defines.h>

LPE_DEFINE_HANDLE( VkInstance );
LPE_DEFINE_HANDLE( VkSurfaceKHR );

struct VkWin32SurfaceCreateInfoKHR;
namespace Graphics
{
    class VlkInstance
    {
    public:
        VlkInstance() = default;
        ~VlkInstance();

        void Init();
        void Release();


		VkSurfaceKHR CreateSurface( const VkWin32SurfaceCreateInfoKHR& createInfo ) const;
		void DestroySurface( VkSurfaceKHR pSurface );

    private:
        VkInstance m_Instance = nullptr;
    };

}; //namespace Graphics