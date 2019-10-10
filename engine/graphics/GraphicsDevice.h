#pragma once
#include "Core/Types.h"
#include "Utilities.h"


/*
	The usage of the graphics device class is to handle creation of buffers, shaders and various
	other resources that the graphics cards needs to operate.

	The class IGraphicsDevice is to ensure a common interface for different graphics devices.
	Such as vulkan and d3d11/d3d12.
	Vulkan and D3D12 does share more similarities, but code can still be put behind a common interface
	with D3D11. 

	We will start with building the vulkan graphics backend and then move on to the D3D11 backend, 
	and finish with the D3D12. Other backends should be able to be added through this interface as well.

	It should be clear and documented of what each function does and should not leave the programmer
	wondering of what a function is supposed to do since that would lead to confusion and that is
	bad for development time.

	If something gets too complex too fast, it's better to rething and redesign at an early stage than
	to proceed with something that will not work in the long run.

	Sometimes complexity is needed for stability and performance, but one should not have to sacriface 
	one for the other.

	It should be clear and well defined what something does and the same code should not be written twice (preferably )

*/

namespace Graphics
{
	class ConstantBuffer;
	class IGraphicsDevice
	{
		public:
			IGraphicsDevice() = default;
			virtual ~IGraphicsDevice() = default;

			virtual void BindConstantBuffer(ConstantBuffer* constantBuffer, uint32 offset) = 0;
			virtual void CreateConstantBuffer(ConstantBuffer* constantBuffer) = 0;
			virtual void DestroyConstantBuffer(ConstantBuffer* constantBuffer) = 0;

		private:
	};

}; //namespace Graphics