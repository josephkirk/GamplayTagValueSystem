// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "TagValueBase.generated.h"

/**
 * Base struct for all tag value types
 * This provides type-safe inheritance for different value types
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FBaseTagValue
{
	GENERATED_BODY()

	/** The gameplay tag this value is associated with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values")
	FGameplayTag Tag;

	FBaseTagValue() {}
	virtual ~FBaseTagValue() {}

	// Virtual function to identify the type
	virtual FName GetValueType() const { return NAME_None; }

	// Try to cast to a specific type
	template<typename T>
	const T* TryCast() const
	{
		if (GetValueType() == T::StaticValueType())
		{
			return static_cast<const T*>(this);
		}
		return nullptr;
	}

	// Serialization support
	bool Serialize(FArchive& Ar);
	bool operator==(const FBaseTagValue& Other) const;
	friend FArchive& operator<<(FArchive& Ar, FBaseTagValue& Value)
	{
		Value.Serialize(Ar);
		return Ar;
	}
};

/**
 * Boolean tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FBoolTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values")
	bool Value;

	FBoolTagValue() : Value(false) {}
	FBoolTagValue(bool InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Bool"); }

	static FName StaticValueType() { return FName("Bool"); }
};

/**
 * Integer tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FIntTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values")
	int32 Value;

	FIntTagValue() : Value(0) {}
	FIntTagValue(int32 InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Int"); }

	static FName StaticValueType() { return FName("Int"); }
};

/**
 * Float tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FFloatTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values")
	float Value;

	FFloatTagValue() : Value(0.0f) {}
	FFloatTagValue(float InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Float"); }

	static FName StaticValueType() { return FName("Float"); }
};

/**
 * Transform tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FTransformTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values")
	FTransform Value;

	FTransformTagValue() : Value(FTransform::Identity) {}
	FTransformTagValue(const FTransform& InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Transform"); }

	static FName StaticValueType() { return FName("Transform"); }
};

/**
 * Class tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FClassTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values")
	TSoftClassPtr<UObject> Value;

	FClassTagValue() {}
	FClassTagValue(const TSoftClassPtr<UObject>& InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Class"); }

	static FName StaticValueType() { return FName("Class"); }
};

/**
 * Object tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FObjectTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values")
	TSoftObjectPtr<UObject> Value;

	FObjectTagValue() {}
	FObjectTagValue(const TSoftObjectPtr<UObject>& InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Object"); }

	static FName StaticValueType() { return FName("Object"); }
};

/**
 * String tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FStringTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values")
	FString Value;

	FStringTagValue() {}
	FStringTagValue(const FString& InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("String"); }

	static FName StaticValueType() { return FName("String"); }
};
