#pragma once

#include "CoreAPIsEnum.hpp"
#include "BaseAPI.hpp"

#include "Avokii/Geometry/Size.hpp"

namespace Avokii
{
	namespace API
	{
		class SystemAPI;

		class DearImGuiAPI
			: public BaseAPI
		{
			friend class Core;

		public:
			virtual ~DearImGuiAPI() {}

			static constexpr APIType GetType() noexcept { return CoreAPIs::DearImGui; }

			virtual void SetEnabled( const bool enable ) = 0;
			virtual bool GetEnabled() const noexcept = 0;

			virtual bool WantsToCaptureKeyboard() const noexcept = 0;
			virtual bool WantsToCaptureMouse() const noexcept = 0;

			virtual void ProcessSystemEvent( void* e ) = 0;

		private:
			virtual void OnWindowResized( Size<uint32_t> new_target_size ) = 0;
		};
	}
}
