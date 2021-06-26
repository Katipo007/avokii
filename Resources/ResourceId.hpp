#pragma once

#include "Utility/HashedString.hpp"

namespace Avokii
{
	using ResourceId = HashedString;
	using ResourceId_T = HashedString::hash_type;

	inline ResourceId_T ToResourceId( const String value ) noexcept { return ResourceId{ value.data() }; }
	inline constexpr ResourceId_T ToResourceId( StringView value ) noexcept { return ResourceId{ value.data() }; }
}
