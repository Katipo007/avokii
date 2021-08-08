#pragma once

#include <array>
#include <string_view>

namespace Avokii::StringUtility
{
	template<size_t ARRAY_SIZE>
	size_t StringToArray( std::string_view str, std::array<char, ARRAY_SIZE>& array )
	{
		static_assert(ARRAY_SIZE > 0);
		const auto src_len = strlen( str.data() );
		AV_ASSERT( src_len < ARRAY_SIZE );

		memcpy_s( array.data(), ARRAY_SIZE, str.data(), src_len );
		array.back() = 0;
		return std::min( ARRAY_SIZE, src_len );
	}
}
