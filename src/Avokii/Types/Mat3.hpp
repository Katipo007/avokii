#pragma once

#include <glm/mat3x3.hpp>

namespace Avokii
{
	template<typename T>
	using Mat3 = glm::mat<3, 3, T>;
	using Mat3f = Mat3<float>;
}
