// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.
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
	return TagValueContainer.HasValue(Tag);
}

TSharedPtr<ITagValueHolder> UTagValueRepositoryComponent::GetValue(FGameplayTag Tag) const
{
	if (TagValueContainer.HasValue(Tag))
	{
		// Find the tag in our container
		const TSharedPtr<FBaseTagValue>* BaseValue = TagValueContainer.Values.Find(Tag);
		if (BaseValue && BaseValue->IsValid())
		{
			// Convert the FBaseTagValue to an ITagValueHolder
			return ConvertTagValueToHolder(*BaseValue);
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

	// Convert the ITagValueHolder to an FBaseTagValue and add to container
	TSharedPtr<FBaseTagValue> TagValue = ConvertHolderToTagValue(Value);
	if (TagValue.IsValid())
	{
		TagValueContainer.Values.Add(Tag, TagValue);
	}
}

void UTagValueRepositoryComponent::RemoveValue(FGameplayTag Tag)
{
	TagValueContainer.RemoveValue(Tag);
}

void UTagValueRepositoryComponent::ClearAllValues()
{
	TagValueContainer.Clear();
}

TArray<FGameplayTag> UTagValueRepositoryComponent::GetAllTags() const
{
	return TagValueContainer.GetAllTags();
}

FName UTagValueRepositoryComponent::GetRepositoryName() const
{
	return RepositoryName;
}

int32 UTagValueRepositoryComponent::GetPriority() const
{
	return Priority;
}

void UTagValueRepositoryComponent::SetBoolTagValue(FGameplayTag InTag, bool InValue)
{
	TagValueContainer.SetValue<FBoolTagValue>(InTag, FBoolTagValue(InValue));
}

void UTagValueRepositoryComponent::SetIntTagValue(FGameplayTag InTag, int32 InValue)
{
	TagValueContainer.SetValue<FIntTagValue>(InTag, FIntTagValue(InValue));
}

void UTagValueRepositoryComponent::SetFloatTagValue(FGameplayTag InTag, float InValue)
{
	TagValueContainer.SetValue<FFloatTagValue>(InTag, FFloatTagValue(InValue));
}

void UTagValueRepositoryComponent::SetStringTagValue(FGameplayTag InTag, const FString& InValue)
{
	TagValueContainer.SetValue<FStringTagValue>(InTag, FStringTagValue(InValue));
}

void UTagValueRepositoryComponent::SetTransformTagValue(FGameplayTag InTag, const FTransform& InValue)
{
	TagValueContainer.SetValue<FTransformTagValue>(InTag, FTransformTagValue(InValue));
}

void UTagValueRepositoryComponent::SetClassTagValue(FGameplayTag InTag, TSoftClassPtr<UObject> InValue)
{
	TagValueContainer.SetValue<FClassTagValue>(InTag, FClassTagValue(InValue));
}

void UTagValueRepositoryComponent::SetObjectTagValue(FGameplayTag InTag, TSoftObjectPtr<UObject> InValue)
{
	TagValueContainer.SetValue<FObjectTagValue>(InTag, FObjectTagValue(InValue));
}

void UTagValueRepositoryComponent::RemoveTagValue(FGameplayTag InTag)
{
	TagValueContainer.RemoveValue(InTag);
}

void UTagValueRepositoryComponent::ClearTagValues()
{
	TagValueContainer.Clear();
}

TSharedPtr<ITagValueHolder> UTagValueRepositoryComponent::ConvertTagValueToHolder(TSharedPtr<FBaseTagValue> Value) const
{
	if (!Value.IsValid())
	{
		return nullptr;
	}

	// Get the type name to determine which holder to create
	FName TypeName = Value->GetValueType();

	// Create the appropriate holder based on the type
	if (TypeName == FName("Bool"))
	{
		FBoolTagValue* TypedValue = static_cast<FBoolTagValue*>(Value.Get());
		return MakeShared<TTagValueHolder<bool>>(TypedValue->Value);
	}
	else if (TypeName == FName("Int"))
	{
		FIntTagValue* TypedValue = static_cast<FIntTagValue*>(Value.Get());
		return MakeShared<TTagValueHolder<int32>>(TypedValue->Value);
	}
	else if (TypeName == FName("Float"))
	{
		FFloatTagValue* TypedValue = static_cast<FFloatTagValue*>(Value.Get());
		return MakeShared<TTagValueHolder<float>>(TypedValue->Value);
	}
	else if (TypeName == FName("String"))
	{
		FStringTagValue* TypedValue = static_cast<FStringTagValue*>(Value.Get());
		return MakeShared<TTagValueHolder<FString>>(TypedValue->Value);
	}
	else if (TypeName == FName("Transform"))
	{
		FTransformTagValue* TypedValue = static_cast<FTransformTagValue*>(Value.Get());
		return MakeShared<TTagValueHolder<FTransform>>(TypedValue->Value);
	}
	else if (TypeName == FName("Class"))
	{
		FClassTagValue* TypedValue = static_cast<FClassTagValue*>(Value.Get());
		return MakeShared<TTagValueHolder<TSoftClassPtr<UObject>>>(TypedValue->Value);
	}
	else if (TypeName == FName("Object"))
	{
		FObjectTagValue* TypedValue = static_cast<FObjectTagValue*>(Value.Get());
		return MakeShared<TTagValueHolder<TSoftObjectPtr<UObject>>>(TypedValue->Value);
	}

	return nullptr;
}

TSharedPtr<FBaseTagValue> UTagValueRepositoryComponent::ConvertHolderToTagValue(const TSharedPtr<ITagValueHolder>& Holder) const
{
	if (!Holder || !Holder->IsValid())
	{
		return nullptr;
	}

	// Check the type of the holder and create the appropriate tag value
	FName TypeName = Holder->GetValueTypeName();

	// Handle bool values
	if (TypeName == TBaseStructure<bool>::Get()->GetFName())
	{
		bool* Value = static_cast<bool*>(Holder->GetValuePtr());
		return MakeShared<FBoolTagValue>(*Value);
	}
	// Handle int32 values
	else if (TypeName == TBaseStructure<int32>::Get()->GetFName())
	{
		int32* Value = static_cast<int32*>(Holder->GetValuePtr());
		return MakeShared<FIntTagValue>(*Value);
	}
	// Handle float values
	else if (TypeName == TBaseStructure<float>::Get()->GetFName())
	{
		float* Value = static_cast<float*>(Holder->GetValuePtr());
		return MakeShared<FFloatTagValue>(*Value);
	}
	// Handle double values and convert to float
	else if (TypeName == TBaseStructure<double>::Get()->GetFName())
	{
		double* Value = static_cast<double*>(Holder->GetValuePtr());
		return MakeShared<FFloatTagValue>(static_cast<float>(*Value));
	}
	// Handle FString values
	else if (TypeName == TBaseStructure<FString>::Get()->GetFName())
	{
		FString* Value = static_cast<FString*>(Holder->GetValuePtr());
		return MakeShared<FStringTagValue>(*Value);
	}
	// Handle transform values
	else if (TypeName == TBaseStructure<FTransform>::Get()->GetFName())
	{
		FTransform* Value = static_cast<FTransform*>(Holder->GetValuePtr());
		return MakeShared<FTransformTagValue>(*Value);
	}
	// Handle class path values
	else if (TypeName == TBaseStructure<TSoftClassPtr<UObject>>::Get()->GetFName() || 
			 TypeName == TBaseStructure<FSoftClassPath>::Get()->GetFName())
	{
		auto* Value = static_cast<TSoftClassPtr<UObject>*>(Holder->GetValuePtr());
		return MakeShared<FClassTagValue>(*Value);
	}
	// Handle object path values
	else if (TypeName == TBaseStructure<TSoftObjectPtr<UObject>>::Get()->GetFName() || 
			 TypeName == TBaseStructure<FSoftObjectPath>::Get()->GetFName())
	{
		auto* Value = static_cast<TSoftObjectPtr<UObject>*>(Holder->GetValuePtr());
		return MakeShared<FObjectTagValue>(*Value);
	}

	// Unknown type
	return nullptr;
}
