#include "Camera.hpp"

#pragma warning(push, 0)
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#pragma warning(pop)

#include "Worldspace.hpp"

namespace
{
	Avokii::Vec3f SphericalToCartesian( float theta, float phi, float radius )
	{
		const float x = radius * glm::sin( phi ) * glm::sin( theta );
		const float y = radius * glm::cos( phi );
		const float z = radius * glm::sin( phi ) * glm::cos( theta );

		return Avokii::Vec3f{ x, y, z };
	}
}

namespace Avokii::Graphics
{
	///
	/// SphericalCamera
	/// 
	SphericalCamera::SphericalCamera()
	{
	}

	SphericalCamera::~SphericalCamera()
	{
	}

	void SphericalCamera::SetPosition( const Vec3f& new_target_position )
	{
		mTargetPosition = new_target_position;
		mDirty = true;
	}

	void SphericalCamera::Move( const Vec3f& delta )
	{
		mTargetPosition += delta;
		mDirty = true;
	}

	void SphericalCamera::Pan( const Vec3f& delta )
	{
		Recalculate();

		mTargetPosition += Vec3f{ glm::row( mRotation, 0 ) * delta.x };
		mTargetPosition += Vec3f{ glm::row( mRotation, 1 ) * delta.y };
		mTargetPosition += Vec3f{ glm::row( mRotation, 2 ) * delta.z };
		mDirty = true;
	}

	void SphericalCamera::PanXY( const glm::vec2& delta )
	{
		Recalculate();

		mTargetPosition += Vec3f{ glm::row( mRotation, 0 ) * delta.x };
		mTargetPosition += Vec3f{ glm::row( mRotation, 1 ) * delta.y };
		mDirty = true;
	}

	void SphericalCamera::SetRadius( float radius_ )
	{
		mRadius = radius_;
		mDirty = true;
	}

	void SphericalCamera::SetRotation( float pitch_, float yaw_ )
	{
		mPitch = pitch_;
		mYaw = yaw_;
		mDirty = true;
	}

	void SphericalCamera::SetRotationD( float theta_deg, float phi_deg )
	{
		SetRotation( glm::radians( theta_deg ), glm::radians( phi_deg ) );
	}

	void SphericalCamera::Rotate( float delta_pitch_rad, float delta_yaw_rad )
	{
		mPitch += delta_pitch_rad;
		mYaw += delta_yaw_rad;
		mDirty = true;
	}

	std::pair<float, float> SphericalCamera::GetSphericalRotation() const
	{
		return std::make_pair( mPitch, mYaw );
	}

	Vec3f SphericalCamera::GetCartesianRotation() const
	{
		return glm::normalize( SphericalToCartesian( mPitch, mYaw, mRadius ) );
	}

	void SphericalCamera::Recalculate() const
	{
		if (!mDirty)
			return;

		constexpr auto identity_mat = glm::identity<Mat4f>();

		mRotation = glm::eulerAngleXZ( mPitch, mYaw ); // equivalent: glm::rotate( identity_mat, pitch, Worldspace::Right3 )* glm::rotate( identity_mat, yaw, Worldspace::Up3 );
		mMatView = glm::translate( identity_mat, Worldspace::Up3 * mRadius ) * mRotation * glm::translate( identity_mat, mTargetPosition );
		mMatViewProjection = GetProjectionMatrix() * mMatView;
		mDirty = false;
	}


	///
	/// PerspectiveCamera
	/// 
	PerspectiveCamera::PerspectiveCamera()
		: mFov{ glm::radians( 60.f ) }
		, mAspect{ 1 }
		, mClipNear{ 0.1f }
		, mClipFar{ 100.f }
		, mProjectionDirty{ true }
	{
	}

	PerspectiveCamera::PerspectiveCamera( float _fov_rad, float _aspect, float _clip_near, float _clip_far )
		: mFov{ _fov_rad }
		, mAspect{ _aspect }
		, mClipNear{ _clip_near }
		, mClipFar{ _clip_far }
		, mProjectionDirty{ true }
	{
	}

	PerspectiveCamera::~PerspectiveCamera()
	{
	}

	void PerspectiveCamera::Reset( float _fov_rad, float _aspect, float _clip_near, float _clip_far )
	{
		mFov = _fov_rad;
		mAspect = _aspect;
		mClipNear = _clip_near;
		mClipFar = _clip_far;
		mProjectionDirty = true;
	}

	void PerspectiveCamera::Resetd( float fov_deg, float aspect, float clip_near, float clip_far )
	{
		Reset( glm::radians( fov_deg ), aspect, clip_near, clip_far );
	}

	void PerspectiveCamera::SetFOV( float _fov_rad )
	{
		mFov = _fov_rad;
		mProjectionDirty = true;
	}

	void PerspectiveCamera::SetFOVd( float fov_deg )
	{
		SetFOV( glm::radians( fov_deg ) );
	}

	void PerspectiveCamera::SetAspectRatio( float _aspect )
	{
		mAspect = _aspect;
		mProjectionDirty = true;
	}

	void PerspectiveCamera::SetClipSpace( float _clip_near, float _clip_far )
	{
		mClipNear = _clip_near;
		mClipFar = _clip_far;
		mProjectionDirty = true;
	}

	void PerspectiveCamera::RecalculatePerspective() const
	{
		if (!mProjectionDirty)
			return;

		mMatProjection = glm::perspective( mFov, mAspect, mClipNear, mClipFar );
		//mat_projection = Mat4f(1.f);
		//mat_projection = glm::perspective( 75.f, 4.0f / 3.0f, 0.1f, 100.0f );
		mProjectionDirty = false;
	}
}
