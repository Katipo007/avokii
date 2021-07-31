#pragma once

#include "Avokii/Utility/HashedString.hpp"

namespace Avokii
{
	using ResourceId = HashedString;

	inline constexpr ResourceId ToResourceId( const String value ) noexcept { return ResourceId{ value.data() }; }
	inline constexpr ResourceId ToResourceId( const Char* const value ) noexcept { return ResourceId{ value }; }
	inline constexpr ResourceId ToResourceId( StringView value ) noexcept { return ResourceId{ value.data() }; }
}
