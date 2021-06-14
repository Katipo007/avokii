#pragma once

#include <memory>
#include <span>

#include "Engine/Core/Resources/Resource.hpp"
#include "Engine/Core/Resources/ResourceTypes.hpp"
#include "Engine/DataTypes/Bytes.hpp"

#include "Visual/Graphics/GraphicsBuffer.hpp"
#include "Visual/Graphics/Shader.hpp"
#include "Visual/Graphics/Texture.hpp"
#include "Visual/Graphics/Types.hpp"

namespace Resources { class ResourceLoader; }

namespace Graphics
{
	class Material;

	class Mesh
		: public ::Resources::Resource
	{
	public:
		static const BufferLayout vertex_format;
#pragma pack(push, 1)
		struct Vertex
		{
			glm::vec4 pos;
			glm::vec4 normal;
			glm::vec4 colour;
			glm::vec4 texcoord0;
		};
#pragma pack(pop)

	public:
		Mesh();
		explicit Mesh( Resources::ResourceLoader& loader );

		static constexpr ::Resources::AssetType GetResourceType() noexcept { return ::Resources::AssetType::Mesh; }
		static std::shared_ptr<Mesh> LoadResource( Resources::ResourceLoader& loader );

		uint32_t GetVertexCount() const noexcept { return num_vertices; }
		std::span<const std::byte> GetVertices() const noexcept { return vertex_data; }
		std::span<const Index_T> GetIndices() const noexcept { return indices; }
		const std::shared_ptr<const Material>& GetMaterial() const noexcept { return material; }

		void SetVertices( std::span<const std::byte> bytes, uint32_t num_vertices );
		void SetIndices( std::span<const Index_T> indices );
		void SetMaterial( std::shared_ptr<const Material> material );

	protected:
		uint32_t num_vertices{ 0 };
		std::vector<std::byte> vertex_data;
		std::vector<Index_T> indices;

		std::vector<std::shared_ptr<const Texture>> textures;
		std::shared_ptr<const Material> material;
	};
}
