#include "Material.hpp"

namespace Graphics
{
	Material::Material()
	{
	}

	Material::Material( Resources::ResourceLoader& loader )
	{
		(void)loader;
		NOT_IMPLEMENTED;
	}

	std::shared_ptr<Material> Material::LoadResource( Resources::ResourceLoader& loader )
	{
		(void)loader;
		NOT_IMPLEMENTED;
		return {};
	}

	void Material::SetShader( std::shared_ptr<const Shader> shader )
	{
		this->shader = shader;
	}

	void Material::SetTextures( std::vector<std::shared_ptr<const Texture>> textures )
	{
		this->textures = textures;
	}
}
