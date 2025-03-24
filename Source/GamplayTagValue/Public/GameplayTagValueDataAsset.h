#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTags.h"
#include "TagValue.h"
#include "GameplayTagValueDataAsset.generated.h"

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
    /** Whether to register this data asset to GameplayTagValueSubsystem on game start */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tag Values", meta=(DisplayName="Register to GameplayTagValueSubsystem"))
    bool bRegisterToGameplayTagValueSubsystem;

    /** Datatable to register to GameplayTagValueSubsystem. Each table should use FTagValueMapping as its row structure. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tag Values", meta=(DisplayName="Data Tables"))
    TArray<UDataTable*> DataTables;
    
    /** Repository name to use when registering the data tables. If empty, uses the default repository. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tag Values", meta=(DisplayName="Repository Name"))
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
