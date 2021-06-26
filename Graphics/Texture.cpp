#include "Texture.hpp"

#include "Core.hpp"
#include "API/VideoAPI.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/ResourceLoader.hpp"

namespace Avokii::Graphics
{
	std::shared_ptr<Texture> Texture::LoadResource( ResourceLoader& loader )
	{
		std::shared_ptr<Texture> texture;

		auto& core = loader.GetManager().GetCore();
		auto& video = core.GetRequiredAPI<API::VideoAPI>();

		Filepath filepath{ loader.GetAssetId() };

		// TODO: some way of storing/fetching metadata

		return video.CreateTexture(
			filepath,
			TextureLoadProperties
			{
				.y_flip = false,
			}
		);
	}
}
