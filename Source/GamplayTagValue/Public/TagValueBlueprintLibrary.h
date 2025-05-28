// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTags.h"
#include "TagValueBase.h"
#include "TagValueContainer.h"
#include "TagValueBlueprintLibrary.generated.h"

/**
 * Blueprint Function Library for the TagValue System
 * Provides Blueprint-friendly functions for operating on TagValueContainers
 */
UCLASS()
class GAMPLAYTAGVALUE_API UTagValueBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Boolean value functions

	/** Set a boolean value in the container */
	UFUNCTION(BlueprintCallable, Category = "TagValue")
	static void SetBoolValue(UPARAM(ref) FTagValueContainer& Container, FGameplayTag Tag, bool Value);

	/** Get a boolean value from the container */
	UFUNCTION(BlueprintPure, Category = "TagValue")
	static bool GetBoolValue(const FTagValueContainer& Container, bool& Success, FGameplayTag Tag, bool DefaultValue);

	// Integer value functions

	/** Set an integer value in the container */
	UFUNCTION(BlueprintCallable, Category = "TagValue")
	static void SetIntValue(UPARAM(ref) FTagValueContainer& Container, FGameplayTag Tag, int32 Value);

	/** Get an integer value from the container */
	UFUNCTION(BlueprintPure, Category = "TagValue")
	static int32 GetIntValue(const FTagValueContainer& Container, bool& Success, FGameplayTag Tag, int32 DefaultValue);

	// Float value functions

	/** Set a float value in the container */
	UFUNCTION(BlueprintCallable, Category = "TagValue")
	static void SetFloatValue(UPARAM(ref) FTagValueContainer& Container, FGameplayTag Tag, float Value);

	/** Get a float value from the container */
	UFUNCTION(BlueprintPure, Category = "TagValue")
	static float GetFloatValue(const FTagValueContainer& Container, bool& Success, FGameplayTag Tag, float DefaultValue);

	// String value functions

	/** Set a string value in the container */
	UFUNCTION(BlueprintCallable, Category = "TagValue")
	static void SetStringValue(UPARAM(ref) FTagValueContainer& Container, FGameplayTag Tag, const FString& Value);

	/** Get a string value from the container */
	UFUNCTION(BlueprintPure, Category = "TagValue")
	static FString GetStringValue(const FTagValueContainer& Container, bool& Success, FGameplayTag Tag, const FString& DefaultValue);

	// Transform value functions

	/** Set a transform value in the container */
	UFUNCTION(BlueprintCallable, Category = "TagValue")
	static void SetTransformValue(UPARAM(ref) FTagValueContainer& Container, FGameplayTag Tag, const FTransform& Value);

	/** Get a transform value from the container */
	UFUNCTION(BlueprintPure, Category = "TagValue")
	static FTransform GetTransformValue(const FTagValueContainer& Container, bool& Success, FGameplayTag Tag, const FTransform& DefaultValue);

	// Class value functions

	/** Set a class value in the container */
	UFUNCTION(BlueprintCallable, Category = "TagValue")
	static void SetClassValue(UPARAM(ref) FTagValueContainer& Container, FGameplayTag Tag, TSoftClassPtr<UObject> Value);

	/** Get a class value from the container */
	UFUNCTION(BlueprintPure, Category = "TagValue")
	static TSoftClassPtr<UObject> GetClassValue(const FTagValueContainer& Container, bool& Success, FGameplayTag Tag, TSoftClassPtr<UObject> DefaultValue);

	// Object value functions

	/** Set an object value in the container */
	UFUNCTION(BlueprintCallable, Category = "TagValue")
	static void SetObjectValue(UPARAM(ref) FTagValueContainer& Container, FGameplayTag Tag, TSoftObjectPtr<UObject> Value);

	/** Get an object value from the container */
	UFUNCTION(BlueprintPure, Category = "TagValue")
	static TSoftObjectPtr<UObject> GetObjectValue(const FTagValueContainer& Container, bool& Success, FGameplayTag Tag, TSoftObjectPtr<UObject> DefaultValue);

	// General container functions

	/** Check if the container has a value for the given tag */
	UFUNCTION(BlueprintPure, Category = "TagValue")
	static bool HasTagValue(const FTagValueContainer& Container, FGameplayTag Tag);

	/** Remove a tag value from the container */
	UFUNCTION(BlueprintCallable, Category = "TagValue")
	static bool RemoveTagValue(UPARAM(ref) FTagValueContainer& Container, FGameplayTag Tag);

	/** Get all tags in the container */
	UFUNCTION(BlueprintPure, Category = "TagValue")
	static TArray<FGameplayTag> GetAllTags(const FTagValueContainer& Container);

	/** Clear all values from the container */
	UFUNCTION(BlueprintCallable, Category = "TagValue")
	static void ClearTagValues(UPARAM(ref) FTagValueContainer& Container);
};
