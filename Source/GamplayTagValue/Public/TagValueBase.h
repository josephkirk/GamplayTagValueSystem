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

	FBaseTagValue() {}
	virtual ~FBaseTagValue() {}

	// Virtual function to identify the type
	virtual FName GetValueType() const { return NAME_None; }

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Value;

	FBoolTagValue() : Value(false) {}
	FBoolTagValue(bool InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Bool"); }
};

/**
 * Integer tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FIntTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value;

	FIntTagValue() : Value(0) {}
	FIntTagValue(int32 InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Int"); }
};

/**
 * Float tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FFloatTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;

	FFloatTagValue() : Value(0.0f) {}
	FFloatTagValue(float InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Float"); }
};

/**
 * Transform tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FTransformTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Value;

	FTransformTagValue() : Value(FTransform::Identity) {}
	FTransformTagValue(const FTransform& InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Transform"); }
};

/**
 * Class tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FClassTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UObject> Value;

	FClassTagValue() {}
	FClassTagValue(const TSoftClassPtr<UObject>& InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Class"); }
};

/**
 * Object tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FObjectTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UObject> Value;

	FObjectTagValue() {}
	FObjectTagValue(const TSoftObjectPtr<UObject>& InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("Object"); }
};

/**
 * String tag value
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FStringTagValue : public FBaseTagValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Value;

	FStringTagValue() {}
	FStringTagValue(const FString& InValue) : Value(InValue) {}

	virtual FName GetValueType() const override { return FName("String"); }
};
