#include "Game.h"

void Game::InitState( StateStack* state_stack )
{
	m_StateStack = state_stack;
}

void Game::EndState() {}

void Game::Update( float /*dt*/ ) 
{


}

void Game::Render( bool /*render_through*/ ) {}
