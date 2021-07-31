#pragma once

#include <entt/core/hashed_string.hpp>
#include "Avokii/String.hpp"

namespace Avokii
{
    typedef ::entt::basic_hashed_string<Char> HashedString;
}


namespace std
{
    template <>
    struct hash<Avokii::HashedString>
    {
        std::size_t operator()( const Avokii::HashedString& k ) const
        {
            return k.value();
        }
    };
}
