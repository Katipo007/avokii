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

		std::function<void( ResourceManager& )> resource_initaliser_func;

		unsigned max_plugins = 0;
		std::function<std::unique_ptr<API::BaseAPI>( Core&, APIType )> plugin_factory;

		bool IsValid() const noexcept
		{
			return true
				&& (fps >= 0)
				&& (max_plugins >= CoreAPIs::Type::User)
				&& (bool)resource_initaliser_func
				&& (bool)plugin_factory
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
		API_T* GetAPI() noexcept { return const_cast<API_T*>(((const Core*)this)->GetAPI<API_T>()); }

		template<APIConcept API_T>
		const API_T* GetAPI() const noexcept
		{
			constexpr auto type{ API_T::GetType() };
			return dynamic_cast<const API_T*>(GetAPI( type ));
		}

		template<APIConcept API_T>
		API_T& GetRequiredAPI() noexcept { return const_cast<API_T&>(((const Core*)this)->GetRequiredAPI<API_T>()); }

		template<APIConcept API_T>
		const API_T& GetRequiredAPI() const noexcept
		{
			constexpr auto type{ API_T::GetType() };
			return dynamic_cast<const API_T&>(GetRequiredAPI( type ));
		}

		inline API::BaseAPI* GetAPI( const APIType type ) noexcept;
		inline const API::BaseAPI* GetAPI( const APIType type ) const noexcept;
		inline API::BaseAPI& rGetRequiredAPI( const APIType type );
		inline const API::BaseAPI& GetRequiredAPI( const APIType type ) const;

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
