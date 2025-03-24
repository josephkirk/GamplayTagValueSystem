#include "GameplayTagValueSubsystem.h"
#include "Engine/DataTable.h"
#include "GameplayTagValueDataAsset.h"
#include "Kismet/GameplayStatics.h"

// Static member initialization
const FName UGameplayTagValueSubsystem::DefaultRepositoryName = TEXT("Default");
const int32 UGameplayTagValueSubsystem::DefaultRepositoryPriority = 100;

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
    TSharedPtr<FMemoryTagValueRepository> DefaultRepository = MakeShared<FMemoryTagValueRepository>(DefaultRepositoryName, DefaultRepositoryPriority);
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
    Repositories.Add(Repository);
    
    // Sort repositories by priority (highest first)
    Repositories.Sort([](const TSharedPtr<ITagValueRepository>& A, const TSharedPtr<ITagValueRepository>& B)
    {
        return A->GetPriority() > B->GetPriority();
    });
}

void UGameplayTagValueSubsystem::UnregisterRepository(FName RepositoryName)
{
    Repositories.RemoveAll([RepositoryName](const TSharedPtr<ITagValueRepository>& Repository)
    {
        return Repository->GetRepositoryName() == RepositoryName;
    });
}

TSharedPtr<ITagValueRepository> UGameplayTagValueSubsystem::GetRepository(FName RepositoryName) const
{
    for (const TSharedPtr<ITagValueRepository>& Repository : Repositories)
    {
        if (Repository->GetRepositoryName() == RepositoryName)
        {
            return Repository;
        }
    }
    
    return nullptr;
}

TArray<TSharedPtr<ITagValueRepository>> UGameplayTagValueSubsystem::GetAllRepositories() const
{
    return Repositories;
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
    for (const TSharedPtr<ITagValueRepository>& Repository : Repositories)
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
        for (const TSharedPtr<ITagValueRepository>& Repository : Repositories)
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
    for (const TSharedPtr<ITagValueRepository>& Repository : Repositories)
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
        for (const TSharedPtr<ITagValueRepository>& Repository : Repositories)
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
    if (!Tag.IsValid() || !Value.IsValid())
    {
        return false;
    }
    
    TSharedPtr<ITagValueRepository> Repository = GetBestRepository(RepositoryName);
    if (!Repository.IsValid())
    {
        return false;
    }
    
    Repository->SetValue(Tag, Value);
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
        if (Repository.IsValid() && Repository->HasValue(Tag))
        {
            Repository->RemoveValue(Tag);
            bRemovedAny = true;
        }
    }
    else
    {
        // Remove from all repositories
        for (const TSharedPtr<ITagValueRepository>& Repository : Repositories)
        {
            if (Repository->HasValue(Tag))
            {
                Repository->RemoveValue(Tag);
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
            Repository->ClearAllValues();
        }
    }
    else
    {
        // Clear all repositories
        for (const TSharedPtr<ITagValueRepository>& Repository : Repositories)
        {
            Repository->ClearAllValues();
        }
    }
}

TArray<FGameplayTag> UGameplayTagValueSubsystem::GetAllTags() const
{
    TSet<FGameplayTag> UniqueTagsSet;
    
    // Collect tags from all repositories
    for (const TSharedPtr<ITagValueRepository>& Repository : Repositories)
    {
        TArray<FGameplayTag> RepositoryTags = Repository->GetAllTags();
        UniqueTagsSet.Append(RepositoryTags);
    }
    
    TArray<FGameplayTag> Result;
    UniqueTagsSet.GetKeys(Result);
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
    
    // Iterate through all rows in the data table
    for (const TPair<FName, uint8*>& Pair : DataTable->GetRowMap())
    {
        FTagValueMapping* Mapping = reinterpret_cast<FTagValueMapping*>(Pair.Value);
        if (Mapping && Mapping->Tag.IsValid())
        {
            // Create appropriate value holders based on the mapping
            // For simplicity, we'll just import all value types and let the caller decide which ones to use
            
            // Bool value
            TSharedPtr<TTagValueHolder<bool>> BoolHolder = MakeShared<TTagValueHolder<bool>>(Mapping->Value.BoolValue);
            Repository->SetValue(Mapping->Tag, BoolHolder);
            
            // Integer value
            TSharedPtr<TTagValueHolder<int32>> IntHolder = MakeShared<TTagValueHolder<int32>>(Mapping->Value.IntegerValue);
            Repository->SetValue(Mapping->Tag, IntHolder);
            
            // Float value
            TSharedPtr<TTagValueHolder<double>> FloatHolder = MakeShared<TTagValueHolder<double>>(Mapping->Value.FloatValue);
            Repository->SetValue(Mapping->Tag, FloatHolder);
            
            // Transform value
            TSharedPtr<TTagValueHolder<FTransform>> TransformHolder = MakeShared<TTagValueHolder<FTransform>>(Mapping->Value.TransformValue);
            Repository->SetValue(Mapping->Tag, TransformHolder);
            
            // Class value
            TSharedPtr<TTagValueHolder<FSoftClassPath>> ClassHolder = MakeShared<TTagValueHolder<FSoftClassPath>>(Mapping->Value.ClassValue);
            Repository->SetValue(Mapping->Tag, ClassHolder);
            
            // Object value
            TSharedPtr<TTagValueHolder<FSoftObjectPath>> ObjectHolder = MakeShared<TTagValueHolder<FSoftObjectPath>>(Mapping->Value.ObjectValue);
            Repository->SetValue(Mapping->Tag, ObjectHolder);
            
            ImportCount++;
        }
    }
    
    return ImportCount;
}

int32 UGameplayTagValueSubsystem::ExportToDataTable(UDataTable* DataTable, FName RepositoryName)
{
    if (!DataTable)
    {
        return 0;
    }
    
    int32 ExportCount = 0;
    TArray<TSharedPtr<ITagValueRepository>> RepositoriesToExport;
    
    if (RepositoryName != NAME_None)
    {
        // Export from specific repository
        TSharedPtr<ITagValueRepository> Repository = GetRepository(RepositoryName);
        if (Repository.IsValid())
        {
            RepositoriesToExport.Add(Repository);
        }
    }
    else
    {
        // Export from all repositories
        RepositoriesToExport = Repositories;
    }
    
    // Clear the data table first
    DataTable->EmptyTable();
    
    // Collect all unique tags from the repositories
    TSet<FGameplayTag> UniqueTagsSet;
    for (const TSharedPtr<ITagValueRepository>& Repository : RepositoriesToExport)
    {
        TArray<FGameplayTag> RepositoryTags = Repository->GetAllTags();
        UniqueTagsSet.Append(RepositoryTags);
    }
    
    // Export each tag to the data table
    TArray<FGameplayTag> UniqueTags;
    UniqueTagsSet.GetKeys(UniqueTags);
    
    for (const FGameplayTag& Tag : UniqueTags)
    {
        FTagValueMapping Mapping;
        Mapping.Tag = Tag;
        
        // Get the values for this tag from the first repository that has it
        for (const TSharedPtr<ITagValueRepository>& Repository : RepositoriesToExport)
        {
            if (Repository->HasValue(Tag))
            {
                TSharedPtr<ITagValueHolder> ValueHolder = Repository->GetValue(Tag);
                if (ValueHolder.IsValid())
                {
                    // Determine the type and set the appropriate value
                    FName TypeName = ValueHolder->GetValueTypeName();
                    void* ValuePtr = ValueHolder->GetValuePtr();
                    
                    if (TypeName == TBaseStructure<bool>::Get()->GetFName())
                    {
                        Mapping.Value.BoolValue = *static_cast<bool*>(ValuePtr);
                    }
                    else if (TypeName == TBaseStructure<int32>::Get()->GetFName())
                    {
                        Mapping.Value.IntegerValue = *static_cast<int32*>(ValuePtr);
                    }
                    else if (TypeName == TBaseStructure<double>::Get()->GetFName())
                    {
                        Mapping.Value.FloatValue = *static_cast<double*>(ValuePtr);
                    }
                    else if (TypeName == TBaseStructure<FTransform>::Get()->GetFName())
                    {
                        Mapping.Value.TransformValue = *static_cast<FTransform*>(ValuePtr);
                    }
                    else if (TypeName == TBaseStructure<FSoftClassPath>::Get()->GetFName())
                    {
                        Mapping.Value.ClassValue = *static_cast<FSoftClassPath*>(ValuePtr);
                    }
                    else if (TypeName == TBaseStructure<FSoftObjectPath>::Get()->GetFName())
                    {
                        Mapping.Value.ObjectValue = *static_cast<FSoftObjectPath*>(ValuePtr);
                    }
                    
                    break;
                }
            }
        }
        
        // Add the mapping to the data table
        FName RowName = *Tag.ToString();
        DataTable->AddRow(RowName, Mapping);
        ExportCount++;
    }
    
    return ExportCount;
}

//------------------------------------------------------------------------------
// Type-specific accessor methods
//------------------------------------------------------------------------------

bool UGameplayTagValueSubsystem::GetBoolValue(FGameplayTag Tag, bool DefaultValue, UObject* Context) const
{
    return GetTypedValue<bool>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetBoolValue(FGameplayTag Tag, bool Value, FName RepositoryName)
{
    return SetTypedValue<bool>(Tag, Value, RepositoryName);
}

int32 UGameplayTagValueSubsystem::GetIntValue(FGameplayTag Tag, int32 DefaultValue, UObject* Context) const
{
    return GetTypedValue<int32>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetIntValue(FGameplayTag Tag, int32 Value, FName RepositoryName)
{
    return SetTypedValue<int32>(Tag, Value, RepositoryName);
}

double UGameplayTagValueSubsystem::GetFloatValue(FGameplayTag Tag, double DefaultValue, UObject* Context) const
{
    return GetTypedValue<double>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetFloatValue(FGameplayTag Tag, double Value, FName RepositoryName)
{
    return SetTypedValue<double>(Tag, Value, RepositoryName);
}

FTransform UGameplayTagValueSubsystem::GetTransformValue(FGameplayTag Tag, const FTransform& DefaultValue, UObject* Context) const
{
    return GetTypedValue<FTransform>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetTransformValue(FGameplayTag Tag, const FTransform& Value, FName RepositoryName)
{
    return SetTypedValue<FTransform>(Tag, Value, RepositoryName);
}

FSoftClassPath UGameplayTagValueSubsystem::GetClassValue(FGameplayTag Tag, const FSoftClassPath& DefaultValue, UObject* Context) const
{
    return GetTypedValue<FSoftClassPath>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetClassValue(FGameplayTag Tag, const FSoftClassPath& Value, FName RepositoryName)
{
    return SetTypedValue<FSoftClassPath>(Tag, Value, RepositoryName);
}

FSoftObjectPath UGameplayTagValueSubsystem::GetObjectValue(FGameplayTag Tag, const FSoftObjectPath& DefaultValue, UObject* Context) const
{
    return GetTypedValue<FSoftObjectPath>(Tag, DefaultValue, Context);
}

bool UGameplayTagValueSubsystem::SetObjectValue(FGameplayTag Tag, const FSoftObjectPath& Value, FName RepositoryName)
{
    return SetTypedValue<FSoftObjectPath>(Tag, Value, RepositoryName);
}

//------------------------------------------------------------------------------
// Helper methods
//------------------------------------------------------------------------------

TSharedPtr<ITagValueRepository> UGameplayTagValueSubsystem::GetBestRepository(FName RepositoryName) const
{
    if (RepositoryName != NAME_None)
    {
        // Try to find the specified repository
        TSharedPtr<ITagValueRepository> Repository = GetRepository(RepositoryName);
        if (Repository.IsValid())
        {
            return Repository;
        }
    }
    
    // Fall back to the default repository
    return GetRepository(DefaultRepositoryName);
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
    
    // This is a bit of a hack since we can't directly call the templated method
    // We need to specialize for each supported type
    if constexpr (std::is_same_v<T, bool>)
    {
        OutValue = TagValueInterface->GetBoolValue(Tag, DefaultValue);
        return true;
    }
    else if constexpr (std::is_same_v<T, int32>)
    {
        OutValue = TagValueInterface->GetIntValue(Tag, DefaultValue);
        return true;
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        OutValue = TagValueInterface->GetFloatValue(Tag, DefaultValue);
        return true;
    }
    else if constexpr (std::is_same_v<T, FTransform>)
    {
        OutValue = TagValueInterface->GetTransformValue(Tag, DefaultValue);
        return true;
    }
    else if constexpr (std::is_same_v<T, FSoftClassPath>)
    {
        OutValue = TagValueInterface->GetClassValue(Tag, DefaultValue);
        return true;
    }
    else if constexpr (std::is_same_v<T, FSoftObjectPath>)
    {
        OutValue = TagValueInterface->GetObjectValue(Tag, DefaultValue);
        return true;
    }
    
    return false;
}

template<typename T>
bool UGameplayTagValueSubsystem::TryGetValueFromRepositories(FGameplayTag Tag, T& OutValue) const
{
    TSharedPtr<ITagValueHolder> ValueHolder = GetRawValue(Tag);
    if (!ValueHolder.IsValid())
    {
        return false;
    }
    
    // Check if the value type matches what we're looking for
    FName TypeName = ValueHolder->GetValueTypeName();
    FName ExpectedTypeName = TBaseStructure<T>::Get()->GetFName();
    
    if (TypeName != ExpectedTypeName)
    {
        return false;
    }
    
    // Get the value
    void* ValuePtr = ValueHolder->GetValuePtr();
    if (!ValuePtr)
    {
        return false;
    }
    
    OutValue = *static_cast<T*>(ValuePtr);
    return true;
}

template<typename T>
T UGameplayTagValueSubsystem::GetTypedValue(FGameplayTag Tag, const T& DefaultValue, UObject* Context) const
{
    if (!Tag.IsValid())
    {
        return DefaultValue;
    }
    
    T Result = DefaultValue;
    
    // Try to get the value from the context first
    if (TryGetValueFromContext(Context, Tag, Result, DefaultValue))
    {
        return Result;
    }
    
    // Try to get the value from the repositories
    if (TryGetValueFromRepositories(Tag, Result))
    {
        return Result;
    }
    
    return DefaultValue;
}

template<typename T>
bool UGameplayTagValueSubsystem::SetTypedValue(FGameplayTag Tag, const T& Value, FName RepositoryName)
{
    if (!Tag.IsValid())
    {
        return false;
    }
    
    TSharedPtr<TTagValueHolder<T>> ValueHolder = MakeShared<TTagValueHolder<T>>(Value);
    return SetRawValue(Tag, ValueHolder, RepositoryName);
}

// Template specializations to ensure they are compiled
template bool UGameplayTagValueSubsystem::TryGetValueFromContext<bool>(UObject*, FGameplayTag, bool&, const bool&) const;
template bool UGameplayTagValueSubsystem::TryGetValueFromContext<int32>(UObject*, FGameplayTag, int32&, const int32&) const;
template bool UGameplayTagValueSubsystem::TryGetValueFromContext<double>(UObject*, FGameplayTag, double&, const double&) const;
template bool UGameplayTagValueSubsystem::TryGetValueFromContext<FTransform>(UObject*, FGameplayTag, FTransform&, const FTransform&) const;
template bool UGameplayTagValueSubsystem::TryGetValueFromContext<FSoftClassPath>(UObject*, FGameplayTag, FSoftClassPath&, const FSoftClassPath&) const;
template bool UGameplayTagValueSubsystem::TryGetValueFromContext<FSoftObjectPath>(UObject*, FGameplayTag, FSoftObjectPath&, const FSoftObjectPath&) const;

template bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<bool>(FGameplayTag, bool&) const;
template bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<int32>(FGameplayTag, int32&) const;
template bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<double>(FGameplayTag, double&) const;
template bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<FTransform>(FGameplayTag, FTransform&) const;
template bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<FSoftClassPath>(FGameplayTag, FSoftClassPath&) const;
template bool UGameplayTagValueSubsystem::TryGetValueFromRepositories<FSoftObjectPath>(FGameplayTag, FSoftObjectPath&) const;

template bool UGameplayTagValueSubsystem::SetTypedValue<bool>(FGameplayTag, const bool&, FName);
template bool UGameplayTagValueSubsystem::SetTypedValue<int32>(FGameplayTag, const int32&, FName);
template bool UGameplayTagValueSubsystem::SetTypedValue<double>(FGameplayTag, const double&, FName);
template bool UGameplayTagValueSubsystem::SetTypedValue<FTransform>(FGameplayTag, const FTransform&, FName);
template bool UGameplayTagValueSubsystem::SetTypedValue<FSoftClassPath>(FGameplayTag, const FSoftClassPath&, FName);
template bool UGameplayTagValueSubsystem::SetTypedValue<FSoftObjectPath>(FGameplayTag, const FSoftObjectPath&, FName);

template bool UGameplayTagValueSubsystem::GetTypedValue<bool>(FGameplayTag, const bool&, UObject*) const;
template bool UGameplayTagValueSubsystem::GetTypedValue<int32>(FGameplayTag, const int32&, UObject*) const;
template double UGameplayTagValueSubsystem::GetTypedValue<double>(FGameplayTag, const double&, UObject*) const;
template FTransform UGameplayTagValueSubsystem::GetTypedValue<FTransform>(FGameplayTag, const FTransform&, UObject*) const;
template FSoftClassPath UGameplayTagValueSubsystem::GetTypedValue<FSoftClassPath>(FGameplayTag, const FSoftClassPath&, UObject*) const;
template FSoftObjectPath UGameplayTagValueSubsystem::GetTypedValue<FSoftObjectPath>(FGameplayTag, const FSoftObjectPath&, UObject*) const;

int32 UGameplayTagValueSubsystem::RegisterConfiguredDataAssets()
{
    int32 RegisteredCount = 0;
    
    // Find all GameplayTagValueDataAsset objects
    TArray<UGameplayTagValueDataAsset*> DataAssets;
    for (TObjectIterator<UGameplayTagValueDataAsset> It; It; ++It)
    {
        UGameplayTagValueDataAsset* DataAsset = *It;
        if (DataAsset && !DataAsset->IsPendingKill() && DataAsset->bRegisterToGameplayTagValueSubsystem)
        {
            // Register this data asset
            int32 ImportedCount = DataAsset->RegisterToSubsystem(this);
            if (ImportedCount > 0)
            {
                RegisteredCount++;
                UE_LOG(LogTemp, Log, TEXT("Auto-registered GameplayTagValueDataAsset '%s' with %d values"), 
                    *DataAsset->GetName(), ImportedCount);
            }
        }
    }
    
    return RegisteredCount;
}