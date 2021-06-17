#include "Mesh.hpp"

namespace Graphics
{
	Mesh::Mesh( BufferLayout _layout )
		: vertex_layout{ _layout }
	{
		ASSERT( vertex_layout.GetStride() > 0 );
	}

	Mesh::Mesh( Resources::ResourceLoader& loader )
	{
		(void)loader;
		NOT_IMPLEMENTED;
	}

	std::shared_ptr<Mesh> Mesh::LoadResource( Resources::ResourceLoader& loader )
	{
		(void)loader;
		NOT_IMPLEMENTED;
		return {};
	}

	void Mesh::SetVertices( std::span<const std::byte> new_bytes, uint32_t new_num_vertices )
	{
		this->vertex_data.assign( std::begin( new_bytes ), std::end( new_bytes ) );
		this->num_vertices = new_num_vertices;
	}

	void Mesh::SetIndices( std::span<const Index_T> new_indices )
	{
		this->indices.assign( std::begin( new_indices ), std::end( new_indices ) );
	}

	void Mesh::SetMaterial( std::shared_ptr<const Material> new_material )
	{
		this->material = new_material;
	}

	BasicMesh::BasicMesh()
		: Mesh( vertex_format )
	{
	}
}
