#pragma once
#include <random>

namespace Core
{
	template<typename T>
	T Rand(const T& min, const T& max)
	{
		static std::random_device device;
		static std::mt19937 e2(device());
		std::uniform_real_distribution<> dist(min, max);
		return static_cast<T>(dist(e2));
	}

	template<typename T>
	T Rand(const T& min, const T& max, int seed)
	{
		static std::mt19937 randomizer(seed);
		std::uniform_real_distribution<> distr(min, max);
		return static_cast<T>(distr(randomizer));
	}

};