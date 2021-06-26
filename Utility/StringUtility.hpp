#pragma once

#include <array>
#include <string_view>

namespace Avokii::StringUtility
{
	template<size_t ARRAY_SIZE>
	size_t StringToArray( std::string_view _str, std::array<char, ARRAY_SIZE>& _array )
	{
		static_assert(ARRAY_SIZE > 0);
		const auto src_len = strlen( _str.data() );
		AV_ASSERT( src_len < ARRAY_SIZE );

		memcpy_s( _array.data(), ARRAY_SIZE, _str.data(), src_len );
		_array.back() = 0;
		return std::min( ARRAY_SIZE, src_len );
	}
}
