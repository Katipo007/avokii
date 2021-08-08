#pragma once

namespace Avokii
{
	struct Timestep
	{
		float time;
		float delta;

		Timestep( float time = 0.f, float delta_time = 0.f )
			: time{ time }
			, delta{ delta_time }
		{}
	};

	struct PreciseTimestep
	{
		double time;
		double delta;

		PreciseTimestep( double time = 0.f, double delta_time = 0.f )
			: time( time )
			, delta( delta_time )
		{}

		operator Timestep() const { return Timestep( (float)time, (float)delta ); }
	};

	enum class StepType
	{
		PreGameStep,
		GameStep,
		PostGameStep,
	};
}
