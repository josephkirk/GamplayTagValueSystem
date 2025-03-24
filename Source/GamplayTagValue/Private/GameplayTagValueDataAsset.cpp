#include "GameplayTagValueDataAsset.h"
#include "GameplayTagValueSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UGameplayTagValueDataAsset::PostLoad()
{
    Super::PostLoad();
    
    // Auto-register to the subsystem if configured to do so
    if (bRegisterToGameplayTagValueSubsystem)
    {
        // We need to use a delay here because the GameInstance might not be fully initialized yet
        // This is handled by the GameplayTagValueSubsystem itself during initialization
        // The subsystem will look for all UGameplayTagValueDataAsset objects and register them
    }
}

int32 UGameplayTagValueDataAsset::RegisterToSubsystem(UGameplayTagValueSubsystem* Subsystem)
{
    int32 TotalImported = 0;
    
    // If no subsystem provided, try to find it
    if (!Subsystem)
    {
        UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
        if (GameInstance)
        {
            Subsystem = GameInstance->GetSubsystem<UGameplayTagValueSubsystem>();
        }
    }
    
    // If we have a valid subsystem, register all data tables
    if (Subsystem)
    {
        for (UDataTable* DataTable : DataTables)
        {
            if (DataTable)
            {
                TotalImported += Subsystem->ImportFromDataTable(DataTable, RepositoryName);
            }
        }
    }
    
    return TotalImported;
}

void UGameplayTagValueDataAsset::UnregisterFromSubsystem(UGameplayTagValueSubsystem* Subsystem, bool bClearAllValues)
{
    // If no subsystem provided, try to find it
    if (!Subsystem)
    {
        UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
        if (GameInstance)
        {
            Subsystem = GameInstance->GetSubsystem<UGameplayTagValueSubsystem>();
        }
    }
    
    // If we have a valid subsystem, unregister
    if (Subsystem)
    {
        if (bClearAllValues)
        {
            // Clear all values in the repository
            Subsystem->ClearAllValues(RepositoryName);
        }
        else
        {
            // Only remove values defined in the data tables
            for (UDataTable* DataTable : DataTables)
            {
                if (DataTable)
                {
                    // Get all rows from the data table
                    TArray<FName> RowNames = DataTable->GetRowNames();
                    for (const FName& RowName : RowNames)
                    {
                        FTagValueMapping* Mapping = DataTable->FindRow<FTagValueMapping>(RowName, TEXT(""));
                        if (Mapping && Mapping->Tag.IsValid())
                        {
                            // Remove this specific tag value
                            Subsystem->RemoveTagValue(Mapping->Tag, RepositoryName);
                        }
                    }
                }
            }
        }
    }
}

#if WITH_EDITOR
EDataValidationResult UGameplayTagValueDataAsset::IsDataValid(TArray<FText>& ValidationErrors)
{
    EDataValidationResult Result = Super::IsDataValid(ValidationErrors);
    
    // Check that all data tables use the correct row structure
    for (int32 Index = 0; Index < DataTables.Num(); ++Index)
    {
        UDataTable* DataTable = DataTables[Index];
        if (DataTable)
        {
            UScriptStruct* RowStruct = DataTable->GetRowStruct();
            if (!RowStruct || RowStruct->GetFName() != FTagValueMapping::StaticStruct()->GetFName())
            {
                ValidationErrors.Add(FText::Format(FText::FromString(TEXT("Data table at index {0} does not use FTagValueMapping as its row structure")), FText::AsNumber(Index)));
                Result = EDataValidationResult::Invalid;
            }
        }
    }
    
    return Result;
}
#endif
