#include "Timer.h"
#include <cassert>

namespace Core
{
	Timer::Timer()
		: m_IsActive( false )
		, m_IsPaused( false )
	{
	}

	void Timer::Update()
	{
		m_Current = std::chrono::steady_clock::now();
		std::chrono::duration<float> diff = m_Current - ( ( !m_IsActive || m_IsPaused ) ? m_Current : m_Prev );
		m_Time = diff.count();
		m_Prev = m_Current;
	}

	void Timer::Start()
	{
		m_IsActive = true;
		m_IsPaused = false;
		m_Start = std::chrono::steady_clock::now();
		m_Current = m_Start;
		m_Prev = m_Start;
	}

	void Timer::Stop()
	{
		m_IsActive = false;
	}

	void Timer::Pause()
	{
		m_IsPaused = true;
	}

	void Timer::Resume()
	{
		if( !m_IsActive )
		{
			assert( !"Can't resume a timer that isn't started. Did you mean Start()?" );
			return;
		}

		m_IsPaused = false;
	}

	const float Timer::GetTime() const
	{
		return m_Time;
	}

	const float Timer::GetTotalTime() const
	{
		return std::chrono::duration<float>( m_Current - m_Start ).count();
	}
}; // namespace Core