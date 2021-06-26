#pragma once

#include <cinttypes>
#include <optional>
#include <string>

namespace Avokii::Graphics
{
	enum class ShaderDataType
	{
		None = 0,

		Float,
		Float2,
		Float3,
		Float4,

		Mat3,
		Mat4,

		Int,
		Int2,
		Int3,
		Int4,

		uInt,
		uInt2,
		uInt3,
		uInt4,

		Bool
	};

	static uint32_t ShaderDataTypeSize( const ShaderDataType& type );

	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		uint32_t size;
		size_t offset;
		bool normalised;

		BufferElement() = default;
		BufferElement( ShaderDataType type, std::string_view name, bool normalised = false );

		uint32_t GetComponentCount() const;
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout( const std::initializer_list<BufferElement>& elements );

		uint32_t GetStride() const { return mStride; }
		const std::vector<BufferElement>& GetElements() const { return mElements; }

		std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
		std::vector<BufferElement>::iterator end() { return mElements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return mElements.end(); }
		std::vector<BufferElement>::const_iterator cbegin() const { return mElements.cbegin(); }
		std::vector<BufferElement>::const_iterator cend() const { return mElements.cend(); }

	private:
		void CalculateOffsetsAndStride();

	private:
		std::vector<BufferElement> mElements;
		uint32_t mStride = 0;
	};


	struct VertexBufferDefinition
	{
		std::optional<std::string> name;
		BufferLayout layout;
		std::vector<unsigned char> data;

		template<typename T>
		void SetDataFromVector( const std::vector<T>& in_ )
		{
			const auto data_size = in_.size() * sizeof( T );
			data.resize( data_size );
			memcpy( data.data(), in_.data(), data_size );
		}
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData( const void* data, uint32_t size ) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout( const BufferLayout& layout ) = 0;
	};

	struct IndexBufferDefinition
	{
		std::optional<std::string> name;
		std::vector<uint32_t> indices;
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;
	};
}
