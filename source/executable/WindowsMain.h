#pragma once
#include "MainBase.h"
class WindowsMain final : public MainBase
{
public:
	WindowsMain() = default;
	~WindowsMain() override;
	void Init(HINSTANCE instanceHandle) override;
	bool Update() override;

private:
};
