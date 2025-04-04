#include "GamplayTagValue.h"
#include "K2Node_GameplayTagValueAction.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "EdGraphSchema_K2.h"
#include "KismetCompiler.h"
#include "GameplayTagValueSubsystem.h"
#include "GameplayTagValueDataAsset.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

// Plugin settings
#include "ISettingsModule.h"
#include "ISettingsSection.h"

#define LOCTEXT_NAMESPACE "FGamplayTagValueModule"

void FGamplayTagValueModule::StartupModule()
{
	// Register custom blueprint nodes
	RegisterBlueprintNodeFactories();

	// Initialize default repositories
	InitializeDefaultRepositories();

	// Register configured data assets
	RegisterConfiguredDataAssets();
}

void FGamplayTagValueModule::ShutdownModule()
{
	// Unregister custom blueprint nodes
	UnregisterBlueprintNodeFactories();
}

void FGamplayTagValueModule::RegisterBlueprintNodeFactories()
{
	// This function intentionally left empty as K2Node classes register themselves
	// via their GetMenuActions() method when the BlueprintActionDatabase is initialized
}

void FGamplayTagValueModule::UnregisterBlueprintNodeFactories()
{
	// No explicit unregistration needed as K2Node classes handle their own registration
}

void FGamplayTagValueModule::InitializeDefaultRepositories()
{
	// Create additional repositories if needed
	// We'll create a "Config" repository with higher priority than the default
	UGameInstance* GameInstance = nullptr;
	UWorld* World = nullptr;

	// Try to find a valid world and game instance
	if (GEngine)
	{
		if (GEngine->GameViewport)
		{
			World = GEngine->GameViewport->GetWorld();
		}
		else if (GEngine->GetWorldContexts().Num() > 0)
		{
			World = GEngine->GetWorldContexts()[0].World();
		}

		if (World)
		{
			GameInstance = World->GetGameInstance();
		}
	}

	if (GameInstance)
	{
		UGameplayTagValueSubsystem* Subsystem = GameInstance->GetSubsystem<UGameplayTagValueSubsystem>();
		if (Subsystem)
		{
			// Create a "Config" repository with higher priority than the default
			FName ConfigRepositoryName = TEXT("Config");
			TSharedPtr<FMemoryTagValueRepository> ConfigRepository = MakeShared<FMemoryTagValueRepository>(ConfigRepositoryName, 200);
			Subsystem->RegisterRepository(ConfigRepository);
			CreatedRepositoryNames.Add(ConfigRepositoryName);

			// Create a "Runtime" repository with lower priority than the default
			FName RuntimeRepositoryName = TEXT("Runtime");
			TSharedPtr<FMemoryTagValueRepository> RuntimeRepository = MakeShared<FMemoryTagValueRepository>(RuntimeRepositoryName, 50);
			Subsystem->RegisterRepository(RuntimeRepository);
			CreatedRepositoryNames.Add(RuntimeRepositoryName);
		}
	}
}

void FGamplayTagValueModule::RegisterConfiguredDataAssets()
{
	UGameInstance* GameInstance = nullptr;
	UWorld* World = nullptr;

	// Try to find a valid world and game instance
	if (GEngine)
	{
		if (GEngine->GameViewport)
		{
			World = GEngine->GameViewport->GetWorld();
		}
		else if (GEngine->GetWorldContexts().Num() > 0)
		{
			World = GEngine->GetWorldContexts()[0].World();
		}

		if (World)
		{
			GameInstance = World->GetGameInstance();
		}
	}

	if (GameInstance)
	{
		UGameplayTagValueSubsystem* Subsystem = GameInstance->GetSubsystem<UGameplayTagValueSubsystem>();
		if (Subsystem)
		{
			// Register all configured data assets
			int32 RegisteredCount = Subsystem->RegisterConfiguredDataAssets();
			UE_LOG(LogTemp, Log, TEXT("Registered %d tag values from configured data assets"), RegisteredCount);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGamplayTagValueModule, GamplayTagValue)