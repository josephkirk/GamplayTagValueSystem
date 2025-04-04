#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTags.h"
#include "TagValueBase.h"
#include "TagValueTypes.h"
#include "GameplayTagValueDataAsset.generated.h"

/**
 * Data table row structure for storing tag-value pairs with dynamic value types.
 * This is used by the GameplayTagValueDataAsset to define tag values in data tables.
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FTagValueDataTableRow : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    /** The gameplay tag this value is associated with */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values")
    FGameplayTag Tag;

    /** Boolean value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values", meta=(EditCondition="ValueType == ETagValueType::Bool", EditConditionHides))
    bool BoolValue = false;

    /** Integer value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values", meta=(EditCondition="ValueType == ETagValueType::Int", EditConditionHides))
    int32 IntValue = 0;

    /** Float value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values", meta=(EditCondition="ValueType == ETagValueType::Float", EditConditionHides))
    float FloatValue = 0.0f;

    /** String value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values", meta=(EditCondition="ValueType == ETagValueType::String", EditConditionHides))
    FString StringValue;

    /** Transform value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values", meta=(EditCondition="ValueType == ETagValueType::Transform", EditConditionHides))
    FTransform TransformValue;

    /** Class value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values", meta=(EditCondition="ValueType == ETagValueType::Class", EditConditionHides))
    TSoftClassPtr<UObject> ClassValue;

    /** Object value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values", meta=(EditCondition="ValueType == ETagValueType::Object", EditConditionHides))
    TSoftObjectPtr<UObject> ObjectValue;

    /** Type of value to use */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Values")
    ETagValueType ValueType = ETagValueType::Bool;

    /** Creates a shared pointer to the appropriate FBaseTagValue subclass based on the ValueType */
    TSharedPtr<FBaseTagValue> CreateTagValue() const
    {
        switch (ValueType)
        {
        case ETagValueType::Bool:
            return MakeShared<FBoolTagValue>(BoolValue);
        case ETagValueType::Int:
            return MakeShared<FIntTagValue>(IntValue);
        case ETagValueType::Float:
            return MakeShared<FFloatTagValue>(FloatValue);
        case ETagValueType::String:
            return MakeShared<FStringTagValue>(StringValue);
        case ETagValueType::Transform:
            return MakeShared<FTransformTagValue>(TransformValue);
        case ETagValueType::Class:
            return MakeShared<FClassTagValue>(ClassValue);
        case ETagValueType::Object:
            return MakeShared<FObjectTagValue>(ObjectValue);
        default:
            return nullptr;
        }
    }
};

/**
 * Data asset that stores tag-value pairs via datatables for the GameplayTagValue system.
 * Can be configured to automatically register its data tables to the GameplayTagValueSubsystem on game start.
 * Use this to define sets of tag values that can be loaded and registered to the system.
 */
UCLASS(BlueprintType)
class GAMPLAYTAGVALUE_API UGameplayTagValueDataAsset : public UDataAsset
{
    GENERATED_BODY()
public:
    /** Whether to auto-register this data asset to GameplayTagValueSubsystem on game start */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tag Values")
    bool bAutoRegister = false;

    /** Priority of this data asset when registering to the subsystem (higher priorities will override lower) */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tag Values")
    int32 Priority = 100;

    /** Datatable to register to GameplayTagValueSubsystem. Each table should use FTagValueDataTableRow as its row structure. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tag Values")
    TArray<UDataTable*> DataTables;
    
    /** Repository name to use when registering the data tables. If empty, uses the default repository. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tag Values")
    FName RepositoryName;
    
    /** 
     * Manually register this data asset's tables to the GameplayTagValueSubsystem.
     * @param Subsystem The GameplayTagValueSubsystem to register with. If null, will try to find it from the game instance.
     * @return Number of tag-value pairs successfully imported
     */
    UFUNCTION(BlueprintCallable, Category="Tag Values")
    int32 RegisterToSubsystem(class UGameplayTagValueSubsystem* Subsystem = nullptr);
    
    /** 
     * Unregister this data asset's values from the GameplayTagValueSubsystem.
     * @param Subsystem The GameplayTagValueSubsystem to unregister from. If null, will try to find it from the game instance.
     * @param bClearAllValues If true, clears all values in the repository. If false, only removes values defined in the data tables.
     */
    UFUNCTION(BlueprintCallable, Category="Tag Values")
    void UnregisterFromSubsystem(class UGameplayTagValueSubsystem* Subsystem = nullptr, bool bClearAllValues = false);
    
    // UDataAsset interface
    virtual void PostLoad() override;
    
#if WITH_EDITOR
    /** Validates that all data tables use the correct row structure */
    virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif
};
