// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.
#include "GameplayTagValueSubsystem.h"
#include "Engine/DataTable.h"
#include "GameplayTagValueDataAsset.h"
#include "Kismet/GameplayStatics.h"

// Static member initialization
const FName UGameplayTagValueSubsystem::DefaultRepositoryName = TEXT("Default");

//------------------------------------------------------------------------------
// FMemoryTagValueRepository Implementation
//------------------------------------------------------------------------------

FMemoryTagValueRepository::FMemoryTagValueRepository(const FName& InName, int32 InPriority)
    : RepositoryName(InName)
    , Priority(InPriority)
{
}

bool FMemoryTagValueRepository::HasValue(FGameplayTag Tag) const
{
    return TagValues.Contains(Tag);
}

TSharedPtr<ITagValueHolder> FMemoryTagValueRepository::GetValue(FGameplayTag Tag) const
{
    const TSharedPtr<ITagValueHolder>* ValuePtr = TagValues.Find(Tag);
    return ValuePtr ? *ValuePtr : nullptr;
}

void FMemoryTagValueRepository::SetValue(FGameplayTag Tag, TSharedPtr<ITagValueHolder> Value)
{
    if (Tag.IsValid() && Value.IsValid())
    {
        TagValues.Add(Tag, Value);
    }
}

void FMemoryTagValueRepository::RemoveValue(FGameplayTag Tag)
{
    TagValues.Remove(Tag);
}

void FMemoryTagValueRepository::ClearAllValues()
{
    TagValues.Empty();
}

TArray<FGameplayTag> FMemoryTagValueRepository::GetAllTags() const
{
    TArray<FGameplayTag> Result;
    TagValues.GetKeys(Result);
    return Result;
}

FName FMemoryTagValueRepository::GetRepositoryName() const
{
    return RepositoryName;
}

int32 FMemoryTagValueRepository::GetPriority() const
{
    return Priority;
}

//------------------------------------------------------------------------------
// UGameplayTagValueSubsystem Implementation
//------------------------------------------------------------------------------

void UGameplayTagValueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Create the default repository
    TSharedPtr<FMemoryTagValueRepository> DefaultRepository = MakeShared<FMemoryTagValueRepository>(DefaultRepositoryName, 100);
    RegisterRepository(DefaultRepository);
    
    // Register any configured data assets
    RegisterConfiguredDataAssets();
}

void UGameplayTagValueSubsystem::Deinitialize()
{
    // Clear all repositories
    Repositories.Empty();
    
    Super::Deinitialize();
}

void UGameplayTagValueSubsystem::RegisterRepository(TSharedPtr<ITagValueRepository> Repository)
{
    if (!Repository.IsValid())
    {
        return;
    }
    
    // Remove any existing repository with the same name
    UnregisterRepository(Repository->GetRepositoryName());
    
    // Add the new repository
    Repositories.Add(Repository->GetRepositoryName(), Repository);
}

void UGameplayTagValueSubsystem::UnregisterRepository(FName RepositoryName)
{
    Repositories.Remove(RepositoryName);
}

TSharedPtr<ITagValueRepository> UGameplayTagValueSubsystem::GetRepository(FName RepositoryName) const
{
    return Repositories.FindRef(RepositoryName);
}

TArray<TSharedPtr<ITagValueRepository>> UGameplayTagValueSubsystem::GetAllRepositories() const
{
    TArray<TSharedPtr<ITagValueRepository>> Result;
    Repositories.GenerateValueArray(Result);
    
    // Sort by priority (highest first)
    Result.Sort([](const TSharedPtr<ITagValueRepository>& A, const TSharedPtr<ITagValueRepository>& B)
    {
        return A->GetPriority() > B->GetPriority();
    });
    
    return Result;
}

bool UGameplayTagValueSubsystem::HasTagValue(FGameplayTag Tag, UObject* Context) const
{
    // Check context first if provided
    if (Context && ImplementsTagValueInterface(Context))
    {
        ITagValueInterface* TagValueInterface = GetTagValueInterface(Context);
        if (TagValueInterface && TagValueInterface->HasTagValue(Tag))
        {
            return true;
        }
    }
    
    // Check repositories
    TArray<TSharedPtr<ITagValueRepository>> RepositoryArray = GetAllRepositories();
    for (const TSharedPtr<ITagValueRepository>& Repository : RepositoryArray)
    {
        if (Repository->HasValue(Tag))
        {
            return true;
        }
    }
    
    // Check parent tags (hierarchical inheritance)
    FGameplayTag ParentTag = Tag.RequestDirectParent();
    while (ParentTag.IsValid())
    {
        for (const TSharedPtr<ITagValueRepository>& Repository : RepositoryArray)
        {
            if (Repository->HasValue(ParentTag))
            {
                return true;
            }
        }
        
        ParentTag = ParentTag.RequestDirectParent();
    }
    
    return false;
}

TSharedPtr<ITagValueHolder> UGameplayTagValueSubsystem::GetRawValue(FGameplayTag Tag, UObject* Context) const
{
    // Check context first if provided
    if (Context && ImplementsTagValueInterface(Context))
    {
        // Context objects handle their own tag hierarchy, so we don't need to check parent tags here
        // This is because the context might have specific override logic
    }
    
    // Check repositories for exact match
    TArray<TSharedPtr<ITagValueRepository>> RepositoryArray = GetAllRepositories();
    for (const TSharedPtr<ITagValueRepository>& Repository : RepositoryArray)
    {
        if (Repository->HasValue(Tag))
        {
            return Repository->GetValue(Tag);
        }
    }
    
    // Check parent tags (hierarchical inheritance)
    FGameplayTag ParentTag = Tag.RequestDirectParent();
    while (ParentTag.IsValid())
    {
        for (const TSharedPtr<ITagValueRepository>& Repository : RepositoryArray)
        {
            if (Repository->HasValue(ParentTag))
            {
                return Repository->GetValue(ParentTag);
            }
        }
        
        ParentTag = ParentTag.RequestDirectParent();
    }
    
    return nullptr;
}

bool UGameplayTagValueSubsystem::SetRawValue(FGameplayTag Tag, TSharedPtr<ITagValueHolder> Value, FName RepositoryName)
{
    if (!Tag.IsValid())
    {
        return false;
    }
    
    TSharedPtr<ITagValueRepository> Repository = GetBestRepository(RepositoryName);
    if (!Repository.IsValid())
    {
        return false;
    }
    
    TSharedPtr<ITagValueHolder> OldValue = Repository->GetValue(Tag);
    
    if (!Value.IsValid())
    {
        Repository->RemoveValue(Tag);
    }
    else
    {
        Repository->SetValue(Tag, Value);
    }
    
    BroadcastTagValueChanged(Tag, Repository->GetRepositoryName(), OldValue, Value);
    
    return true;
}

bool UGameplayTagValueSubsystem::RemoveTagValue(FGameplayTag Tag, FName RepositoryName)
{
    if (!Tag.IsValid())
    {
        return false;
    }
    
    bool bRemovedAny = false;
    
    if (RepositoryName != NAME_None)
    {
        // Remove from specific repository
        TSharedPtr<ITagValueRepository> Repository = GetRepository(RepositoryName);
        if (Repository.IsValid())
        {
            if (Repository->HasValue(Tag))
            {
                TSharedPtr<ITagValueHolder> OldValue = Repository->GetValue(Tag);
                Repository->RemoveValue(Tag);
                BroadcastTagValueChanged(Tag, Repository->GetRepositoryName(), OldValue, nullptr);
                bRemovedAny = true;
            }
        }
    }
    else
    {
        // Remove from all repositories
        TArray<TSharedPtr<ITagValueRepository>> RepositoryArray = GetAllRepositories();
        for (const TSharedPtr<ITagValueRepository>& Repository : RepositoryArray)
        {
            if (Repository->HasValue(Tag))
            {
                TSharedPtr<ITagValueHolder> OldValue = Repository->GetValue(Tag);
                Repository->RemoveValue(Tag);
                BroadcastTagValueChanged(Tag, Repository->GetRepositoryName(), OldValue, nullptr);
                bRemovedAny = true;
            }
        }
    }
    
    return bRemovedAny;
}

void UGameplayTagValueSubsystem::ClearAllValues(FName RepositoryName)
{
    if (RepositoryName != NAME_None)
    {
        // Clear specific repository
        TSharedPtr<ITagValueRepository> Repository = GetRepository(RepositoryName);
        if (Repository.IsValid())
        {
            // Get all tags before clearing
            TArray<FGameplayTag> Tags = Repository->GetAllTags();
            
            // Clear repository
            Repository->ClearAllValues();
            
            // Broadcast changes for each tag
            for (const FGameplayTag& Tag : Tags)
            {
                BroadcastTagValueChanged(Tag, Repository->GetRepositoryName(), nullptr, nullptr);
            }
        }
    }
    else
    {
        // Clear all repositories
        TArray<TSharedPtr<ITagValueRepository>> RepositoryArray = GetAllRepositories();
        for (const TSharedPtr<ITagValueRepository>& Repository : RepositoryArray)
        {
            // Get all tags before clearing
            TArray<FGameplayTag> Tags = Repository->GetAllTags();
            
            // Clear repository
            Repository->ClearAllValues();
            
            // Broadcast changes for each tag
            for (const FGameplayTag& Tag : Tags)
            {
                BroadcastTagValueChanged(Tag, Repository->GetRepositoryName(), nullptr, nullptr);
            }
        }
    }
}

void UGameplayTagValueSubsystem::BroadcastTagValueChanged(FGameplayTag Tag, FName RepositoryName, const TSharedPtr<ITagValueHolder>& OldValue, const TSharedPtr<ITagValueHolder>& NewValue)
{
    OnTagValueChanged.Broadcast(Tag, RepositoryName);
}

TArray<FGameplayTag> UGameplayTagValueSubsystem::GetAllTags() const
{
    TArray<FGameplayTag> Result;
    TSet<FGameplayTag> TagSet;
    
    // Add all tags from all repositories
    TArray<TSharedPtr<ITagValueRepository>> RepositoryArray = GetAllRepositories();
    for (const TSharedPtr<ITagValueRepository>& Repository : RepositoryArray)
    {
        TArray<FGameplayTag> RepositoryTags = Repository->GetAllTags();
        for (const FGameplayTag& Tag : RepositoryTags)
        {
            TagSet.Add(Tag);
        }
    }
    
    // Convert to array
    TagSet.GetKeys(Result);
    return Result;
}

int32 UGameplayTagValueSubsystem::ImportFromDataTable(UDataTable* DataTable, FName RepositoryName)
{
    if (!DataTable)
    {
        return 0;
    }
    
    TSharedPtr<ITagValueRepository> Repository = GetBestRepository(RepositoryName);
    if (!Repository.IsValid())
    {
        return 0;
    }
    
    int32 ImportCount = 0;
    
    // Import rows from data table
    // The implementation would depend on the structure of the data table
    // and the specific tag value types to import
    
    return ImportCount;
}

int32 UGameplayTagValueSubsystem::ExportToDataTable(UDataTable* DataTable, FName RepositoryName)
{
    if (!DataTable)
    {
        return 0;
    }
    
    int32 ExportCount = 0;
    
    // Export tag values to data table
    // The implementation would depend on the structure of the data table
    // and the specific tag value types to export
    
    return ExportCount;
}

//------------------------------------------------------------------------------
// Type-specific accessor methods
//------------------------------------------------------------------------------

bool UGameplayTagValueSubsystem::GetBoolValue(FGameplayTag Tag, bool DefaultValue, UObject* Context) const
{
    return GetTypedValue<FBoolTagValue>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetBoolValue(FGameplayTag Tag, bool Value, FName RepositoryName)
{
    return SetTypedValue<FBoolTagValue>(Tag, Value, RepositoryName);
}

int32 UGameplayTagValueSubsystem::GetIntValue(FGameplayTag Tag, int32 DefaultValue, UObject* Context) const
{
    return GetTypedValue<FIntTagValue>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetIntValue(FGameplayTag Tag, int32 Value, FName RepositoryName)
{
    return SetTypedValue<FIntTagValue>(Tag, Value, RepositoryName);
}

float UGameplayTagValueSubsystem::GetFloatValue(FGameplayTag Tag, float DefaultValue, UObject* Context) const
{
    return GetTypedValue<FFloatTagValue>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetFloatValue(FGameplayTag Tag, float Value, FName RepositoryName)
{
    return SetTypedValue<FFloatTagValue>(Tag, Value, RepositoryName);
}

FString UGameplayTagValueSubsystem::GetStringValue(FGameplayTag Tag, const FString& DefaultValue, UObject* Context) const
{
    return GetTypedValue<FStringTagValue>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetStringValue(FGameplayTag Tag, const FString& Value, FName RepositoryName)
{
    return SetTypedValue<FStringTagValue>(Tag, Value, RepositoryName);
}

FTransform UGameplayTagValueSubsystem::GetTransformValue(FGameplayTag Tag, const FTransform& DefaultValue, UObject* Context) const
{
    return GetTypedValue<FTransformTagValue>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetTransformValue(FGameplayTag Tag, const FTransform& Value, FName RepositoryName)
{
    return SetTypedValue<FTransformTagValue>(Tag, Value, RepositoryName);
}

TSoftClassPtr<UObject> UGameplayTagValueSubsystem::GetClassValue(FGameplayTag Tag, TSoftClassPtr<UObject> DefaultValue, UObject* Context) const
{
    return GetTypedValue<FClassTagValue>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetClassValue(FGameplayTag Tag, TSoftClassPtr<UObject> Value, FName RepositoryName)
{
    return SetTypedValue<FClassTagValue>(Tag, Value, RepositoryName);
}

TSoftObjectPtr<UObject> UGameplayTagValueSubsystem::GetObjectValue(FGameplayTag Tag, TSoftObjectPtr<UObject> DefaultValue, UObject* Context) const
{
    return GetTypedValue<FObjectTagValue>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetObjectValue(FGameplayTag Tag, TSoftObjectPtr<UObject> Value, FName RepositoryName)
{
    return SetTypedValue<FObjectTagValue>(Tag, Value, RepositoryName);
}

//------------------------------------------------------------------------------
// Helper methods
//------------------------------------------------------------------------------

TSharedPtr<ITagValueRepository> UGameplayTagValueSubsystem::GetBestRepository(FName RepositoryName) const
{
    if (RepositoryName != NAME_None)
    {
        // Get specific repository
        return GetRepository(RepositoryName);
    }
    else
    {
        // Get highest priority repository
        TArray<TSharedPtr<ITagValueRepository>> RepositoryArray = GetAllRepositories();
        return RepositoryArray.Num() > 0 ? RepositoryArray[0] : nullptr;
    }
}

bool UGameplayTagValueSubsystem::ImplementsTagValueInterface(UObject* Object) const
{
    return Object && Object->GetClass()->ImplementsInterface(UTagValueInterface::StaticClass());
}

ITagValueInterface* UGameplayTagValueSubsystem::GetTagValueInterface(UObject* Object) const
{
    if (ImplementsTagValueInterface(Object))
    {
        return Cast<ITagValueInterface>(Object);
    }
    
    return nullptr;
}

template<typename T>
bool UGameplayTagValueSubsystem::TryGetValueFromContext(UObject* Context, FGameplayTag Tag, T& OutValue, const T& DefaultValue) const
{
    if (!Context || !ImplementsTagValueInterface(Context))
    {
        return false;
    }
    
    ITagValueInterface* TagValueInterface = GetTagValueInterface(Context);
    if (!TagValueInterface || !TagValueInterface->HasTagValue(Tag))
    {
        return false;
    }
    
    // Use the appropriate getter based on the type
    if constexpr (std::is_same_v<T, bool>)
    {
        OutValue = TagValueInterface->GetBoolValue(Tag, DefaultValue);
    }
    else if constexpr (std::is_same_v<T, int32>)
    {
        OutValue = TagValueInterface->GetIntValue(Tag, DefaultValue);
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        OutValue = TagValueInterface->GetFloatValue(Tag, DefaultValue);
    }
    else if constexpr (std::is_same_v<T, FString>)
    {
        OutValue = TagValueInterface->GetStringValue(Tag, DefaultValue);
    }
    else if constexpr (std::is_same_v<T, FTransform>)
    {
        OutValue = TagValueInterface->GetTransformValue(Tag, DefaultValue);
    }
    else if constexpr (std::is_same_v<T, TSoftClassPtr<UObject>>)
    {
        OutValue = TagValueInterface->GetClassValue(Tag, DefaultValue);
    }
    else if constexpr (std::is_same_v<T, TSoftObjectPtr<UObject>>)
    {
        OutValue = TagValueInterface->GetObjectValue(Tag, DefaultValue);
    }
    else
    {
        // Unknown type
        return false;
    }
    
    return true;
}

template<typename TagValueType>
bool UGameplayTagValueSubsystem::TryGetValueFromRepositories(FGameplayTag Tag, typename TagValueType::ValueType& OutValue) const
{
    TSharedPtr<ITagValueHolder> RawValue = GetRawValue(Tag, nullptr);
    if (!RawValue.IsValid())
    {
        return false;
    }
    
    // Try to get the value from the holder
    void* ValuePtr = RawValue->GetValuePtr();
    if (!ValuePtr)
    {
        return false;
    }
    
    // Check if the value holder is of the correct type
    if (RawValue->GetValueTypeName() == TagValueType::StaticStruct()->GetFName())
    {
        // It's the right type, so cast and return the value
        TagValueType* TypedValue = static_cast<TagValueType*>(ValuePtr);
        OutValue = TypedValue->Value;
        return true;
    }
    
    return false;
}

template<typename TagValueType>
typename TagValueType::ValueType UGameplayTagValueSubsystem::GetTypedValue(FGameplayTag Tag, const typename TagValueType::ValueType& DefaultValue, UObject* Context) const
{
    using ValueType = typename TagValueType::ValueType;
    ValueType Result = DefaultValue;
    
    // Try to get from context first
    if (TryGetValueFromContext(Context, Tag, Result, DefaultValue))
    {
        return Result;
    }
    
    // Try to get from repositories
    if (TryGetValueFromRepositories<TagValueType>(Tag, Result))
    {
        return Result;
    }
    
    return DefaultValue;
}

template<typename TagValueType>
bool UGameplayTagValueSubsystem::SetTypedValue(FGameplayTag Tag, const typename TagValueType::ValueType& Value, FName RepositoryName)
{
    if (!Tag.IsValid())
    {
        return false;
    }
    
    TSharedPtr<ITagValueRepository> Repository = GetBestRepository(RepositoryName);
    if (!Repository.IsValid())
    {
        return false;
    }
    
    // Create a typed tag value and wrap it in a value holder
    TagValueType* TagValue = new TagValueType(Value);
    TSharedPtr<ITagValueHolder> ValueHolder = MakeShared<TTagValueHolder<TagValueType>>(*TagValue);
    delete TagValue; // The holder makes a copy, so we can delete the original
    
    // Set the value in the repository
    return SetRawValue(Tag, ValueHolder, Repository->GetRepositoryName());
}

// Template specializations to ensure they are compiled
bool UGameplayTagValueSubsystem::TryGetValueFromContext<bool>(UObject*, FGameplayTag, bool&, const bool&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromContext<int32>(UObject*, FGameplayTag, int32&, const int32&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromContext<float>(UObject*, FGameplayTag, float&, const float&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromContext<FString>(UObject*, FGameplayTag, FString&, const FString&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromContext<FTransform>(UObject*, FGameplayTag, FTransform&, const FTransform&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromContext<TSoftClassPtr<UObject>>(UObject*, FGameplayTag, TSoftClassPtr<UObject>&, const TSoftClassPtr<UObject>&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromContext<TSoftObjectPtr<UObject>>(UObject*, FGameplayTag, TSoftObjectPtr<UObject>&, const TSoftObjectPtr<UObject>&) const;

bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<FBoolTagValue>(FGameplayTag, bool&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<FIntTagValue>(FGameplayTag, int32&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<FFloatTagValue>(FGameplayTag, float&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<FStringTagValue>(FGameplayTag, FString&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<FTransformTagValue>(FGameplayTag, FTransform&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<FClassTagValue>(FGameplayTag, TSoftClassPtr<UObject>&) const;
bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<FObjectTagValue>(FGameplayTag, TSoftObjectPtr<UObject>&) const;

bool UGameplayTagValueSubsystem::SetTypedValue<FBoolTagValue>(FGameplayTag, const bool&, FName);
bool UGameplayTagValueSubsystem::SetTypedValue<FIntTagValue>(FGameplayTag, const int32&, FName);
bool UGameplayTagValueSubsystem::SetTypedValue<FFloatTagValue>(FGameplayTag, const float&, FName);
bool UGameplayTagValueSubsystem::SetTypedValue<FStringTagValue>(FGameplayTag, const FString&, FName);
bool UGameplayTagValueSubsystem::SetTypedValue<FTransformTagValue>(FGameplayTag, const FTransform&, FName);
bool UGameplayTagValueSubsystem::SetTypedValue<FClassTagValue>(FGameplayTag, const TSoftClassPtr<UObject>&, FName);
bool UGameplayTagValueSubsystem::SetTypedValue<FObjectTagValue>(FGameplayTag, const TSoftObjectPtr<UObject>&, FName);

int32 UGameplayTagValueSubsystem::RegisterConfiguredDataAssets()
{
    int32 RegisteredCount = 0;
    
    // Find all GameplayTagValueDataAsset objects
    TArray<UGameplayTagValueDataAsset*> DataAssets;
    for (TObjectIterator<UGameplayTagValueDataAsset> It; It; ++It)
    {
        UGameplayTagValueDataAsset* DataAsset = *It;
        if (DataAsset && !DataAsset->IsTemplate() && DataAsset->bAutoRegister)
        {
            DataAssets.Add(DataAsset);
        }
    }
    
    // Sort by priority
    DataAssets.Sort([](const UGameplayTagValueDataAsset& A, const UGameplayTagValueDataAsset& B)
    {
        return A.Priority > B.Priority;
    });
    
    // Register each data asset
    for (UGameplayTagValueDataAsset* DataAsset : DataAssets)
    {
        // Implementation depends on the structure of UGameplayTagValueDataAsset
        // and how it stores tag values
        RegisteredCount++;
    }
    
    return RegisteredCount;
}