#include "Core.hpp"

#include <assert.h>

#include "Resources/ResourceManager.hpp"
#include "AbstractGame.hpp"

#include "API/BaseAPI.hpp"
#include "API/DearImGuiAPI.hpp"
#include "API/InputAPI.hpp"
#include "API/SystemAPI.hpp"
#include "API/VideoAPI.hpp"

#include "Resources/StandardResources.hpp"

#include "Containers/ContainerOperations.hpp"

using namespace Avokii::ContainerOps;

namespace Avokii
{
	Core::Core( CoreProperties&& _props, std::unique_ptr<AbstractGame> _game )
		: mpGame( std::move( _game ) )
		, mResourceInitaliserFunc{ _props.resource_initaliser_func }
		, mTargetFps{ std::max( 0, _props.fps ) }
	{
		AV_ASSERT( _props.IsValid() );
		if (!_props.IsValid())
			throw std::runtime_error( "Bad core properties" );

		if (!mpGame)
			throw std::runtime_error( "Where yo game at? (Game object was null at Core construction)" );

		///
		/// initialise plugins
		///
		mApis.resize( _props.max_plugins );
		for (APIType t = 0; t < _props.max_plugins; t++)
		{
			if (auto plugin = _props.plugin_factory( *this, t ))
				mApis[t] = std::move( plugin );
		}
		AV_LOG_INFO( LoggingChannels::Application, "{} plugins initalised", CountIf( mApis, []( const auto& entry ) { return entry != nullptr; } ) );
	}

	Core::~Core()
	{
		Shutdown();
	}

	void Core::Init()
	{
		assert( !mIsInitialised );

		InitResources();
		InitAPIs();
		InitRNG();

		mpGame->mpCore = this;
		mpGame->mpResourceManager = mpResourceManager.get();
		mpGame->Init();

		mIsInitialised = true;
	}

	int Core::Dispatch()
	{
		using Clock_T = std::chrono::steady_clock;

		int64_t num_steps = 0;
		const Clock_T::time_point start_time = Clock_T::now();
		Clock_T::time_point target_time;
		Clock_T::time_point last_time;
		target_time = last_time = start_time;

		if (mTargetFps <= 0)
		{
			while (mIsRunning)
			{
				const auto current_time = Clock_T::now();
				const double current_time_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch()).count() / 1000.0;
				constexpr double FixedDeltaTimeSeconds = 1.0 / 60.0;

				const auto timestep = PreciseTimestep( current_time_seconds, FixedDeltaTimeSeconds );
				DoFixedUpdate( timestep );
				DoVariableUpdate( timestep );
				rGetRequiredAPI<API::SystemAPI>().Sleep( static_cast<unsigned long>(FixedDeltaTimeSeconds * 1000) );
			}
		}
		else
		{
			while (mIsRunning)
			{
				Clock_T::time_point current_time = Clock_T::now();
				const double current_time_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch()).count() / 1000.0;

				// Time for a fixed update?
				if (current_time >= target_time)
				{
					constexpr int MaxFixedStepsPerFrame = 5;
					const double fixed_timestep_seconds = 1.0 / mTargetFps;

					// Perform a given number of steps this frame
					const int steps_needed = static_cast<int>(std::chrono::duration<float>( current_time - target_time ).count() * mTargetFps);
					for (int i = 0; i < std::min( steps_needed, MaxFixedStepsPerFrame ); i++)
						DoFixedUpdate( PreciseTimestep( current_time_seconds, fixed_timestep_seconds ) );

					num_steps += steps_needed;
					target_time = start_time + std::chrono::microseconds( (num_steps * 1000000ll) / mTargetFps );
				}
				else
				{
					std::this_thread::yield();
				}

				// Variable update
				constexpr double MaxDeltaTimeSeconds = 0.1;
				const double seconds_since_last_frame = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time).count() / 1000.0;
				DoVariableUpdate( PreciseTimestep( current_time_seconds, std::min( seconds_since_last_frame, MaxDeltaTimeSeconds ) ) );

				last_time = current_time;
			}
		}

		return 0;
	}

	void Core::InitResources()
	{
		AV_ASSERT( !mpResourceManager );
		mpResourceManager = std::make_unique<ResourceManager>( *this );

		InitStandardResources( *mpResourceManager );

		if (mResourceInitaliserFunc)
			mResourceInitaliserFunc( *mpResourceManager );
	}

	void Core::InitRNG()
	{
		time_t current_time = time( nullptr );
		clock_t current_clock = clock();
		int seed = static_cast<int>(current_time) ^ static_cast<int>(current_clock) ^ 0xb67b820e;
		srand( seed );
	}

	void Core::Shutdown()
	{
		AV_ASSERT( mIsInitialised );

		mpGame->OnGameEnd();
		mpGame.reset();

		ShutdownAPIs();
		mpResourceManager.reset();

		mIsInitialised = false;
	}

	void Core::DoFixedUpdate( const PreciseTimestep& ts )
	{
		assert( ts.delta > 0 );

		for (auto& plugin : mActiveApis)
			plugin->OnFixedUpdate( ts, StepType::PreGameStep );

		if (mIsRunning)
			mpGame->OnFixedUpdate( ts );

		for (auto& plugin : mActiveApis)
			plugin->OnFixedUpdate( ts, StepType::PostGameStep );
	}

	void Core::DoVariableUpdate( const PreciseTimestep& ts )
	{
		AV_ASSERT( ts.delta >= 0 );
		PumpEvents( ts );

		for (auto& plugin : mActiveApis)
			plugin->OnVariableUpdate( ts, StepType::PreGameStep );

		if (mpGame->GetExitCode())
		{
			mExitCode = mpGame->GetExitCode().value();
			mIsRunning = false;
		}

		if (mIsRunning)
			mpGame->OnVariableUpdate( ts );

		for (auto& plugin : mActiveApis)
			plugin->OnVariableUpdate( ts, StepType::PostGameStep );

		DoRender( ts );
	}

	void Core::DoRender( const PreciseTimestep& ts )
	{
		if (auto* video_api = GetAPI<API::VideoAPI>())
		{
			video_api->BeginRender();

			for (auto& plugin : mActiveApis)
				plugin->OnRender( ts, StepType::PreGameStep );

			if (mIsRunning)
				mpGame->OnRender( ts );

			for (auto& plugin : mActiveApis)
				plugin->OnRender( ts, StepType::PostGameStep );

			video_api->EndRender();
		}
	}

	void Core::PumpEvents( const PreciseTimestep& ts )
	{
		auto* system_api = GetAPI<API::SystemAPI>();
		auto* input_api = GetAPI<API::InputAPI>();
		auto* video_api = GetAPI<API::VideoAPI>();
		auto* dearimgui_api = GetAPI<API::DearImGuiAPI>();

		if (input_api)
			input_api->BeginEvents( ts );

		if (!system_api->GenerateEvents( video_api, input_api, dearimgui_api ))
		{
			mExitCode = 0;
			mIsRunning = false;
		}
	}

	void Core::InitAPIs()
	{
		// order is important

		mActiveApis.clear();
		for (auto& api : mApis )
		{
			if (api != nullptr)
			{
				api->Init();
				mActiveApis.push_back( api.get() );
			}
		};
	}

	void Core::ShutdownAPIs()
	{
		// order is important and should be done in reverse of that in InitAPIs()

		mActiveApis.clear();
		for (auto& api : mApis | std::views::reverse )
		{
			if (api != nullptr)
				api->Shutdown();
		};
	}
}
