#include "TagValueRepositoryComponent.h"
#include "GameplayTagValueSubsystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTagValueRepositoryComponent::UTagValueRepositoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	PrimaryComponentTick.bCanEverTick = false;

	// Default values
	RepositoryName = FName(TEXT("ActorComponentRepository"));
	Priority = 100;
	bRegisterToSubsystem = true;
	bIsRegistered = false;
}

void UTagValueRepositoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Auto-register with the subsystem if configured to do so
	if (bRegisterToSubsystem)
	{
		RegisterWithSubsystem();
	}
}

void UTagValueRepositoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from the subsystem when the component is destroyed
	if (bIsRegistered)
	{
		UnregisterFromSubsystem();
	}

	Super::EndPlay(EndPlayReason);
}

void UTagValueRepositoryComponent::RegisterWithSubsystem()
{
	if (bIsRegistered)
	{
		return; // Already registered
	}

	// Get the subsystem
	UGameplayTagValueSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UGameplayTagValueSubsystem>();
	if (Subsystem)
	{
		// Register this component as a repository
		TSharedPtr<ITagValueRepository> ThisRepository(this);
		Subsystem->RegisterRepository(ThisRepository);
		bIsRegistered = true;
	}
}

void UTagValueRepositoryComponent::UnregisterFromSubsystem()
{
	if (!bIsRegistered)
	{
		return; // Not registered
	}

	// Get the subsystem
	UGameplayTagValueSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UGameplayTagValueSubsystem>();
	if (Subsystem)
	{
		// Unregister this component as a repository
		Subsystem->UnregisterRepository(GetRepositoryName());
		bIsRegistered = false;
	}
}

// ITagValueRepository interface implementation
bool UTagValueRepositoryComponent::HasValue(FGameplayTag Tag) const
{
	// Check if the tag exists in our mappings
	for (const FTagValueMapping& Mapping : TagValueMappings)
	{
		if (Mapping.Tag == Tag)
		{
			return true;
		}
	}
	return false;
}

TSharedPtr<ITagValueHolder> UTagValueRepositoryComponent::GetValue(FGameplayTag Tag) const
{
	// Find the tag in our mappings
	for (const FTagValueMapping& Mapping : TagValueMappings)
	{
		if (Mapping.Tag == Tag)
		{
			// Convert the FTagValue to an ITagValueHolder
			return ConvertTagValueToHolder(Mapping.Value);
		}
	}
	return nullptr;
}

void UTagValueRepositoryComponent::SetValue(FGameplayTag Tag, TSharedPtr<ITagValueHolder> Value)
{
	if (!Value)
	{
		// Remove the value if nullptr is provided
		RemoveValue(Tag);
		return;
	}

	// Convert the ITagValueHolder to an FTagValue
	FTagValue TagValue;
	if (ConvertHolderToTagValue(Value, TagValue))
	{
		// Update or add the mapping
		SetTagValueMapping(Tag, TagValue);
	}
}

void UTagValueRepositoryComponent::RemoveValue(FGameplayTag Tag)
{
	RemoveTagValueMapping(Tag);
}

void UTagValueRepositoryComponent::ClearAllValues()
{
	ClearTagValueMappings();
}

TArray<FGameplayTag> UTagValueRepositoryComponent::GetAllTags() const
{
	TArray<FGameplayTag> Tags;
	for (const FTagValueMapping& Mapping : TagValueMappings)
	{
		Tags.Add(Mapping.Tag);
	}
	return Tags;
}

FName UTagValueRepositoryComponent::GetRepositoryName() const
{
	return RepositoryName;
}

int32 UTagValueRepositoryComponent::GetPriority() const
{
	return Priority;
}

void UTagValueRepositoryComponent::SetTagValueMapping(FGameplayTag InTag, FTagValue InValue)
{
	// Check if the tag already exists
	for (int32 i = 0; i < TagValueMappings.Num(); ++i)
	{
		if (TagValueMappings[i].Tag == InTag)
		{
			// Update the existing mapping
			TagValueMappings[i].Value = InValue;
			return;
		}
	}

	// Add a new mapping
	FTagValueMapping NewMapping;
	NewMapping.Tag = InTag;
	NewMapping.Value = InValue;
	TagValueMappings.Add(NewMapping);
}

void UTagValueRepositoryComponent::RemoveTagValueMapping(FGameplayTag InTag)
{
	// Find and remove the mapping
	for (int32 i = 0; i < TagValueMappings.Num(); ++i)
	{
		if (TagValueMappings[i].Tag == InTag)
		{
			TagValueMappings.RemoveAt(i);
			break;
		}
	}
}

void UTagValueRepositoryComponent::ClearTagValueMappings()
{
	TagValueMappings.Empty();
}

TSharedPtr<ITagValueHolder> UTagValueRepositoryComponent::ConvertTagValueToHolder(const FTagValue& Value) const
{
	// Create a new holder for the appropriate type
	// This is a simple implementation that just uses the bool value
	// A more complete implementation would check which field is set and use that
	return MakeShared<TTagValueHolder<bool>>(Value.BoolValue);

	// For a more complete implementation, you would need to determine which value is actually set
	// and create the appropriate holder type. This might involve additional metadata in FTagValue
	// to indicate which field is the active one.
}

bool UTagValueRepositoryComponent::ConvertHolderToTagValue(const TSharedPtr<ITagValueHolder>& Holder, FTagValue& OutValue) const
{
	if (!Holder || !Holder->IsValid())
	{
		return false;
	}

	// Check the type of the holder and extract the appropriate value
	FName TypeName = Holder->GetValueTypeName();

	// Handle bool values
	if (TypeName == TBaseStructure<bool>::Get()->GetFName())
	{
		OutValue.BoolValue = *static_cast<bool*>(Holder->GetValuePtr());
		return true;
	}
	// Handle int32 values
	else if (TypeName == TBaseStructure<int32>::Get()->GetFName())
	{
		OutValue.IntegerValue = *static_cast<int32*>(Holder->GetValuePtr());
		return true;
	}
	// Handle double/float values
	else if (TypeName == TBaseStructure<double>::Get()->GetFName())
	{
		OutValue.FloatValue = *static_cast<double*>(Holder->GetValuePtr());
		return true;
	}
	// Handle transform values
	else if (TypeName == TBaseStructure<FTransform>::Get()->GetFName())
	{
		OutValue.TransformValue = *static_cast<FTransform*>(Holder->GetValuePtr());
		return true;
	}
	// Handle class path values
	else if (TypeName == TBaseStructure<FSoftClassPath>::Get()->GetFName())
	{
		OutValue.ClassValue = *static_cast<FSoftClassPath*>(Holder->GetValuePtr());
		return true;
	}
	// Handle object path values
	else if (TypeName == TBaseStructure<FSoftObjectPath>::Get()->GetFName())
	{
		OutValue.ObjectValue = *static_cast<FSoftObjectPath*>(Holder->GetValuePtr());
		return true;
	}

	// Unknown type
	return false;
}
