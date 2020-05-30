#pragma once

class StateStack;
class State
{
public:
	State() = default;

	virtual void InitState(StateStack* state_stack) = 0;
	virtual void Update(float dt) = 0;
	virtual void Render(bool render_through = false) = 0;
	virtual void EndState() = 0;

	virtual void PauseState() { m_Paused = true; }
	virtual void ResumeState() { m_Paused = false; }

protected:
	StateStack* m_StateStack = nullptr;
	bool m_Paused = false;
};
