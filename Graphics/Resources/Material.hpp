#pragma once

#include <memory>

#include "Engine/Core/Resources/Resource.hpp"
#include "Engine/Core/Resources/ResourceTypes.hpp"

#include "Visual/Graphics/Shader.hpp"
#include "Visual/Graphics/Texture.hpp"
#include "Visual/Graphics/Types.hpp"

namespace Resources { class ResourceLoader; }

namespace Graphics
{
	class Material
		: public ::Resources::Resource
	{
	public:
		Material();
		explicit Material( Resources::ResourceLoader& loader );

		static constexpr ::Resources::AssetType GetResourceType() noexcept { return ::Resources::AssetType::Material; }
		static std::shared_ptr<Material> LoadResource( Resources::ResourceLoader& loader );
		
		const std::shared_ptr<const Shader> GetShader() const noexcept { return shader; }
		const std::vector<std::shared_ptr<const Texture>> GetTextures() const noexcept { return textures; }

		void SetShader( std::shared_ptr<const Shader> shader );
		void SetTextures( std::vector<std::shared_ptr<const Texture>> textures );

	protected:
		std::shared_ptr<const Shader> shader;
		std::vector<std::shared_ptr<const Texture>> textures;
	};
}
