#pragma once

#include "Vendor/frozen/include/frozen/unordered_map.h"

namespace Avokii
{
	template <class Key, class Value, std::size_t N, typename Hash = frozen::anna<Key>, class KeyEqual = std::equal_to<Key>>
	using ConstexprHashMap = frozen::unordered_map<Key, Value, N, Hash, KeyEqual>;
}
