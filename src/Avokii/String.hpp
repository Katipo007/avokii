#pragma once

#include <string>
#include <string_view>

namespace Avokii
{
	using Char = char;
	using String = std::basic_string<Char>;
	using StringView = std::basic_string_view<Char>;

	namespace StringLiterals = std::string_literals;
	namespace StringViewLiterals = std::string_view_literals;
}
