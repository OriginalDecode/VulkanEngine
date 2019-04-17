#pragma once
#include <chrono>
namespace Core
{
	class Timer
	{
	public:
		Timer();
		~Timer() = default;

		void Update();

		const float GetTotalTime() const;
		const float GetTime() const;
		void Start();
		void Stop();
		void Pause();
		void Resume();

	private:
		using TimePoint = std::chrono::steady_clock::time_point;

		TimePoint m_Start;
		TimePoint m_Prev;
		TimePoint m_Current;

		float m_Time = 0.f;

		bool m_IsActive : 4;
		bool m_IsPaused : 4;
	};
}; // namespace Core