#pragma once
#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "TagValue.generated.h"

USTRUCT(BlueprintType)
struct FTagValue
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="BoolValue", MakeStructureDefaultValue="False"))
	bool BoolValue;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="IntegerValue", MakeStructureDefaultValue="0"))
	int32 IntegerValue;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="FloatValue", MakeStructureDefaultValue="0.000000"))
	double FloatValue;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="TransformValue", MakeStructureDefaultValue="0.000000,0.000000,0.000000|0.000000,0.000000,0.000000|1.000000,1.000000,1.000000"))
	FTransform TransformValue;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ClassValue", MakeStructureDefaultValue="None"))
	FSoftClassPath ClassValue;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ObjectValue", MakeStructureDefaultValue="None"))
	FSoftObjectPath ObjectValue;
};

/** Please add a struct description */
USTRUCT(BlueprintType)
struct FTagValueMapping
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Tag"))
	FGameplayTag Tag;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Value"))
	FTagValue Value;
};
