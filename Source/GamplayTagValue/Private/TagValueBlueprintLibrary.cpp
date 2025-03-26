#include "TagValueBlueprintLibrary.h"

// Boolean value functions
void UTagValueBlueprintLibrary::SetBoolValue(FTagValueContainer& Container, FGameplayTag Tag, bool Value)
{
	Container.SetValue<FBoolTagValue>(Tag, FBoolTagValue(Value));
}

bool UTagValueBlueprintLibrary::GetBoolValue(const FTagValueContainer& Container, FGameplayTag Tag, bool DefaultValue, bool& Success)
{
	FBoolTagValue TypedValue;
	Success = Container.GetValue<FBoolTagValue>(Tag, TypedValue);
	return Success ? TypedValue.Value : DefaultValue;
}

// Integer value functions
void UTagValueBlueprintLibrary::SetIntValue(FTagValueContainer& Container, FGameplayTag Tag, int32 Value)
{
	Container.SetValue<FIntTagValue>(Tag, FIntTagValue(Value));
}

int32 UTagValueBlueprintLibrary::GetIntValue(const FTagValueContainer& Container, FGameplayTag Tag, int32 DefaultValue, bool& Success)
{
	FIntTagValue TypedValue;
	Success = Container.GetValue<FIntTagValue>(Tag, TypedValue);
	return Success ? TypedValue.Value : DefaultValue;
}

// Float value functions
void UTagValueBlueprintLibrary::SetFloatValue(FTagValueContainer& Container, FGameplayTag Tag, float Value)
{
	Container.SetValue<FFloatTagValue>(Tag, FFloatTagValue(Value));
}

float UTagValueBlueprintLibrary::GetFloatValue(const FTagValueContainer& Container, FGameplayTag Tag, float DefaultValue, bool& Success)
{
	FFloatTagValue TypedValue;
	Success = Container.GetValue<FFloatTagValue>(Tag, TypedValue);
	return Success ? TypedValue.Value : DefaultValue;
}

// String value functions
void UTagValueBlueprintLibrary::SetStringValue(FTagValueContainer& Container, FGameplayTag Tag, const FString& Value)
{
	Container.SetValue<FStringTagValue>(Tag, FStringTagValue(Value));
}

FString UTagValueBlueprintLibrary::GetStringValue(const FTagValueContainer& Container, FGameplayTag Tag, const FString& DefaultValue, bool& Success)
{
	FStringTagValue TypedValue;
	Success = Container.GetValue<FStringTagValue>(Tag, TypedValue);
	return Success ? TypedValue.Value : DefaultValue;
}

// Transform value functions
void UTagValueBlueprintLibrary::SetTransformValue(FTagValueContainer& Container, FGameplayTag Tag, const FTransform& Value)
{
	Container.SetValue<FTransformTagValue>(Tag, FTransformTagValue(Value));
}

FTransform UTagValueBlueprintLibrary::GetTransformValue(const FTagValueContainer& Container, FGameplayTag Tag, const FTransform& DefaultValue, bool& Success)
{
	FTransformTagValue TypedValue;
	Success = Container.GetValue<FTransformTagValue>(Tag, TypedValue);
	return Success ? TypedValue.Value : DefaultValue;
}

// Class value functions
void UTagValueBlueprintLibrary::SetClassValue(FTagValueContainer& Container, FGameplayTag Tag, TSoftClassPtr<UObject> Value)
{
	Container.SetValue<FClassTagValue>(Tag, FClassTagValue(Value));
}

TSoftClassPtr<UObject> UTagValueBlueprintLibrary::GetClassValue(const FTagValueContainer& Container, FGameplayTag Tag, TSoftClassPtr<UObject> DefaultValue, bool& Success)
{
	FClassTagValue TypedValue;
	Success = Container.GetValue<FClassTagValue>(Tag, TypedValue);
	return Success ? TypedValue.Value : DefaultValue;
}

// Object value functions
void UTagValueBlueprintLibrary::SetObjectValue(FTagValueContainer& Container, FGameplayTag Tag, TSoftObjectPtr<UObject> Value)
{
	Container.SetValue<FObjectTagValue>(Tag, FObjectTagValue(Value));
}

TSoftObjectPtr<UObject> UTagValueBlueprintLibrary::GetObjectValue(const FTagValueContainer& Container, FGameplayTag Tag, TSoftObjectPtr<UObject> DefaultValue, bool& Success)
{
	FObjectTagValue TypedValue;
	Success = Container.GetValue<FObjectTagValue>(Tag, TypedValue);
	return Success ? TypedValue.Value : DefaultValue;
}

// General container functions
bool UTagValueBlueprintLibrary::HasTagValue(const FTagValueContainer& Container, FGameplayTag Tag)
{
	return Container.HasValue(Tag);
}

bool UTagValueBlueprintLibrary::RemoveTagValue(FTagValueContainer& Container, FGameplayTag Tag)
{
	return Container.RemoveValue(Tag);
}

TArray<FGameplayTag> UTagValueBlueprintLibrary::GetAllTags(const FTagValueContainer& Container)
{
	return Container.GetAllTags();
}

void UTagValueBlueprintLibrary::ClearTagValues(FTagValueContainer& Container)
{
	Container.Clear();
}
