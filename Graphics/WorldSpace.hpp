#pragma once

#include "Types/Vec3.hpp"
#include "Types/Vec4.hpp"
#include <glm/geometric.hpp>

namespace Avokii::Worldspace
{
	namespace _constexpr
	{
		constexpr Vec3f Up3{ 0.f, 0.f, -1.f };
		constexpr Vec3f Forward3{ 0.f, 1.f, 0.f };
		constexpr Vec3f Right3{ 1.f, 0.f, 0.f };

		static_assert(glm::cross( Up3, Forward3 ) == Right3);
	}

	inline static const Vec3f Up3{ _constexpr::Up3 };
	inline static const Vec3f Forward3{ _constexpr::Forward3 };
	inline static const Vec3f Right3{ _constexpr::Right3 };

	inline static const Vec4f Up4{ Up3, 1.f };
	inline static const Vec4f Forward4{ Forward3, 1.f };
	inline static const Vec4f Right4{ Right3, 1.f };
}
