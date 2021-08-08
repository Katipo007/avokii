#pragma once

#include <memory>
#include <span>

#include "Avokii/Resources/BaseResource.hpp"
#include "Avokii/Resources/ResourceTypes.hpp"
#include "Avokii/Types/Bytes.hpp"

#include "Avokii/Graphics/GraphicsBuffer.hpp"
#include "Avokii/Graphics/Shader.hpp"
#include "Avokii/Graphics/Texture.hpp"
#include "Avokii/Graphics/Types.hpp"

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
			explicit Mesh( ResourceLoader& r_loader );

			static constexpr AssetType GetResourceType() noexcept { return AssetType::Mesh; }
			static std::shared_ptr<Mesh> LoadResource( ResourceLoader& r_loader );

			uint32_t GetVertexCount() const noexcept { return mNumVertices; }
			std::span<const std::byte> GetVertices() const noexcept { return mVertexData; }
			std::span<const Index_T> GetIndices() const noexcept { return mIndices; }
			const std::shared_ptr<const Material>& GetMaterial() const noexcept { return mMaterial; }

			void SetVertices( std::span<const std::byte> bytes, uint32_t num_vertices );
			void SetIndices( std::span<const Index_T> indices );
			void SetMaterial( std::shared_ptr<const Material> material );

			virtual const BufferLayout& GetLayout() const noexcept { return mVertexLayout; }

		protected:
			uint32_t mNumVertices{ 0 };
			std::vector<std::byte> mVertexData;
			std::vector<Index_T> mIndices;
			BufferLayout mVertexLayout;

			std::vector<std::shared_ptr<const Texture>> mTextures;
			std::shared_ptr<const Material> mMaterial;
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

				constexpr Vertex( Vec4f pos, Vec4f normal, Vec4f colour, Vec4f texcoord0 )
					: position{ pos }, normal{ normal }, colour{ colour }, texcoord0{ texcoord0 }
				{}
			};
#pragma pack(pop)

			const BufferLayout& GetLayout() const noexcept override { return sBasicMeshLayout; }

		public:
			BasicMesh();
		};
	}
}
