#pragma once

#include <string>

#include "Avokii/Types/Vector.hpp"
#include "Avokii/Types/Matrix.hpp"

namespace Avokii::Graphics
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt( const std::string& name, int value ) = 0;
		virtual void SetUInt( const std::string& name, unsigned int value ) = 0;
		virtual void SetIntArray( const std::string& name, int* values, uint32_t count ) = 0;
		virtual void SetUIntArray( const std::string& name, unsigned int* values, uint32_t count ) = 0;
		virtual void SetFloat( const std::string& name, float value ) = 0;
		virtual void SetFloat3( const std::string& name, Vec3f value ) = 0;
		virtual void SetFloat4( const std::string& name, Vec4f value ) = 0;
		virtual void SetMat4( const std::string& name, Mat4f value ) = 0;

		virtual std::string_view GetName() const = 0;
	};
}
