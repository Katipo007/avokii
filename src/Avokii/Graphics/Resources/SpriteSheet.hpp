#pragma once

#include "Avokii/Resources/BaseResource.hpp"
#include "Avokii/Resources/ResourceTypes.hpp"

#include "Avokii/Containers/StringHashMap.hpp"
#include "Avokii/File/Filepath.hpp"
#include "Avokii/Geometry/Point2D.hpp"
#include "Avokii/Geometry/Size.hpp"
#include "Avokii/Geometry/Rect.hpp"

namespace Avokii
{
	class ResourceLoader;
	class ResourceManager;

	namespace Graphics
	{
		class Texture;

		struct SpriteSheetEntry
		{
			Point2D<float> pivot;
			Rect<float> uvs;
			Size<float> size;
			bool rotated = false;
			bool trimmed = false;
		};

		class SpriteSheet final
			: public BaseResource
		{
		public:
			using SpriteIdx_T = uint32_t;

			SpriteSheet( ResourceManager& manager_ );

			const std::shared_ptr<const Texture>& GetTexture() const noexcept;
			const SpriteSheetEntry& GetSprite( StringView name ) const;
			const SpriteSheetEntry& GetSprite( SpriteIdx_T idx ) const;

			size_t GetNumSprites() const noexcept;
			SpriteIdx_T GetSpriteIndex( StringView name ) const;
			bool HasSprite( StringView name ) const;

			bool LoadFromJson( StringView json_string, const Filepath& filepath_prefix );
			void SetTextureId( StringView new_texture_id );
			void AddSprite( StringView name, const SpriteSheetEntry& sprite );

			static constexpr AssetType GetResourceType() noexcept { return AssetType::SpriteSheet; }
			static std::shared_ptr<SpriteSheet> LoadResource( ResourceLoader& loader );

		private:
			void ReloadTexture( ResourceManager& manager ) const;

		private:
			ResourceManager& mrManager;

			String mTextureAssetId;
			mutable std::shared_ptr<const Texture> mpTexture;
			std::vector<SpriteSheetEntry> mSprites;
			StringHashMap<SpriteIdx_T> mIdToSpriteIdxMap;
		};

		class Sprite final
			: public BaseResource
		{
		public:
			Sprite( std::shared_ptr<const SpriteSheet> parent, SpriteSheet::SpriteIdx_T idx );

			const SpriteSheetEntry& GetSprite() const;
			SpriteSheet::SpriteIdx_T GetIndex() const noexcept;
			std::shared_ptr<const SpriteSheet> GetSpriteSheet() const;

			static constexpr AssetType GetResourceType() noexcept { return AssetType::Sprite; }
			static std::shared_ptr<Sprite> LoadResource( ResourceLoader& loader );

		private:
			std::weak_ptr<const SpriteSheet> mpParentSpriteSheet;
			SpriteSheet::SpriteIdx_T mIndex{ static_cast<SpriteSheet::SpriteIdx_T>(-1) };
		};
	}
}
