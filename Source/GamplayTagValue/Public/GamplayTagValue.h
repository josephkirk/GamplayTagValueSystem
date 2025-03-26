


#pragma once

#include "Modules/ModuleManager.h"

class FGamplayTagValueModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Register the custom Blueprint nodes */
	void RegisterBlueprintNodeFactories();

	/** Unregister the custom Blueprint nodes */
	void UnregisterBlueprintNodeFactories();

	/** Initialize default repositories */
	void InitializeDefaultRepositories();

	/** Register any configured data assets */
	void RegisterConfiguredDataAssets();

	/** Get the module instance */
	static FGamplayTagValueModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FGamplayTagValueModule>("GamplayTagValue");
	}

	/** Check if the module is loaded */
	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("GamplayTagValue");
	}

private:
	/** Handle to the registered BlueprintNodeFactories delegate */
	FDelegateHandle BlueprintNodeFactoriesHandle;

	/** Names of repositories created by this module */
	TArray<FName> CreatedRepositoryNames;
};
