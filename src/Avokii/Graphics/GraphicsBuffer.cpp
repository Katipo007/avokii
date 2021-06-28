#include "GraphicsBuffer.hpp"

#include "Avokii/Assert.hpp"

namespace Avokii::Graphics
{
    uint32_t ShaderDataTypeSize( const ShaderDataType& type )
    {
		switch (type)
		{
		case ShaderDataType::Float:		return 4;
		case ShaderDataType::Float2:	return 4 * 2;
		case ShaderDataType::Float3:	return 4 * 3;
		case ShaderDataType::Float4:	return 4 * 4;

		case ShaderDataType::Mat3:		return 4 * 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4 * 4;

		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Int2:		return 4 * 2;
		case ShaderDataType::Int3:		return 4 * 3;
		case ShaderDataType::Int4:		return 4 * 4;

		case ShaderDataType::uInt:		return 4;
		case ShaderDataType::uInt2:		return 4 * 2;
		case ShaderDataType::uInt3:		return 4 * 3;
		case ShaderDataType::uInt4:		return 4 * 4;

		case ShaderDataType::Bool:		return 1;

		default:
			AV_ASSERT( false, "Unknown ShaderDataType!" );
			return 0;
		}
    }

	BufferElement::BufferElement( ShaderDataType type, std::string_view name, bool normalised )
		: name{ name.data() }
		, type{ type }
		, offset{ 0 }
		, normalised{ normalised }
		, size{ ShaderDataTypeSize( type ) }
	{
	
	}

	uint32_t BufferElement::GetComponentCount() const
	{
		switch (type)
		{
		case ShaderDataType::Float:		return 1;
		case ShaderDataType::Float2:	return 2;
		case ShaderDataType::Float3:	return 3;
		case ShaderDataType::Float4:	return 4;

		case ShaderDataType::Mat3:		return 3; // 3* float3
		case ShaderDataType::Mat4:		return 4; // 4* float4

		case ShaderDataType::Int:		return 1;
		case ShaderDataType::Int2:		return 2;
		case ShaderDataType::Int3:		return 3;
		case ShaderDataType::Int4:		return 4;

		case ShaderDataType::uInt:		return 1;
		case ShaderDataType::uInt2:		return 2;
		case ShaderDataType::uInt3:		return 3;
		case ShaderDataType::uInt4:		return 4;

		case ShaderDataType::Bool:		return 1;

		default:
			AV_ASSERT( false, "Unknown ShaderDataType!" );
			return 0;
		}
	}

	BufferLayout::BufferLayout( const std::initializer_list<BufferElement>& elements )
		: mElements{ elements }
	{
		CalculateOffsetsAndStride();
	}

	void BufferLayout::CalculateOffsetsAndStride()
	{
		size_t offset = 0;
		mStride = 0;
		for (auto& element : mElements)
		{
			element.offset = offset;
			offset += element.size;
			mStride += element.size;
		}
	}
}
