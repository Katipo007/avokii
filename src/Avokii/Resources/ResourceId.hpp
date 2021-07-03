#pragma once

#include "Avokii/Utility/HashedString.hpp"

namespace Avokii
{
	using ResourceId = HashedString;
	using ResourceId_T = HashedString::hash_type;

	inline ResourceId_T ToResourceId( const String value ) noexcept { return ResourceId{ value.data() }.value(); }
	inline constexpr ResourceId_T ToResourceId( const Char* const value ) noexcept { return ResourceId{ value }.value(); }
	inline constexpr ResourceId_T ToResourceId( StringView value ) noexcept { return ResourceId{ value.data() }.value(); }
}
