#pragma once

#include <memory>
#include <span>

#include "Resources/BaseResource.hpp"
#include "Resources/ResourceTypes.hpp"
#include "Types/Bytes.hpp"

#include "Graphics/GraphicsBuffer.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Types.hpp"

namespace Avokii
{
	class ResourceLoader;

	namespace Graphics
	{
		class Material;

		class Mesh
			: public BaseResource
		{
		public:
			Mesh( BufferLayout layout );
			explicit Mesh( ResourceLoader& rLoader );

			static constexpr AssetType GetResourceType() noexcept { return AssetType::Mesh; }
			static std::shared_ptr<Mesh> LoadResource( ResourceLoader& rLoader );

			uint32_t GetVertexCount() const noexcept { return numVertices; }
			std::span<const std::byte> GetVertices() const noexcept { return vertexData; }
			std::span<const Index_T> GetIndices() const noexcept { return indices; }
			const std::shared_ptr<const Material>& GetMaterial() const noexcept { return material; }

			void SetVertices( std::span<const std::byte> bytes, uint32_t num_vertices );
			void SetIndices( std::span<const Index_T> indices );
			void SetMaterial( std::shared_ptr<const Material> material );

			virtual const BufferLayout& GetLayout() const noexcept { return vertexLayout; }

		protected:
			uint32_t numVertices{ 0 };
			std::vector<std::byte> vertexData;
			std::vector<Index_T> indices;
			BufferLayout vertexLayout;

			std::vector<std::shared_ptr<const Texture>> textures;
			std::shared_ptr<const Material> material;
		};


		class BasicMesh final
			: public Mesh
		{
		public:
			inline static const BufferLayout sBasicMeshLayout
			{
				{ Graphics::ShaderDataType::Float4, "a_Position" },
				{ Graphics::ShaderDataType::Float4, "a_Colour" },
				{ Graphics::ShaderDataType::Float4, "a_Normal" },
				{ Graphics::ShaderDataType::Float4, "a_Texcoord0" },
			};

#pragma pack(push, 1)
			struct Vertex
			{
				Vec4f position;
				Vec4f normal;
				Vec4f colour;
				Vec4f texcoord0;

				constexpr Vertex()
					: position{ 0, 0, 0, 1 }, normal{ 0, 0, 0, 1 }, colour{ 1, 1, 1, 1 }, texcoord0{ 0, 0, 0, 0 }
				{}

				constexpr Vertex( Vec4f _pos, Vec4f _normal, Vec4f _colour, Vec4f _texcoord0 )
					: position{ _pos }, normal{ _normal }, colour{ _colour }, texcoord0{ _texcoord0 }
				{}
			};
#pragma pack(pop)

			const BufferLayout& GetLayout() const noexcept override { return sBasicMeshLayout; }

		public:
			BasicMesh();
		};
	}
}
