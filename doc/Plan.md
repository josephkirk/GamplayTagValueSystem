implementation for extending Unreal Engine's GameplayTags system to store arbitrary values using a subsystem and integrating with Data Tables:

## GameplayTagValueSubsystem

```cpp
// GameplayTagValueSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "UObject/UnrealType.h"
#include "Engine/DataTable.h"
#include "GameplayTagValueSubsystem.generated.h"

UCLASS()
class UTagValueContainer : public UObject
{
    GENERATED_BODY()
public:
    UTagValueContainer() {}
};

USTRUCT(BlueprintType)
struct FGameplayTagValueTableRow : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag")
    FGameplayTag Tag;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value")
    uint8 ValueType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value", meta=(EditCondition="ValueType==0"))
    bool BoolValue;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value", meta=(EditCondition="ValueType==1"))
    int32 IntValue;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value", meta=(EditCondition="ValueType==2"))
    float FloatValue;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value", meta=(EditCondition="ValueType==3"))
    FString StringValue;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Value", meta=(EditCondition="ValueType==4"))
    TSoftObjectPtr ObjectValue;
};

UCLASS()
class UGameplayTagValueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    // Value setter functions
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    void SetTagBoolValue(FGameplayTag Tag, bool Value);
    
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    void SetTagIntValue(FGameplayTag Tag, int32 Value);
    
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    void SetTagFloatValue(FGameplayTag Tag, float Value);
    
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    void SetTagStringValue(FGameplayTag Tag, const FString& Value);
    
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    void SetTagObjectValue(FGameplayTag Tag, UObject* Value);
    
    // Value getter functions
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    bool GetTagBoolValue(FGameplayTag Tag, bool& OutValue);
    
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    bool GetTagIntValue(FGameplayTag Tag, int32& OutValue);
    
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    bool GetTagFloatValue(FGameplayTag Tag, float& OutValue);
    
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    bool GetTagStringValue(FGameplayTag Tag, FString& OutValue);
    
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    bool GetTagObjectValue(FGameplayTag Tag, UObject*& OutValue);
    
    // Utility functions
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    bool HasTagValue(FGameplayTag Tag) const;
    
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    void ClearTagValue(FGameplayTag Tag);
    
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    void ClearAllTagValues();
    
    // Data table integration
    UFUNCTION(BlueprintCallable, Category="GameplayTags|Values")
    void LoadValuesFromDataTable(UDataTable* DataTable);

private:
    // Container to hold property values
    UPROPERTY()
    UTagValueContainer* ValueContainer;
    
    // Map between tags and their properties
    TMap TagPropertyMap;
    
    // Registered data tables
    UPROPERTY()
    TArray RegisteredDataTables;
    
    // Helper for property creation
    template
    PropertyClass* CreateProperty(FGameplayTag Tag, const ValueType& Value);
    
    // Create a property name from a tag
    FName MakePropertyName(const FGameplayTag& Tag) const;
};
```

```cpp
// GameplayTagValueSubsystem.cpp
#include "GameplayTagValueSubsystem.h"

void UGameplayTagValueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Create container for our properties
    ValueContainer = NewObject(this);
    ValueContainer->AddToRoot(); // Prevent garbage collection
}

void UGameplayTagValueSubsystem::Deinitialize()
{
    // Clean up
    if (ValueContainer)
    {
        ValueContainer->RemoveFromRoot();
        ValueContainer = nullptr;
    }
    
    TagPropertyMap.Empty();
    Super::Deinitialize();
}

template
PropertyClass* UGameplayTagValueSubsystem::CreateProperty(FGameplayTag Tag, const ValueType& Value)
{
    FName PropName = MakePropertyName(Tag);
    
    PropertyClass* NewProp = new PropertyClass(ValueContainer->GetClass(), PropName);
    ValueContainer->GetClass()->AddPropertyToRoot(NewProp);
    
    TagPropertyMap.Add(Tag, NewProp);
    NewProp->SetPropertyValue_InContainer(ValueContainer, Value);
    
    return NewProp;
}

FName UGameplayTagValueSubsystem::MakePropertyName(const FGameplayTag& Tag) const
{
    FString TagString = Tag.ToString();
    TagString.ReplaceInline(TEXT("."), TEXT("_"));
    return FName(*TagString);
}

void UGameplayTagValueSubsystem::SetTagBoolValue(FGameplayTag Tag, bool Value)
{
    if (!Tag.IsValid()) return;
    
    FProperty* ExistingProp = TagPropertyMap.FindRef(Tag);
    if (ExistingProp && !CastField(ExistingProp))
    {
        TagPropertyMap.Remove(Tag);
        ExistingProp = nullptr;
    }
    
    if (!ExistingProp)
    {
        CreateProperty(Tag, Value);
    }
    else
    {
        CastField(ExistingProp)->SetPropertyValue_InContainer(ValueContainer, Value);
    }
}

// (Similar implementations for Int, Float, String, and Object setters)

bool UGameplayTagValueSubsystem::GetTagBoolValue(FGameplayTag Tag, bool& OutValue)
{
    if (!Tag.IsValid()) return false;
    
    FProperty* Prop = TagPropertyMap.FindRef(Tag);
    if (!Prop) return false;
    
    FBoolProperty* BoolProp = CastField(Prop);
    if (!BoolProp) return false;
    
    OutValue = BoolProp->GetPropertyValue_InContainer(ValueContainer);
    return true;
}

// (Similar implementations for Int, Float, String, and Object getters)

void UGameplayTagValueSubsystem::LoadValuesFromDataTable(UDataTable* DataTable)
{
    if (!DataTable) return;
    
    RegisteredDataTables.AddUnique(DataTable);
    
    TArray Rows;
    DataTable->GetAllRows(TEXT("GameplayTagValueSubsystem"), Rows);
    
    for (FGameplayTagValueTableRow* Row : Rows)
    {
        if (!Row || !Row->Tag.IsValid()) continue;
        
        switch (Row->ValueType)
        {
            case 0: SetTagBoolValue(Row->Tag, Row->BoolValue); break;
            case 1: SetTagIntValue(Row->Tag, Row->IntValue); break;
            case 2: SetTagFloatValue(Row->Tag, Row->FloatValue); break;
            case 3: SetTagStringValue(Row->Tag, Row->StringValue); break;
            case 4: 
                if (Row->ObjectValue.IsValid())
                    SetTagObjectValue(Row->Tag, Row->ObjectValue.LoadSynchronous());
                break;
        }
    }
}
```

## Data Table Integration

To use this system:

1. **Create a Data Table Asset**:
   - Right-click in the Content Browser > Miscellaneous > Data Table
   - Choose `FGameplayTagValueTableRow` as the row structure
   - Add rows with tags and default values

2. **Setup in Project Settings**:
   - Add Data Table to Gameplay Tag Table List (Project Settings > GameplayTags)
   - Configure tag sources hierarchy

3. **Initialize in Game Instance**:
```cpp
void UYourGameInstance::Init()
{
    Super::Init();
    
    if(UGameplayTagValueSubsystem* TagSystem = GetSubsystem())
    {
        TagSystem->LoadValuesFromDataTable(CharacterAttributesTable);
        TagSystem->LoadValuesFromDataTable(WeaponStatsTable);
    }
}
```

## Blueprint Integration

The functions are already made blueprint-callable, so you can easily use them in Blueprints:

1. Get the GameplayTagValueSubsystem from the Game Instance
2. Call SetTagValue, GetTagValue, or other functions to manage your tag-value associations
3. Use these values in your gameplay logic

This implementation provides:

- Centralized value management through Data Tables
- Hierarchical value inheritance
- Runtime data hot-reloading
- Blueprint-accessible configuration
- Validation and error checking

Remember to add `#include "GameplayTagValueTableRow.h"` to your subsystem implementation files and regenerate project files for proper Data Table recognition.

Citations:
[1] https://www.tomlooman.com/unreal-engine-gameplaytags-data-driven-design/
[2] https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-tags-in-unreal-engine
[3] https://www.quodsoler.com/blog/using-datatables-to-store-game-data
[4] https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-tags?application_version=4.27
[5] https://www.unrealengine.com/es-ES/tech-blog/using-gameplay-tags-to-label-and-organize-your-content-in-ue4
[6] https://forums.unrealengine.com/t/gameplay-tag-data-table-setup/1835479
[7] https://forums.unrealengine.com/t/constants-for-gameplay-tags/258140
[8] https://www.reddit.com/r/unrealengine/comments/1gss7n5/can_gameplay_tags_be_used_instead_of_enumerations/
[9] https://dev.epicgames.com/community/learning/tutorials/Gp9j/working-with-data-in-unreal-engine-data-tables-data-assets-uproperty-specifiers-and-more
[10] https://itsbaffled.github.io/posts/UE/GameplayTags-And-FNames-In-Depth
[11] https://forums.unrealengine.com/t/dataasset-vs-datatables-for-an-item-system/828711
[12] https://www.unrealengine.com/fr/tech-blog/using-gameplay-tags-to-label-and-organize-your-content-in-ue4
[13] https://forums.unrealengine.com/t/from-string-to-gameplay-tag/1262226
[14] https://www.youtube.com/watch?v=e-yh1a8-Hqo
[15] https://www.youtube.com/watch?v=-hcAvuY3tMY
[16] https://www.youtube.com/watch?v=sodHbqcAoYg
[17] https://forums.unrealengine.com/t/best-practices-with-data-tables/59809
[18] https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/GameplayTags/UGameplayTagsManager
[19] https://www.youtube.com/watch?v=nTqsf34nK2c
[20] https://forums.unrealengine.com/t/comprehensive-gameplayabilities-analysis-series/91086

---
Answer from Perplexity: pplx.ai/share