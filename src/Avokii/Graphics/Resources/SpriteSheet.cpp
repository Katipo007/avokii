#include "SpriteSheet.hpp"

#include "Avokii/File/FileOps.hpp"
#include "Avokii/Resources/ResourceManager.hpp"
#include "Avokii/Resources/ResourceLoader.hpp"
#include "Avokii/Containers/ContainerOperations.hpp"
#include "Avokii/Utility/Json.hpp"

#include "Avokii/Graphics/Texture.hpp"

#ifdef _DEBUG
#	define DEBUG_SPRITESHEET_LOADING 1
#else
#	define DEBUG_SPRITESHEET_LOADING 0
#endif

namespace
{
	using namespace Avokii;

	bool ParseFreeTexPackerSpritesheetJson( const Json& json, const Filepath& filepath_prefix, std::vector<Graphics::SpriteSheetEntry>& out, Filepath& texture_filename_out )
	{
		AV_ASSERT( std::filesystem::is_directory( filepath_prefix ) );

		const size_t old_out_size{ out.size() };

		const auto parse_frame = [&]( std::string_view key, const nlohmann::json& obj, const Size<float>& texture_size )
		{
			// rotated
			const bool rotated = obj.contains( "rotated" ) ? obj["rotated"].get<bool>() : false;
			if (rotated)
			{
				AV_ASSERT( false, "Currently don't support rotated images" );
				return;
			}

			// trimmed
			const bool trimmed = obj.contains( "trimmed" ) ? obj["trimmed"].get<bool>() : false;
			if (trimmed)
			{
				AV_ASSERT( false, "Currently don't support trimmed images" );
				return;
			}

			// frame
			const auto& frame = obj.at( "frame" );
			AV_ASSERT( frame.is_object(), "Expected frame to be an object" );
			const auto rect = Rect<float>{
				Point2D<float>{ frame["x"].get<float>(), frame["y"].get<float>() },
				Size<float>{ frame["w"].get<float>(), frame["h"].get<float>() }
			};
			AV_ASSERT( rect.IsValid(), "Frame bounds are not a valid rectangle" );
			if (!rect.IsValid())
				return;

			// pivot
			Point2D<float> pivot{ 0, 0 };
			if (obj.contains( "pivot" ))
			{
				const auto& pivot_obj = obj.at( "pivot" );
				pivot = { pivot_obj["x"].get<float>(), pivot_obj["y"].get<float>() };
			}

			out.emplace_back(
				Graphics::SpriteSheetEntry
				{
					.assetId = String{ key },
					.pivot = { rect.w * pivot.x, rect.h * pivot.y },
					.uvs = { Point2D<float>{ rect.x / texture_size.width, rect.y / texture_size.height }, Size<float>{ rect.w / texture_size.width, rect.h / texture_size.height } },
					.size = { rect.w, rect.h },
					.rotated = rotated,
					.trimmed = trimmed,
				} );
		};

		Size<float> texture_size;
		const auto meta = json.at( "meta" );
		// id the texture
		{
			auto texture_filename = meta.at( "image" ).get<std::string>();
			if (!texture_filename.empty())
				texture_filename_out = filepath_prefix / texture_filename;
			else
			{
				AV_LOG_WARN( LoggingChannels::Resource, "Spritesheet JSON didn't supply a texture filename" );
				texture_filename_out = "";
			}

			const auto& size_obj = meta.at( "size" );
			texture_size = { size_obj.at( "w" ).get<float>(), size_obj.at( "h" ).get<float>() };
			AV_ASSERT( texture_size.width > 0 && texture_size.height > 0 );
		}

		// parse images
		const auto frames = json.at( "frames" );
		for (const auto& [key, value] : frames.items())
		{
			try
			{
				parse_frame( key, value, texture_size );
			}
			catch( std::runtime_error& )
			{
#if DEBUG_SPRITESHEET_LOADING
				AV_ASSERT( false );
#endif
				continue;
			}
		}

		const auto added_images = out.size() - old_out_size;
		AV_ASSERT( added_images > 0, "Expected to load at least one image?" );
		return added_images > 0;
	}
}

namespace Avokii::Graphics
{
	SpriteSheet::SpriteSheet( ResourceManager& _rManager )
		: mrManager{ _rManager }
	{
	}

	const std::shared_ptr<const Texture>& SpriteSheet::GetTexture() const noexcept
	{
		if (!mpTexture)
			ReloadTexture( mrManager );

		return mpTexture;
	}

	const SpriteSheetEntry& SpriteSheet::GetSpriteByAssetId( StringView _assetId ) const
	{
		return GetSpriteBySpriteIndex( GetSpriteIndexByAssetId( _assetId ) );
	}

	const SpriteSheetEntry& SpriteSheet::GetSpriteByResourceId( ResourceId _resourceId ) const
	{
		return GetSpriteBySpriteIndex( GetSpriteIndexByResourceId( _resourceId ) );
	}

	const SpriteSheetEntry& SpriteSheet::GetSpriteBySpriteIndex( SpriteIdx_T idx ) const
	{
		return mSprites.at( idx );
	}

	size_t SpriteSheet::GetNumSprites() const noexcept
	{
		return std::size( mSprites );
	}

	SpriteSheet::SpriteIdx_T SpriteSheet::GetSpriteIndexByAssetId( StringView _assetId ) const
	{
		return GetSpriteIndexByResourceId( ToResourceId( _assetId ) );
	}

	SpriteSheet::SpriteIdx_T SpriteSheet::GetSpriteIndexByResourceId( ResourceId _resourceId ) const
	{
		if (const auto found = mSpriteIdxMapping.find( _resourceId ); found != std::end( mSpriteIdxMapping ))
			return found->second;

		AV_LOG_ERROR( LoggingChannels::Resource, "SpriteSheet does not contain sprite with resource id '{}'", _resourceId );
		return static_cast<SpriteIdx_T>(-1);
	}

	bool SpriteSheet::HasSprite( StringView _assetId ) const noexcept
	{
		return mSpriteIdxMapping.contains( ToResourceId( _assetId ) );
	}

	bool SpriteSheet::HasSprite( ResourceId _resourceId ) const noexcept
	{
		return mSpriteIdxMapping.contains( _resourceId );
	}

	bool SpriteSheet::LoadFromJson( StringView json_string, const Filepath& filepath_prefix )
	{
		AV_ASSERT( std::filesystem::is_directory( filepath_prefix ) );
		auto j = nlohmann::json::parse( json_string );
		bool handled = false;

		Filepath texture_filename;
		std::vector<SpriteSheetEntry> entries;
		try
		{
			if (j.contains( "frames" ) && j.contains( "meta" ) && j["meta"]["app"].is_string() && j["meta"]["app"].get<std::string>() == "http://free-tex-packer.com")
				handled = ParseFreeTexPackerSpritesheetJson( j, filepath_prefix, entries, texture_filename );
		}
		catch (nlohmann::json::exception& e)
		{
			AV_LOG_ERROR( LoggingChannels::Resource, "Exception while parsing spritesheet json: '{}'", e.what() );
			handled = false;
		}
		catch (std::runtime_error& e)
		{
			AV_LOG_ERROR( LoggingChannels::Resource, "Exception while parsing spritesheet:'{}'", e.what() );
			handled = false;
		}

		mTextureAssetId = Filepath{ texture_filename }.lexically_normal().generic_string();
		AV_ASSERT( !mTextureAssetId.empty() );
		for (auto& entry : entries)
			AddSprite( entry.assetId, entry );

		return handled;
	}

	void SpriteSheet::SetTextureId( StringView _textureId )
	{
		mTextureAssetId = _textureId;
		if (mpTexture)
			ReloadTexture( mrManager );
	}

	void SpriteSheet::AddSprite( StringView _assetId, const SpriteSheetEntry& _sprite )
	{
		return AddSprite( ToResourceId( _assetId ), _sprite );
	}

	void SpriteSheet::AddSprite( ResourceId _resourceId, const SpriteSheetEntry& _sprite )
	{
		const auto [it, success] = mSpriteIdxMapping.try_emplace( _resourceId, static_cast<SpriteIdx_T>(mSprites.size()) );
		if (success)
			mSprites.push_back( _sprite );
	}

	void SpriteSheet::LoadSprites() const noexcept
	{
		for (auto& entry : mSprites)
			mrManager.Load<Sprite>( entry.assetId );
	}

	std::shared_ptr<SpriteSheet> SpriteSheet::LoadResource( ResourceLoader& loader )
	{
		using namespace StringLiterals;

		auto sheet = std::make_shared<SpriteSheet>( loader.GetManager() );
		
		const Filepath filepath{ loader.GetAssetId() };
		String file_contents;
		if (FileOps::ReadFile( filepath, file_contents ))
			sheet->LoadFromJson( file_contents, FileOps::GetFileDirectory( filepath ) );
		else
			throw std::runtime_error( "Failed to load spritesheet '"s + std::string( loader.GetAssetId() ) + "'"s );

		return sheet;
	}

	void SpriteSheet::ReloadTexture( ResourceManager& _manager ) const
	{
		mpTexture = _manager.GetOrLoad<Texture>( mTextureAssetId );
	}


	/// 
	/// Sprite
	/// 

	Sprite::Sprite( std::shared_ptr<const SpriteSheet> _parent, SpriteSheet::SpriteIdx_T _idx )
		: mpParentSpriteSheet{ _parent }
		, mIndex{ _idx }
	{
	}

	const SpriteSheetEntry& Sprite::GetSprite() const
	{
		if( auto sheet = GetSpriteSheet() )
			return sheet->GetSpriteBySpriteIndex( mIndex );

		throw std::runtime_error( "Spritesheet has expired" );
	}

	SpriteSheet::SpriteIdx_T Sprite::GetIndex() const noexcept
	{
		return mIndex;
	}

	std::shared_ptr<const SpriteSheet> Sprite::GetSpriteSheet() const
	{
		return mpParentSpriteSheet.lock();
	}

	std::shared_ptr<Sprite> Sprite::LoadResource( ResourceLoader& loader )
	{
		auto& manager = loader.GetManager();
		auto& spritesheets = manager.GetCache<SpriteSheet>();
		const auto the_sheet = spritesheets.FindIf( [&loader]( const SpriteSheet& entry ) -> bool
			{
				return entry.HasSprite( loader.GetAssetId() );
			} );


		if (the_sheet)
			return std::make_shared<Sprite>( the_sheet, the_sheet->GetSpriteIndexByAssetId( loader.GetAssetId() ) );

		return nullptr;
	}
}
