#pragma once
#include "State.h"

class Camera;

class Game final : public State
{
public:
	Game() = default;
	~Game() = default;

	void InitState(StateStack* state_stack) override;
	void Update(float dt) override;
	void Render(bool render_through = false) override;
	void EndState() override;

private:
	Camera* m_Camera = nullptr;
};
