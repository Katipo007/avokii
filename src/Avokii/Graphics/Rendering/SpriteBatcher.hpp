#pragma once

#include "Avokii/Types/Colour.hpp"
#include "Avokii/Types/Vec2.hpp"
#include "Avokii/Types/Vec3.hpp"
#include "Avokii/Types/Mat4.hpp"

#include "Avokii/Resources/ResourceHandle.hpp"

namespace Avokii
{
	namespace API { class VideoAPI; }
	class Image;

	namespace Graphics
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

			void Begin( const Camera& rCamera, const Mat4f& worldTransform = Mat4f{ 1.f } );
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
			TextureSlotId FindOrAddTexture( const std::shared_ptr<const Graphics::Texture>& textureHandle );

		private:
			API::VideoAPI& mrVideo;

			struct Data;
			std::unique_ptr<Data> mpData;

			Statistics mStatistics;
			bool mActive{ false };
		};
	}
}
