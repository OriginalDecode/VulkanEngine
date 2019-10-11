#include "StateStack.h"
#include "State.h"

void StateStack::PopCurrentMainState()
{
	while( m_GameStates[m_MainIndex].Size() > 0 )
	{
		PopCurrentSubState();
	}

	m_GameStates.RemoveCyclicAtIndex( m_MainIndex );
	m_MainIndex--;

	if( m_MainIndex >= 0 )
		m_SubIndex = m_GameStates[m_MainIndex].Size() - 1;
	else
		m_SubIndex = -1;

	if( m_MainIndex >= 0 && m_SubIndex >= 0 )
		m_GameStates[m_MainIndex][m_SubIndex]->ResumeState();
}

void StateStack::PopCurrentSubState()
{
	m_GameStates[m_MainIndex][m_SubIndex]->EndState();
	m_GameStates[m_MainIndex].RemoveCyclicAtIndex( m_SubIndex );
	m_SubIndex--;

	if( m_SubIndex >= 0 )
		m_GameStates[m_MainIndex][m_SubIndex]->ResumeState();
}

void StateStack::PushState( State* game_state, EGameStateType game_state_type )
{
	if( game_state_type == MAIN )
	{
		m_GameStates.Add( SubStateContainer( 4 ) );
		m_MainIndex = m_GameStates.Size() - 1; // what happens at size 0? state -1?
		m_GameStates[m_MainIndex].Add( game_state );
		State* state = m_GameStates[m_MainIndex].GetLast();
		state->InitState( this );
	}
	else if( game_state_type == SUB )
	{
		m_GameStates[m_MainIndex].Add( game_state );
		game_state->InitState( this );
		m_SubIndex = m_GameStates[m_MainIndex].Size() - 1;
	}
}

void StateStack::PauseCurrentState()
{
	m_GameStates[m_MainIndex][m_SubIndex]->PauseState();
}

void StateStack::ResumeCurrentState()
{
	m_GameStates[m_MainIndex][m_SubIndex]->ResumeState();
}

bool StateStack::UpdateCurrentState( float dt )
{
	if( m_GameStates.Size() > 0 )
	{
		m_GameStates[m_MainIndex][m_SubIndex]->Update( dt );
		return true;
	}
	return false; //( m_GameStates.Size() > 0 );
}

void StateStack::Clear()
{
	while( m_GameStates.Size() > 0 )
	{
		PopCurrentMainState();
	}
}
