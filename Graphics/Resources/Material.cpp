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

	void Material::SetShader( std::shared_ptr<const Shader> new_shader )
	{
		this->shader = new_shader;
	}

	void Material::SetTextures( std::vector<std::shared_ptr<const Texture>> new_textures )
	{
		this->textures = new_textures;
	}
}
