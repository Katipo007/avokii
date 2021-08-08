#pragma once

#include "Avokii/Types/Vec3.hpp"
#include "Avokii/Types/Vec4.hpp"
#include <glm/geometric.hpp>

namespace Avokii::Worldspace
{
	constexpr Vec3f cxUp3{ 0.f, 0.f, -1.f };
	constexpr Vec3f cxForward3{ 0.f, 1.f, 0.f };
	constexpr Vec3f cxRight3{ 1.f, 0.f, 0.f };

	static_assert(glm::cross( cxUp3, cxForward3 ) == cxRight3);

	inline static const Vec3f Up3{ cxUp3 };
	inline static const Vec3f Forward3{ cxForward3 };
	inline static const Vec3f Right3{ cxRight3 };

	inline static const Vec4f Up4{ Up3, 1.f };
	inline static const Vec4f Forward4{ Forward3, 1.f };
	inline static const Vec4f Right4{ Right3, 1.f };
}
