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
		Mesh( BufferLayout layout );
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

		virtual const BufferLayout& GetLayout() const noexcept { return vertex_layout; }

	protected:
		uint32_t num_vertices{ 0 };
		std::vector<std::byte> vertex_data;
		std::vector<Index_T> indices;
		BufferLayout vertex_layout;

		std::vector<std::shared_ptr<const Texture>> textures;
		std::shared_ptr<const Material> material;
	};


	class BasicMesh final
		: public Mesh
	{
	public:
		inline static const BufferLayout BasicMeshLayout
		{
			{ Graphics::ShaderDataType::Float4, "a_Position" },
			{ Graphics::ShaderDataType::Float4, "a_Colour" },
			{ Graphics::ShaderDataType::Float4, "a_Normal" },
			{ Graphics::ShaderDataType::Float4, "a_Texcoord0" },
		};

#pragma pack(push, 1)
		struct Vertex
		{
			glm::vec4 position;
			glm::vec4 normal;
			glm::vec4 colour;
			glm::vec4 texcoord0;

			constexpr Vertex()
				: position{ 0, 0, 0, 1 }, normal{ 0, 0, 0, 1 }, colour{ 1, 1, 1, 1 }, texcoord0{ 0, 0, 0, 0 }
			{}

			constexpr Vertex( glm::vec4 _pos, glm::vec4 _normal, glm::vec4 _colour, glm::vec4 _texcoord0 )
				: position{ _pos }, normal{ _normal }, colour{ _colour }, texcoord0{ _texcoord0 }
			{}
		};
#pragma pack(pop)

		const BufferLayout& GetLayout() const noexcept override { return BasicMeshLayout; }

	public:
		BasicMesh();
	};
}
