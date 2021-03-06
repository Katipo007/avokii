#pragma once

#include <array>
#include <vector>

#include "API/CoreAPIsEnum.hpp"
#include "Timestep.hpp"

namespace Avokii
{
	class AbstractGame;
	class Core;
	class ResourceManager;

	namespace API
	{
		class BaseAPI;
		class SystemAPI;
		class VideoAPI;
	}

	struct CoreProperties
	{
		int fps = 60;

		std::function<void( ResourceManager& )> resourceInitaliserFunc;

		unsigned maxPlugins = 0;
		std::function<std::unique_ptr<API::BaseAPI>( Core&, APIType )> pluginFactory;

		bool IsValid() const noexcept
		{
			return true
				&& (fps >= 0)
				&& (maxPlugins >= CoreAPIs::Type::User)
				&& (bool)resourceInitaliserFunc
				&& (bool)pluginFactory
				;
		}
	};

	template<typename T>
	concept APIConcept = requires(T t)
	{
		std::derived_from<T, API::BaseAPI>;
		{ T::GetType() } -> std::same_as<APIType>;
	};

	/// <summary>
	/// The be all container for the application
	/// </summary>
	class Core final
	{
	public:
		explicit Core( CoreProperties&&, std::unique_ptr<AbstractGame> game );
		~Core();

		void Init();

		AbstractGame& GetGame() const { return *mpGame; }
		ResourceManager& GetResourceManager() const { return *mpResourceManager; }

		template<APIConcept API_T>
		API_T* rGetAPI() noexcept
		{
			constexpr auto type{ API_T::GetType() };
			return dynamic_cast<API_T*>(rGetAPI( type ));
		}

		template<APIConcept API_T>
		const API_T* GetAPI() const noexcept
		{
			constexpr auto type{ API_T::GetType() };
			return dynamic_cast<const API_T*>(GetAPI( type ));
		}

		template<APIConcept API_T>
		API_T& rGetRequiredAPI()
		{
			constexpr auto type{ API_T::GetType() };
			return dynamic_cast<API_T&>(rGetRequiredAPI( type ));
		}

		template<APIConcept API_T>
		const API_T& GetRequiredAPI() const
		{
			constexpr auto type{ API_T::GetType() };
			return dynamic_cast<const API_T&>(GetRequiredAPI( type ));
		}

		inline API::BaseAPI* rGetAPI( const APIType type ) noexcept { return mApis.at( type ).get(); }
		inline const API::BaseAPI* GetAPI( const APIType type ) const noexcept { return mApis.at( type ).get(); }
		inline API::BaseAPI& rGetRequiredAPI( const APIType type )
		{
			if (auto* const api = rGetAPI( type ))
				return *api;

			throw std::runtime_error( "Missing required API" );
		}
		inline const API::BaseAPI& GetRequiredAPI( const APIType type ) const
		{
			if (const auto* const api = GetAPI( type ))
				return *api;

			throw std::runtime_error( "Missing required API" );
		}

		int Dispatch();

	private:
		void InitResources();
		void InitRNG();

		void Shutdown();

		void DoFixedUpdate( const PreciseTimestep& ts );
		void DoVariableUpdate( const PreciseTimestep& ts );
		void DoRender( const PreciseTimestep& ts );

		void PumpEvents( const PreciseTimestep& ts );

		void InitAPIs();
		void ShutdownAPIs();

	private:
		bool mIsInitialised = false;

		int mTargetFps;
		bool mIsRunning = true;
		int mExitCode = -1;

		std::unique_ptr<AbstractGame> mpGame;
		std::unique_ptr<ResourceManager> mpResourceManager;

		const std::function<void( ResourceManager& )> mResourceInitaliserFunc;

		std::vector<std::unique_ptr<API::BaseAPI>> mApis;
		std::vector<API::BaseAPI*> mActiveApis;
	};
}
