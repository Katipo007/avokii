#include "Mesh.hpp"

namespace Avokii::Graphics
{
	Mesh::Mesh( BufferLayout _layout )
		: vertexLayout{ _layout }
	{
		AV_ASSERT( vertexLayout.GetStride() > 0 );
	}

	Mesh::Mesh( ResourceLoader& _rLoader )
	{
		(void)_rLoader;
		AV_NOT_IMPLEMENTED;
	}

	std::shared_ptr<Mesh> Mesh::LoadResource( ResourceLoader& _rLoader )
	{
		(void)_rLoader;
		AV_NOT_IMPLEMENTED;
		return {};
	}

	void Mesh::SetVertices( std::span<const std::byte> _newBytes, uint32_t _newNumVertices )
	{
		vertexData.assign( std::begin( _newBytes ), std::end( _newBytes ) );
		numVertices = _newNumVertices;
	}

	void Mesh::SetIndices( std::span<const Index_T> _newIndices )
	{
		indices.assign( std::begin( _newIndices ), std::end( _newIndices ) );
	}

	void Mesh::SetMaterial( std::shared_ptr<const Material> _newMaterial )
	{
		material = _newMaterial;
	}

	BasicMesh::BasicMesh()
		: Mesh{ sBasicMeshLayout }
	{
	}
}
