#pragma once

#include "Timestep.hpp"

namespace Avokii
{
	class Core;

	namespace API
	{
		class BaseAPI
		{
			friend class Core;

		public:
			virtual ~BaseAPI() {}

			virtual StringView GetName() const noexcept = 0;

		private:
			virtual void Init() = 0;
			virtual void Shutdown() = 0;
			virtual void OnFixedUpdate( const PreciseTimestep&, StepType ) {}
			virtual void OnVariableUpdate( const PreciseTimestep&, StepType ) {}
			virtual void OnRender( const PreciseTimestep&, StepType ) {}
		};
	}
}
