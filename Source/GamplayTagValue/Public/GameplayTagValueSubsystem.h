#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTags.h"
#include "TagValueInterface.h"
#include "TagValue.h"
#include "GameplayTagValueSubsystem.generated.h"

// Forward declaration
class UGameplayTagValueDataAsset;

/**
 * Delegate for when a tag value changes
 * @param Tag The tag that changed
 * @param RepositoryName The repository where the change occurred
 * @param OldValue The previous value (may be null if no previous value existed)
 * @param NewValue The new value (may be null if the value was removed)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnTagValueChanged, FGameplayTag, Tag, FName, RepositoryName, const TSharedPtr<ITagValueHolder>&, OldValue, const TSharedPtr<ITagValueHolder>&, NewValue);

/**
 * Memory-based repository implementation for storing tag values in memory
 */
class GAMPLAYTAGVALUE_API FMemoryTagValueRepository : public ITagValueRepository
{
public:
    FMemoryTagValueRepository(const FName& InName, int32 InPriority);
    
    // ITagValueRepository interface
    virtual bool HasValue(FGameplayTag Tag) const override;
    virtual TSharedPtr<ITagValueHolder> GetValue(FGameplayTag Tag) const override;
    virtual void SetValue(FGameplayTag Tag, TSharedPtr<ITagValueHolder> Value) override;
    virtual void RemoveValue(FGameplayTag Tag) override;
    virtual void ClearAllValues() override;
    virtual TArray<FGameplayTag> GetAllTags() const override;
    virtual FName GetRepositoryName() const override;
    virtual int32 GetPriority() const override;
    
private:
    /** Map of tags to their values */
    TMap<FGameplayTag, TSharedPtr<ITagValueHolder>> TagValues;
    
    /** Name of this repository */
    FName RepositoryName;
    
    /** Priority of this repository */
    int32 Priority;
};

/**
 * Central subsystem for managing gameplay tag values
 * Provides a unified API for getting and setting values associated with gameplay tags
 * Manages multiple repositories and handles hierarchical tag inheritance
 */
UCLASS()
class GAMPLAYTAGVALUE_API UGameplayTagValueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    /**
     * Event triggered when a tag value changes (set, removed, or cleared)
     * Provides the tag that changed and the repository name where the change occurred
     */
    UPROPERTY(BlueprintAssignable, Category = "Gameplay Tags|Values")
    FOnTagValueChanged OnTagValueChanged;
    
    /**
     * Register a repository with the subsystem
     * @param Repository The repository to register
     */
    void RegisterRepository(TSharedPtr<ITagValueRepository> Repository);
    
    /**
     * Unregister a repository from the subsystem
     * @param RepositoryName The name of the repository to unregister
     */
    void UnregisterRepository(FName RepositoryName);
    
    /**
     * Get a repository by name
     * @param RepositoryName The name of the repository to get
     * @return The repository, or nullptr if not found
     */
    TSharedPtr<ITagValueRepository> GetRepository(FName RepositoryName) const;
    
    /**
     * Get all registered repositories
     * @return Array of all repositories
     */
    TArray<TSharedPtr<ITagValueRepository>> GetAllRepositories() const;
    
    /**
     * Check if a value exists for the given tag in any repository
     * @param Tag The tag to check
     * @param Context Optional context object that implements UTagValueInterface
     * @return True if a value exists for the tag
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    bool HasTagValue(FGameplayTag Tag, UObject* Context = nullptr) const;
    
    /**
     * Get a bool value for the given tag
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @return The bool value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    bool GetBoolValue(FGameplayTag Tag, bool DefaultValue = false, UObject* Context = nullptr) const;
    
    /**
     * Set a bool value for the given tag
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    bool SetBoolValue(FGameplayTag Tag, bool Value, FName RepositoryName = NAME_None);
    
    /**
     * Get an integer value for the given tag
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @return The integer value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    int32 GetIntValue(FGameplayTag Tag, int32 DefaultValue = 0, UObject* Context = nullptr) const;
    
    /**
     * Set an integer value for the given tag
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    bool SetIntValue(FGameplayTag Tag, int32 Value, FName RepositoryName = NAME_None);
    
    /**
     * Get a float value for the given tag
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @return The float value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    double GetFloatValue(FGameplayTag Tag, double DefaultValue = 0.0, UObject* Context = nullptr) const;
    
    /**
     * Set a float value for the given tag
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    bool SetFloatValue(FGameplayTag Tag, double Value, FName RepositoryName = NAME_None);
    
    /**
     * Get a transform value for the given tag
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @return The transform value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    FTransform GetTransformValue(FGameplayTag Tag, const FTransform& DefaultValue = FTransform::Identity, UObject* Context = nullptr) const;
    
    /**
     * Set a transform value for the given tag
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    bool SetTransformValue(FGameplayTag Tag, const FTransform& Value, FName RepositoryName = NAME_None);
    
    /**
     * Get a class value for the given tag
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @return The class value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    FSoftClassPath GetClassValue(FGameplayTag Tag, const FSoftClassPath& DefaultValue = FSoftClassPath(), UObject* Context = nullptr) const;
    
    /**
     * Set a class value for the given tag
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    bool SetClassValue(FGameplayTag Tag, const FSoftClassPath& Value, FName RepositoryName = NAME_None);
    
    /**
     * Get an object value for the given tag
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @return The object value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    FSoftObjectPath GetObjectValue(FGameplayTag Tag, const FSoftObjectPath& DefaultValue = FSoftObjectPath(), UObject* Context = nullptr) const;
    
    /**
     * Set an object value for the given tag
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    bool SetObjectValue(FGameplayTag Tag, const FSoftObjectPath& Value, FName RepositoryName = NAME_None);
    
    /**
     * Get a raw value holder for the given tag
     * @param Tag The tag to get the value for
     * @param Context Optional context object that implements UTagValueInterface
     * @return The value holder, or nullptr if not found
     */
    TSharedPtr<ITagValueHolder> GetRawValue(FGameplayTag Tag, UObject* Context = nullptr) const;
    
    /**
     * Set a raw value holder for the given tag
     * @param Tag The tag to set the value for
     * @param Value The value holder to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    bool SetRawValue(FGameplayTag Tag, TSharedPtr<ITagValueHolder> Value, FName RepositoryName = NAME_None);
    
    /**
     * Remove a value for the given tag
     * @param Tag The tag to remove the value for
     * @param RepositoryName Optional repository name to target (removes from all if not specified)
     * @return True if the value was removed successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    bool RemoveTagValue(FGameplayTag Tag, FName RepositoryName = NAME_None);
    
    /**
     * Clear all values in all repositories or a specific repository
     * @param RepositoryName Optional repository name to target (clears all if not specified)
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    void ClearAllValues(FName RepositoryName = NAME_None);
    
    /**
     * Get all tags that have values in any repository
     * @return Array of all tags with values
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    TArray<FGameplayTag> GetAllTags() const;
    
    /**
     * Import tag values from a data table
     * @param DataTable The data table to import from
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return Number of tag-value pairs imported
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    int32 ImportFromDataTable(UDataTable* DataTable, FName RepositoryName = NAME_None);
    
    /**
     * Export tag values to a data table
     * @param DataTable The data table to export to
     * @param RepositoryName Optional repository name to target (exports from all if not specified)
     * @return Number of tag-value pairs exported
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    int32 ExportToDataTable(UDataTable* DataTable, FName RepositoryName = NAME_None);
    
    /**
     * Register all GameplayTagValueDataAssets that are configured to auto-register
     * This is called automatically during initialization
     * @return Number of data assets registered
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    int32 RegisterConfiguredDataAssets();
    
    /**
     * Broadcast that a tag value has changed
     * @param Tag The tag that changed
     * @param RepositoryName The repository where the change occurred
     * @param OldValue The previous value (may be null if no previous value existed)
     * @param NewValue The new value (may be null if the value was removed)
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    void BroadcastTagValueChanged(FGameplayTag Tag, FName RepositoryName, const TSharedPtr<ITagValueHolder>& OldValue = nullptr, const TSharedPtr<ITagValueHolder>& NewValue = nullptr);
    
private:
    /** The default repository name */
    static const FName DefaultRepositoryName;
    
    /** The repositories, sorted by priority */
    TArray<TSharedPtr<ITagValueRepository>> Repositories;
    
    /** Get the best repository for setting values */
    TSharedPtr<ITagValueRepository> GetBestRepository(FName RepositoryName = NAME_None) const;
    
    /** Check if an object implements the UTagValueInterface */
    bool ImplementsTagValueInterface(UObject* Object) const;
    
    /** Get the ITagValueInterface from an object */
    ITagValueInterface* GetTagValueInterface(UObject* Object) const;
    
    /** Try to get a value from a context object */
    template<typename T>
    bool TryGetValueFromContext(UObject* Context, FGameplayTag Tag, T& OutValue, const T& DefaultValue) const;
    
    /** Try to get a value from the repositories */
    template<typename T>
    bool TryGetValueFromRepositories(FGameplayTag Tag, T& OutValue) const;
    
    /** Helper function for getting a typed value */
    template<typename T>
    T GetTypedValue(FGameplayTag Tag, const T& DefaultValue, UObject* Context) const;
    
    /** Helper function for setting a typed value */
    template<typename T>
    bool SetTypedValue(FGameplayTag Tag, const T& Value, FName RepositoryName);
};