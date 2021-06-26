#pragma once

#include <glm/vec4.hpp>

namespace Avokii
{
	template<typename T>
	using Vec4 = glm::vec<4, T>;
	using Vec4f = Vec4<float>;
}
