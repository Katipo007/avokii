#pragma once

#include "Avokii/Types/Vector.hpp"
#include "Avokii/Types/Quaternion.hpp"
#include "Avokii/Types/Matrix.hpp"

#include "Avokii/Graphics/WorldSpace.hpp"

namespace Avokii::Graphics
{
	class Camera
	{
	public:
		enum class Type
		{
			Free,
			Orthographic,
			Spherical,

			NumCameraTypes,
		};

	public:
		virtual Type GetType() const = 0;

		virtual const Mat4f& GetProjectionMatrix() const = 0;
		virtual const Mat4f& GetViewMatrix() const = 0;
		virtual const Mat4f& GetViewProjectionMatrix() const = 0;

	protected:
		Camera() = default;
		virtual ~Camera() = default;
	};

	class PerspectiveCamera
		: public Camera
	{
	public:
		virtual const Mat4f& GetProjectionMatrix() const override { RecalculatePerspective(); return mMatProjection; }

		[[nodiscard]] float GetFOV() const { RecalculatePerspective(); return mFov; }
		[[nodiscard]] float GetAspect() const { RecalculatePerspective(); return mAspect; }
		[[nodiscard]] float GetClipNear() const { RecalculatePerspective(); return mClipNear; }
		[[nodiscard]] float GetClipFar() const { RecalculatePerspective(); return mClipFar; }

		virtual void Reset( float fov_rad, float aspect, float clip_near, float clip_far );
		void Resetd( float fov_deg, float aspect, float clip_near, float clip_far );
		virtual void SetFOV( float fov_rad );
		void SetFOVd( float fov_deg );
		virtual void SetAspectRatio( float aspect );
		virtual void SetAspectRatio( uint32_t width, uint32_t height ) { SetAspectRatio( (float)height / float( width ) ); }
		virtual void SetClipSpace( float clip_near, float clip_far );

	protected:
		PerspectiveCamera();
		PerspectiveCamera( float fov_rad, float aspect, float clip_near, float clip_far );

	private:
		void RecalculatePerspective() const; // not really const

		mutable Mat4f mMatProjection{ 1.f };
		mutable bool mProjectionDirty{ true };

		float mFov; // radians
		float mAspect;
		float mClipNear;
		float mClipFar;
	};

	class FreelookCamera
		: public PerspectiveCamera
	{
	public:
		FreelookCamera();

		[[nodiscard]] Type GetType() const override { return Type::Free; }
		[[nodiscard]] static Type GetStaticType() noexcept { return Type::Free; }

		[[nodiscard]] const Mat4f& GetViewMatrix() const override { Recalculate(); return mMatView; }
		[[nodiscard]] const Mat4f& GetViewProjectionMatrix() const override { Recalculate(); return mMatViewProjection; }

		[[nodiscard]] const Vec3f& GetPosition() const noexcept { return mPosition; }
		void SetPosition( const Vec3f& new_target_position );

		[[nodiscard]] const auto& GetOrientation() const noexcept { return mOrientation; }
		void SetOrientation( const Quaternion& orientation );

	private:
		void Recalculate() const;

	private:
		Vec3f mPosition{};
		Quaternion mOrientation{};

		// cached matrices
		mutable Mat4f mMatView{ 1.f };
		mutable Mat4f mMatViewProjection{ 1.f };

		mutable bool mDirty{ true };
	};

	class GimbalFreeLookCamera final
		: public FreelookCamera
	{

	};

	// TODO: Orthographic camera
	
	class SphericalCamera final
		: public PerspectiveCamera
	{
	public:
		SphericalCamera() = default;
			
		[[nodiscard]] Type GetType() const override { return Type::Spherical; }
		[[nodiscard]] static Type GetStaticType() { return Type::Spherical; }

		[[nodiscard]] const Mat4f& GetViewMatrix() const override { Recalculate(); return mMatView; }
		[[nodiscard]] const Mat4f& GetViewProjectionMatrix() const override { Recalculate(); return mMatViewProjection; }

		[[nodiscard]] const Vec3f& GetPosition() const noexcept { return mTargetPosition; }
		void SetPosition( const Vec3f& new_target_position );
		// Move relative to world
		void Move( const Vec3f& delta );
		// Move relative to camera look direction
		void Pan( const Vec3f& delta );
		// Move relative to camera look direction without changing Z coordinate
		void PanXY( const Vec2f& delta );

		float GetRadius() const noexcept { return mRadius; }
		void SetRadius( float radius );
		// +: in, -: out
		void Zoom( float distance ) { SetRadius( GetRadius() - distance ); }

		void SetRotation( float pitch_rad, float yaw_rad );
		void SetRotationD( float pitch_deg, float yaw_deg );
		void Rotate( float delta_pitch_rad, float delta_yaw_rad );

		// [pitch, yaw], radians
		[[nodiscard]] std::pair<float, float> GetSphericalRotation() const;
		[[nodiscard]] Vec3f GetCartesianRotation() const;

	private:
		void Recalculate() const;

	private:
		float mPitch{ 0 }; // radians
		float mYaw{ 0 }; // radians
		float mRadius{ 1 }; // distance from target, worldspace

		Vec3f mTargetPosition{ 0, 0, 0 };

		// cached matrices
		mutable Mat4f mRotation{ 1.f };
		mutable Mat4f mMatView{ 1.f };
		mutable Mat4f mMatViewProjection{ 1.f };

		mutable bool mDirty{ true };
	};
}
