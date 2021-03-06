#pragma once

#include <ranges>

#include "Avokii/Resources/BaseResource.hpp"
#include "Avokii/Resources/ResourceTypes.hpp"

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
			String assetId;
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

			SpriteSheet( ResourceManager& rManager );

			const std::shared_ptr<const Texture>& GetTexture() const noexcept;
			[[nodiscard]] const SpriteSheetEntry& GetSpriteByAssetId( StringView assetId ) const;
			[[nodiscard]] const SpriteSheetEntry& GetSpriteByResourceId( ResourceId resourceId ) const;
			[[nodiscard]] const SpriteSheetEntry& GetSpriteBySpriteIndex( SpriteIdx_T idx ) const;

			[[nodiscard]] size_t GetNumSprites() const noexcept;
			[[nodiscard]] SpriteIdx_T GetSpriteIndexByAssetId( StringView assetId ) const;
			[[nodiscard]] SpriteIdx_T GetSpriteIndexByResourceId( ResourceId resourceId ) const;
			[[nodiscard]] bool HasSprite( StringView assetId ) const noexcept;
			[[nodiscard]] bool HasSprite( ResourceId resourceId ) const noexcept;

			[[nodiscard]] auto GetSprites() const noexcept { return std::views::all( mSpriteIdxMapping ); }
			[[nodiscard]] auto GetSpriteEntries() const noexcept { return std::views::all( mSprites ); }

			bool LoadFromJson( StringView json_string, const Filepath& filepath_prefix );
			void SetTextureId( StringView textureAssetId );
			void AddSprite( StringView assetId, const SpriteSheetEntry& sprite );
			void AddSprite( ResourceId resourceId, const SpriteSheetEntry& sprite );

			// Load the contained sprites into the resource manager
			void LoadSprites() const noexcept;

			static constexpr AssetType GetResourceType() noexcept { return AssetType::SpriteSheet; }
			static std::shared_ptr<SpriteSheet> LoadResource( ResourceLoader& loader );

		private:
			void ReloadTexture( ResourceManager& manager ) const;

		private:
			ResourceManager& mrManager;

			String mTextureAssetId;
			mutable std::shared_ptr<const Texture> mpTexture;
			std::vector<SpriteSheetEntry> mSprites;
			std::unordered_map<ResourceId, SpriteIdx_T> mSpriteIdxMapping;
			mutable bool mSpritesLoadedIntoManager{ false };
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
