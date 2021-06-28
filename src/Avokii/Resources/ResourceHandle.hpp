#pragma once

#include <memory>

#include "Concepts.hpp"

namespace Avokii
{
	template<Concepts::Resource R>
	using ResourceHandle = std::shared_ptr<const R>;
}
