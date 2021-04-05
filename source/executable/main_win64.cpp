

#include "graphics/Window.h"
#include "graphics/GraphicsEngine.h"

#include "core/Timer.h"
#include "input/InputManager.h"
#include "Logger/Debug.h"

#include "game/StateStack.h"
#include "game/Game.h"
#include "imgui/imgui.h"

#ifdef _WIN32
#include <Windows.h>
#include "WindowsMain.h"
int WINAPI WinMain(HINSTANCE instance, HINSTANCE /*prevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
#elif _OSX
int main(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	Graphics::CreateImGuiContext();

	MainBase* main = nullptr;
#ifdef _WIN32
	main = new WindowsMain();
	main->Init(instance);
#endif
	Log::Debug::Create();
	main->GetWindow()->SetText("Kaffe bönan");

	Graphics::GraphicsEngine::Create();
	Graphics::GraphicsEngine& graphics_engine = Graphics::GraphicsEngine::Get();
	if(!graphics_engine.Init(*main->GetWindow()))
		return -1;

	Input::InputManager& input = Input::InputManager::Get();
	input.Initialize(main->GetWindow()->GetHandle(), instance);

	Core::Timer timer;
	timer.Init();

	Game game;
	StateStack state_stack;
	state_stack.PushState(&game, StateStack::MAIN);

	do
	{
		timer.Update();
		main->Update();

		state_stack.UpdateCurrentState(timer.GetTime());
		input.Update();
		graphics_engine.Present(timer.GetTime());

	} while(true);

	delete main;

	Input::InputManager::Destroy();
	Log::Debug::Destroy();

	return 0;
}
