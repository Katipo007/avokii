#pragma once

#include <glm/mat4x4.hpp>

namespace Avokii
{
	template<typename T>
	using Mat4 = glm::mat<4, 4, T>;
	using Mat4f = Mat4<float>;
}
