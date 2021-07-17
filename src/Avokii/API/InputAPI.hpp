#pragma once

#include "CoreAPIsEnum.hpp"
#include "BaseAPI.hpp"

namespace Avokii
{
	namespace Input
	{
		class KeyboardInput;
		class GamepadInput;
	}

	namespace API
	{
		class InputAPI
			: public BaseAPI
		{
			friend class Core;

		public:
			virtual ~InputAPI() {}

			static constexpr APIType GetType() noexcept { return CoreAPIs::Input; }

			virtual size_t GetKeyboardCount() const = 0;
			virtual std::shared_ptr<Input::KeyboardInput> GetKeyboard( size_t idx ) const = 0;;
			virtual size_t GetGamepadCount() const = 0;
			virtual std::shared_ptr<Input::GamepadInput> GetGamepad( size_t idx ) const = 0;

			// TODO: Mouse input

		private:
			virtual void BeginEvents( const PreciseTimestep& ts ) = 0;
		};
	}
}
