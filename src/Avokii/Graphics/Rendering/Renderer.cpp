#include "Renderer.hpp"

#include "Avokii/API/VideoAPI.hpp"
#include "Avokii/Graphics/Window.hpp"
#include "Avokii/Types/Colour.hpp"
#include "Avokii/Graphics/Camera.hpp"
#include "Avokii/Graphics/Shader.hpp"
#include "Avokii/Graphics/Texture.hpp"
#include "Avokii/Graphics/VertexArray.hpp"

namespace Avokii::Graphics
{
	// Static renderer data
	std::unique_ptr<Renderer::SceneData> Renderer::msSceneData = std::make_unique<Renderer::SceneData>();

	struct Renderer::SceneData
	{
		Mat4f viewProjectionMatrix;
	};

	///
	/// Renderer
	/// 

	Renderer::Renderer( API::VideoAPI& rVideo )
		: mrVideo( rVideo )
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Init()
	{

	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::OnWindowResize( const uint32_t width, const uint32_t height )
	{
		mrVideo.SetViewport( Rect<uint32_t>( 0, 0, width, height ) );
	}

	void Renderer::BeginScene( Camera& rCamera )
	{	
		//mrVideo.SetClearColour( 0x101010FF_rgba );
		//mrVideo.Clear();

		msSceneData->viewProjectionMatrix = rCamera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit( const std::shared_ptr<Graphics::Shader>& shader, const std::shared_ptr<Graphics::VertexArray>& vertexArray, const Mat4f& modelTransform )
	{
		AV_ASSERT( shader != nullptr );
		if (!shader)
			return;

		shader->Bind();
		shader->SetMat4( "u_ViewProjection", msSceneData->viewProjectionMatrix );
		shader->SetMat4( "u_Model", modelTransform );

		vertexArray->Bind();
		mrVideo.DrawIndexed( vertexArray );
	}
}
