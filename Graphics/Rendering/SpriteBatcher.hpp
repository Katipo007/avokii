#pragma once

#include "Types/Colour.hpp"
#include "Types/Vec2.hpp"
#include "Types/Vec3.hpp"
#include "Types/Mat4.hpp"

#include "Resources/ResourceHandle.hpp"

namespace Avokii
{
	namespace API { class VideoAPI; }
	namespace Resources { class Image; }
}

namespace Avokii::Graphics
{
	class Camera;
	class Sprite;
	class Texture;

	// Unlike Renderer this does not provide a static interface and allows multiple instances to be created
	//
	// "Flat" refers to a "floor" or laid on the ground
	// "Standing" refers to standing upright, perpendicular to the floor
	class SpriteBatcher final
	{
	private:
		using TextureSlotId = unsigned int;

	public:
		struct Statistics
		{
			uint32_t nDrawCalls = 0;
			uint32_t nQuads = 0;

			uint32_t GetTotalVertexCount() const { return nQuads * 4; }
			uint32_t GetTotalIndexCount() const { return nQuads * 6; }
		};

	public:
		explicit SpriteBatcher( API::VideoAPI& video );
		virtual ~SpriteBatcher();

		void Begin( const Camera& camera, const Mat4f& world_transform = Mat4f{ 1.f } );
		void EndScene();

#pragma region Drawing state
		void PushMultiplyColour( ColourRGBA colour );
		void PopMultiplyColour();

		void Flush();
#pragma endregion

#pragma region Submission
		void DrawStandingSprite( const ResourceHandle<Sprite>& sprite, Vec3f location );
#pragma endregion

		const Statistics& GetStatistics() const { return mStatistics; }
		void ClearStats();

	private:
		void StartBatch();
		void NextBatch();

		// Warning: can cause batch breaks
		TextureSlotId FindOrAddTexture( const std::shared_ptr<const Graphics::Texture>& texture_handle );

	private:
		API::VideoAPI& mrVideo;

		struct Data;
		std::unique_ptr<Data> mpData;

		Statistics mStatistics;
		bool mActive{ false };
	};
}
