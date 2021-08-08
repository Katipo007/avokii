#include "Mesh.hpp"

namespace Avokii::Graphics
{
	Mesh::Mesh( BufferLayout layout )
		: mVertexLayout{ layout }
	{
		AV_ASSERT( mVertexLayout.GetStride() > 0 );
	}

	Mesh::Mesh( ResourceLoader& r_loader )
	{
		(void)r_loader;
		AV_NOT_IMPLEMENTED;
	}

	std::shared_ptr<Mesh> Mesh::LoadResource( ResourceLoader& r_loader )
	{
		(void)r_loader;
		AV_NOT_IMPLEMENTED;
		return {};
	}

	void Mesh::SetVertices( std::span<const std::byte> new_bytes, uint32_t new_num_vertices )
	{
		mVertexData.assign( std::begin( new_bytes ), std::end( new_bytes ) );
		mNumVertices = new_num_vertices;
	}

	void Mesh::SetIndices( std::span<const Index_T> new_indices )
	{
		mIndices.assign( std::begin( new_indices ), std::end( new_indices ) );
	}

	void Mesh::SetMaterial( std::shared_ptr<const Material> new_material )
	{
		mMaterial = new_material;
	}

	BasicMesh::BasicMesh()
		: Mesh{ sBasicMeshLayout }
	{
	}
}
