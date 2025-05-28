// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.
#include "GameplayTagValueDataAsset.h"
#include "GameplayTagValueSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UGameplayTagValueDataAsset::PostLoad()
{
    Super::PostLoad();
    
    // Auto-register to the subsystem if configured to do so
    if (bAutoRegister)
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
                // Go through each row in the table and register its value
                int32 ImportedCount = 0;
                
                for (const auto& Pair : DataTable->GetRowMap())
                {
                    FName RowName = Pair.Key;
                    FTagValueDataTableRow* Row = reinterpret_cast<FTagValueDataTableRow*>(Pair.Value);
                    
                    if (Row && Row->Tag.IsValid())
                    {
                        // Create the appropriate tag value based on the row's type
                        TSharedPtr<FBaseTagValue> TagValue = Row->CreateTagValue();
                        
                        if (TagValue.IsValid())
                        {
                            // Create a tag value holder and register it with the appropriate type
                            bool bSuccess = false;
                            
                            switch (Row->ValueType)
                            {
                            case ETagValueType::Bool:
                                bSuccess = Subsystem->SetBoolValue(Row->Tag, Row->BoolValue, RepositoryName);
                                break;
                            case ETagValueType::Int:
                                bSuccess = Subsystem->SetIntValue(Row->Tag, Row->IntValue, RepositoryName);
                                break;
                            case ETagValueType::Float:
                                bSuccess = Subsystem->SetFloatValue(Row->Tag, Row->FloatValue, RepositoryName);
                                break;
                            case ETagValueType::String:
                                bSuccess = Subsystem->SetStringValue(Row->Tag, Row->StringValue, RepositoryName);
                                break;
                            case ETagValueType::Transform:
                                bSuccess = Subsystem->SetTransformValue(Row->Tag, Row->TransformValue, RepositoryName);
                                break;
                            case ETagValueType::Class:
                                bSuccess = Subsystem->SetClassValue(Row->Tag, Row->ClassValue, RepositoryName);
                                break;
                            case ETagValueType::Object:
                                bSuccess = Subsystem->SetObjectValue(Row->Tag, Row->ObjectValue, RepositoryName);
                                break;
                            }
                            
                            if (bSuccess)
                            {
                                ImportedCount++;
                            }
                        }
                    }
                }
                
                TotalImported += ImportedCount;
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
                        FTagValueDataTableRow* Row = DataTable->FindRow<FTagValueDataTableRow>(RowName, TEXT(""));
                        if (Row && Row->Tag.IsValid())
                        {
                            // Remove this specific tag value
                            Subsystem->RemoveTagValue(Row->Tag, RepositoryName);
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
            if (!RowStruct || RowStruct->GetFName() != FTagValueDataTableRow::StaticStruct()->GetFName())
            {
                ValidationErrors.Add(FText::Format(FText::FromString(TEXT("Data table at index {0} does not use FTagValueDataTableRow as its row structure")), FText::AsNumber(Index)));
                Result = EDataValidationResult::Invalid;
            }
        }
    }
    
    return Result;
}
#endif
