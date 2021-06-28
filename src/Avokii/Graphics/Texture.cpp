#include "Texture.hpp"

#include "Avokii/Core.hpp"
#include "Avokii/API/VideoAPI.hpp"
#include "Avokii/Resources/ResourceManager.hpp"
#include "Avokii/Resources/ResourceLoader.hpp"

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
