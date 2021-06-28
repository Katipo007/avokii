#include "SpriteBatcher.hpp"

#include <cinttypes>
#include <numeric>
#include <stack>

#include "Avokii/API/VideoAPI.hpp"

#include "Avokii/Graphics/Camera.hpp"
#include "Avokii/Graphics/DeviceCapabilities.hpp"
#include "Avokii/Graphics/GraphicsBuffer.hpp"
#include "Avokii/Graphics/VertexArray.hpp"
#include "Avokii/Graphics/Shader.hpp"
#include "Avokii/Graphics/Texture.hpp"
#include "Avokii/Graphics/Resources/SpriteSheet.hpp"

#include "Avokii/Containers/ContainerOperations.hpp"

using namespace Avokii::ContainerOps;

namespace Avokii::Graphics
{
	namespace
	{
#pragma pack(push, 1)
		struct QuadVertex
		{
			Vec3f pos;
			Vec4f colour;
			Vec2f texcoord;
			uint32_t texture_index;

			QuadVertex()
				: pos{ 0.f, 0.f, 0.f }
				, colour{ 1.f, 1.f, 1.f, 1.f }
				, texcoord{ 0.f, 0.f }
				, texture_index{ 0 }
			{}

			QuadVertex( Vec3f pos, Vec4f colour, Vec2f texcoord, uint32_t texture_index )
				: pos{ pos }
				, colour{ colour }
				, texcoord{ texcoord }
				, texture_index{ texture_index }
			{}
		};
#pragma pack(pop)

		static const BufferLayout VertexLayout
		{
			BufferElement{ ShaderDataType::Float3, "a_Position" },
			BufferElement{ ShaderDataType::Float4, "a_Colour" },
			BufferElement{ ShaderDataType::Float2, "a_Texcoord" },
			BufferElement{ ShaderDataType::uInt, "a_TextureIndex" },
		};

		constexpr uint32_t DefaultTextureIndex = 0; // index that the plain white texture should be loaded in
	}

	///
	/// Data
	/// 
	struct SpriteBatcher::Data
	{
		const uint32_t NMaxQuads;
		const uint32_t NMaxVertices;
		const uint32_t NMaxIndices;
		const uint32_t NMaxTextureSlots;

		API::VideoAPI& rVideo;

		// multiply colour
		std::stack<Vec4f> multiply_colour;

		// device objects
		std::shared_ptr<Graphics::VertexArray> va;
		std::shared_ptr<Graphics::VertexBuffer> vb;
		std::shared_ptr<Graphics::Shader> default_shader;
		std::shared_ptr<Graphics::Texture> white_texture;
		std::vector<std::shared_ptr<const Graphics::Texture>> texture_slots;

		// data pointers/counters
		std::vector<QuadVertex> vertex_data;
		uint32_t quad_index_count = 0;
		uint32_t texture_slot_index = 0;
		std::shared_ptr<Graphics::Shader> active_shader;

		// current scene data
		const Camera* pSceneCamera = nullptr;
		Mat4f scene_transform{ 1.f };

		Data( API::VideoAPI& _rVideo, const uint32_t _maxQuads, const uint32_t _maxTextureSlots )
			: rVideo{ _rVideo }
			, NMaxQuads{ _maxQuads }
			, NMaxVertices{ NMaxQuads * 4 }
			, NMaxIndices{ NMaxQuads * 6 }
			, NMaxTextureSlots{ std::min( (uint32_t)32, _maxTextureSlots ) }
		{
			if (NMaxQuads < 1)
				throw std::runtime_error( "Device reported it can't render quads?" );
			if (NMaxTextureSlots <= 1)
				throw std::runtime_error( "Device reported not enough texture slots" );

			std::shared_ptr<Graphics::IndexBuffer> ib;

			// vertex buffer
			vb = rVideo.CreateVertexBuffer(
				VertexBufferDefinition
				{
					.name = "SpriteBatcher VB",
					.layout = VertexLayout,
					.data = std::vector<unsigned char>( sizeof(QuadVertex) * NMaxVertices ),
				} );

			// index buffer
			{
				IndexBufferDefinition ib_props
				{
					.name = "SpriteBatcher IB",
				};

				// generate the indices, a repeating pattern
				constexpr uint32_t quad_indices[] = { 0, 1, 2, 1, 3, 2 };
				constexpr uint32_t n_quad_indices = sizeof( quad_indices ) / sizeof( quad_indices[0] );
				uint32_t i = 0;
				ib_props.indices.reserve( NMaxIndices );
				std::generate_n( std::back_inserter( ib_props.indices ), NMaxIndices, [&]() mutable { return ((i / n_quad_indices) * 4) + quad_indices[i++ % n_quad_indices]; } );

				ib = rVideo.CreateIndexBuffer( ib_props );
			}

			// vertex array
			va = rVideo.CreateVertexArray( VertexArrayDefinition
				{
					.name = "SpriteBatcher VA",
					.vertex_buffers = { vb },
					.index_buffer = std::move( ib ),
				} );

			// texture slots
			texture_slots.resize( NMaxTextureSlots );
			Fill( texture_slots, nullptr );

			// white texture
			{
				white_texture = rVideo.CreateTexture( TextureDefinition
					{
						.size = { 1, 1 },
					} );
				uint32_t white_texture_data = 0xffffffff;
				white_texture->SetData( &white_texture_data, sizeof( decltype(white_texture_data) ) );

				// first slot is always the white texture
				texture_slots[0] = white_texture;
			}

			// default shader
			{
				std::vector<int> initial_samplers{ std::vector<int>( NMaxTextureSlots ) };
				std::iota( std::begin( initial_samplers ), std::end( initial_samplers ), 0 ); // fill initial samplers with 0, 1, 2, 3, etc

				default_shader = rVideo.CreateShader( Filepath{ "Shaders/DefaultSpriteBatchShader.glsl" } ); // TODO: replace extention once we support multiple pipelines
				default_shader->Bind();
				default_shader->SetIntArray( "u_Textures", initial_samplers.data(), NMaxTextureSlots );
			}

			// default states
			{
				vertex_data.reserve( NMaxQuads );

				multiply_colour.emplace( Vec4f{ 1.f, 1.f, 1.f, 1.f } );
			}
		}

		~Data() = default;
	};


	///
	/// SpriteBatcher
	/// 

	SpriteBatcher::SpriteBatcher( API::VideoAPI& _rVideo )
		: mrVideo( _rVideo )
	{
		const auto& capabilities = mrVideo.GetDeviceCapabilities();

		// TODO: determine max number of vertexes/indices
		mpData = std::make_unique<Data>( mrVideo, 20000, std::min( (uint32_t)capabilities.max_texture_slots, (uint32_t)10 ) );
	}

	SpriteBatcher::~SpriteBatcher() = default;

	void SpriteBatcher::Begin( const Camera& camera, const glm::mat4& world_transform )
	{
		AV_ASSERT( !mActive );

		mpData->pSceneCamera = &camera;
		mpData->scene_transform = world_transform;

		mpData->default_shader->Bind();
		mpData->default_shader->SetMat4( "u_ViewProjection", camera.GetViewProjectionMatrix() );
		mpData->default_shader->SetMat4( "u_Model", world_transform );

		mpData->active_shader = mpData->default_shader;

		mActive = true;
		StartBatch();
	}

	void SpriteBatcher::EndScene()
	{
		AV_ASSERT( mActive );
		Flush();

		if (mpData->active_shader)
			mpData->active_shader->Unbind();
		mActive = false;
	}

	void SpriteBatcher::Flush()
	{
		if (mpData->vertex_data.empty())
			return; // nothing currently pending
		
		if (mpData->active_shader == nullptr)
			mpData->active_shader = mpData->default_shader;

		mpData->active_shader->Bind();

		// bind textures
		for (uint32_t i = 0; i < mpData->texture_slot_index; i++)
			mpData->texture_slots[i]->Bind( i );

		mpData->va->Bind();

		// move data into the vertex buffer object
		mpData->vb->SetData( mpData->vertex_data.data(), (uint32_t)mpData->vertex_data.size() * sizeof( QuadVertex ) );

		mrVideo.DrawIndexed( mpData->va, mpData->quad_index_count );

		mpData->va->Unbind();

		++mStatistics.nDrawCalls;
	}

	void SpriteBatcher::StartBatch()
	{
		AV_ASSERT( mActive );
		mpData->quad_index_count = 0;
		mpData->vertex_data.clear(); // reset data pointer to the start
		AV_ASSERT( mpData->vertex_data.capacity() > 0 );
		mpData->texture_slot_index = DefaultTextureIndex + 1;
	}

	void SpriteBatcher::NextBatch()
	{
		AV_ASSERT( mActive );
		Flush();
		StartBatch();
	}

	SpriteBatcher::TextureSlotId SpriteBatcher::FindOrAddTexture( const std::shared_ptr<const Graphics::Texture>& texture_handle )
	{
		SpriteBatcher::TextureSlotId index = std::numeric_limits<TextureSlotId>::max();

		const auto first = std::begin( mpData->texture_slots );
		const auto last = first + (size_t)mpData->texture_slot_index;

		// found the entry
		if (const auto found = std::find( first, last, texture_handle ); found < last)
		{
			index = static_cast<SpriteBatcher::TextureSlotId>(std::distance( first, found ));
		}
		// add a new entry
		else
		{
			// break batches if exceeding max number of bound texture slots
			if (mpData->texture_slot_index >= mpData->NMaxTextureSlots)
				NextBatch();

			index = static_cast<SpriteBatcher::TextureSlotId>(mpData->texture_slot_index);
			mpData->texture_slots[index] = texture_handle;
			mpData->texture_slot_index++;
		}

		AV_ASSERT( index > 0 && index < std::numeric_limits<TextureSlotId>::max() );
		return index;
	}

	void SpriteBatcher::DrawStandingSprite( const ResourceHandle<Sprite>& _sprite, Vec3f _location )
	{
		if (!_sprite)
			return;
		auto sprite_sheet = _sprite->GetSpriteSheet();
		if (!sprite_sheet)
			return;

		if (mpData->quad_index_count >= mpData->NMaxIndices)
			NextBatch();

		auto& img = _sprite->GetSprite();
		
		const auto img_size_vec = glm::vec2( img.size.width, img.size.height );
		const auto min = glm::vec2( -img.pivot.x, -img.pivot.y );
		const auto max = min + img_size_vec;

		const auto& texture_id = FindOrAddTexture( sprite_sheet->GetTexture() );
		const auto& uvs = img.uvs;
		const auto& multiply_colour = mpData->multiply_colour.top();

		mpData->vertex_data.emplace_back( Vec3f{ _location.x + min.x, _location.y + 0.f, _location.z + min.y }, multiply_colour, Vec2f( uvs.GetLeft(), uvs.GetTop() ), texture_id ); // top left
		mpData->vertex_data.emplace_back( Vec3f{ _location.x + max.x, _location.y + 0.f, _location.z + min.y }, multiply_colour, Vec2f( uvs.GetRight(), uvs.GetTop() ), texture_id ); // top right
		mpData->vertex_data.emplace_back( Vec3f{ _location.x + min.x, _location.y + 0.f, _location.z + max.y }, multiply_colour, Vec2f( uvs.GetLeft(), uvs.GetBottom() ), texture_id ); // bottom left
		mpData->vertex_data.emplace_back( Vec3f{ _location.x + max.x, _location.y + 0.f, _location.z + max.y }, multiply_colour, Vec2f( uvs.GetRight(), uvs.GetBottom() ), texture_id ); // bottom right

		mpData->quad_index_count += 6;
		++mStatistics.nQuads;
	}

	void SpriteBatcher::PushMultiplyColour( ColourRGBA colour )
	{
		mpData->multiply_colour.emplace( std::move( colour.AsFloatsRGBA() ) );
	}

	void SpriteBatcher::PopMultiplyColour()
	{
		AV_ASSERT( mpData->multiply_colour.size() > 1 );
		mpData->multiply_colour.pop();
	}

	void SpriteBatcher::ClearStats()
	{
		memset( &mStatistics, 0, sizeof( SpriteBatcher::Statistics ) );
	}
}
