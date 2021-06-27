#pragma once

#include "Types/Vector.hpp"
#include "Types/Matrix.hpp"

#include "WorldSpace.hpp"

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
		Camera() {}
		virtual ~Camera() = default;
	};

	class PerspectiveCamera
		: public Camera
	{
	public:
		virtual const Mat4f& GetProjectionMatrix() const override { RecalculatePerspective(); return mMatProjection; }

		float GetFOV() const { RecalculatePerspective(); return mFov; }
		float GetAspect() const { RecalculatePerspective(); return mAspect; }
		float GetClipNear() const { RecalculatePerspective(); return mClipNear; }
		float GetClipFar() const { RecalculatePerspective(); return mClipFar; }

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
		virtual ~PerspectiveCamera() override;

	private:
		void RecalculatePerspective() const; // not really const

		mutable Mat4f mMatProjection{ 1.f };
		mutable bool mProjectionDirty{ true };

		float mFov; // radians
		float mAspect;
		float mClipNear;
		float mClipFar;
	};

	// TODO: Freelook camera
	// TODO: Orthographic camera
	
	class SphericalCamera
		: public PerspectiveCamera
	{
	public:
		SphericalCamera();
		virtual ~SphericalCamera() override;
			
		virtual Type GetType() const override { return Type::Spherical; }
		static Type GetStaticType() { return Type::Spherical; }

		virtual const Mat4f& GetViewMatrix() const override { Recalculate(); return mMatView; }
		virtual const Mat4f& GetViewProjectionMatrix() const override { Recalculate(); return mMatViewProjection; }

		const Vec3f& GetPosition() const noexcept { return mTargetPosition; }
		virtual void SetPosition( const Vec3f& new_target_position );
		// Move relative to world
		virtual void Move( const Vec3f& delta );
		// Move relative to camera look direction
		virtual void Pan( const Vec3f& delta );
		// Move relative to camera look direction without changing Z coordinate
		virtual void PanXY( const Vec2f& delta );

		float GetRadius() const noexcept { return mRadius; }
		virtual void SetRadius( float radius );
		// +: in, -: out
		virtual void Zoom( float distance ) { SetRadius( GetRadius() - distance ); }

		virtual void SetRotation( float pitch_rad, float yaw_rad );
		virtual void SetRotationD( float pitch_deg, float yaw_deg );
		virtual void Rotate( float delta_pitch_rad, float delta_yaw_rad );

		// [pitch, yaw], radians
		[[nodiscard]] virtual std::pair<float, float> GetSphericalRotation() const;
		[[nodiscard]] virtual Vec3f GetCartesianRotation() const;

	private:
		void Recalculate() const;

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
