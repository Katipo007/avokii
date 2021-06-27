#pragma once

#include "Types/Mat4.hpp"

namespace Avokii
{
	namespace API { class VideoAPI; }

	namespace Graphics
	{
		class Camera;
		class Shader;
		class VertexArray;
		class Window;

		// 3D renderer interface
		class Renderer
		{
		public:
			explicit Renderer( API::VideoAPI& rVideo );
			virtual ~Renderer();

			void Init();
			void Shutdown();

			void OnWindowResize( const uint32_t width, const uint32_t height );

			void BeginScene( Camera& rCamera );
			void EndScene();

			void Submit( const std::shared_ptr<Graphics::Shader>& shader, const std::shared_ptr<Graphics::VertexArray>& vertexArray, const Mat4f& modelTransform = Mat4f{ 1.f } );

		private:
			API::VideoAPI& mrVideo;

			struct SceneData;
			static std::unique_ptr<SceneData> msSceneData;
		};
	}
}
