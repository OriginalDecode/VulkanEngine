#pragma once


namespace Graphics
{
	class IGfxDevice;
	class IGfxObject
	{
	public:
		virtual ~IGfxObject() {}
		virtual void Release(IGfxDevice* device) = 0;

	};


}; //namespace Graphics