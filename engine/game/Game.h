#pragma once
#include "State.h"

class Camera;

class Game : public State
{
public:
	Game() = default;
	~Game() = default;

	STATE_INTERFACE;


private:

	Camera* m_Camera{ nullptr };

};
