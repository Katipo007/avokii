#pragma once

#include <glm/vec3.hpp>

namespace Avokii
{
	template<typename T>
	using Vec3 = glm::vec<3, T>;
	using Vec3f = Vec3<float>;
}
