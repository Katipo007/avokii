#pragma once

#include <concepts>
#include <random>

namespace Avokii::Random
{
	using Generator = std::mt19937;

	Generator CreateGenerator( unsigned int seed )
	{
		return Generator{ seed };
	}

	Generator CreateGenerator()
	{
		static std::random_device device;
		return Generator{ device() };
	}

	Generator& GetGlobalGenerator() noexcept
	{
		static auto generator{ CreateGenerator() };
		return generator;
	}

	template<std::floating_point T>
	T GenerateReal( const T& min, const T& max, Generator& generator )
	{
		const std::uniform_real_distribution<T> distribution{ min, max };
		return distribution( generator );
	}

	template<std::integral T>
	T GenerateIntegral( const T& min, const T& max, Generator& generator )
	{
		const std::uniform_int_distribution<T> distribution{ min, max };
		return distribution( generator );
	}

	template<std::floating_point T>
	bool Chance( const T& chance, Generator& generator )
	{
		AV_ASSERT( chance >= 0.f );
		AV_ASSERT( chance <= 1.f );
		return (chance > 0.f) && ((chance >= 1.f) || (std::normal_distribution{ 0.f, 1.f }(generator) < chance));
	}
}
