#pragma once

namespace Avokii
{
	struct Timestep
	{
		float time;
		float delta;

		Timestep( float _time = 0.f, float _delta_time = 0.f )
			: time{ _time }
			, delta{ _delta_time }
		{}
	};

	struct PreciseTimestep
	{
		double time;
		double delta;

		PreciseTimestep( double _time = 0.f, double _delta_time = 0.f )
			: time( _time )
			, delta( _delta_time )
		{}

		operator Timestep() const { return Timestep( (float)time, (float)delta ); }
	};
}
