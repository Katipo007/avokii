#pragma once

#include "Engine/Utility/MagicEnum.hpp"

namespace Resources
{
	enum class AssetType
	{
		Texture,
		Shader,
		MaterialDefinition,
		Material,
		Mesh,
		SpriteSheet,
		Sprite,
	};

	constexpr std::string_view GetAssetTypeName( const AssetType type ) noexcept { return magic_enum::enum_name( type ); }
}
