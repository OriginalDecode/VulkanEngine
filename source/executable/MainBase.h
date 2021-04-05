#pragma once
#include "core/Defines.h"
class Window;
class MainBase
{
public:
	MainBase() = default;
	virtual ~MainBase()
	{
		delete m_Window;
		m_Window = nullptr;
	};

	virtual void Init(HINSTANCE instanceHandle) = 0;
	virtual bool Update() = 0;
	Window* GetWindow() { return m_Window; }

protected:
	Window* m_Window{ nullptr };
};
