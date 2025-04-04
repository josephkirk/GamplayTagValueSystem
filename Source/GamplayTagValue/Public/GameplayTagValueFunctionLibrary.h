#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTags.h"
#include "TagValueBase.h"
#include "TagValueContainer.h"
#include "GameplayTagValueFunctionLibrary.generated.h"

/**
 * Blueprint Function Library for the GameplayTag Value System
 * Provides convenient access to the GameplayTagValueSubsystem from Blueprints
 */
UCLASS()
class GAMPLAYTAGVALUE_API UGameplayTagValueFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Get the GameplayTagValueSubsystem from the current world context
     * @param WorldContextObject World context object
     * @return The GameplayTagValueSubsystem, or nullptr if not found
     */
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static class UGameplayTagValueSubsystem* GetGameplayTagValueSubsystem(const UObject* WorldContextObject);
    
    /**
     * Check if a value exists for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to check
     * @param Context Optional context object that implements UTagValueInterface
     * @return True if a value exists for the tag
     */
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool HasTagValue(const UObject* WorldContextObject, FGameplayTag Tag, UObject* Context = nullptr);
    
    /**
     * Get a bool value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @param bSuccess Output parameter indicating if the tag had a valid value
     * @return The bool value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool GetBoolTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, bool DefaultValue = false, UObject* Context = nullptr);
    
    /**
     * Set a bool value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool SetBoolTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool Value, FName RepositoryName = NAME_None);
    
    /**
     * Get an integer value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @param bSuccess Output parameter indicating if the tag had a valid value
     * @return The integer value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static int32 GetIntTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, int32 DefaultValue = 0, UObject* Context = nullptr);
    
    /**
     * Set an integer value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool SetIntTagValue(const UObject* WorldContextObject, FGameplayTag Tag, int32 Value, FName RepositoryName = NAME_None);
    
    /**
     * Get a float value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @param bSuccess Output parameter indicating if the tag had a valid value
     * @return The float value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static float GetFloatTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, float DefaultValue = 0.0f, UObject* Context = nullptr);
    
    /**
     * Set a float value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool SetFloatTagValue(const UObject* WorldContextObject, FGameplayTag Tag, float Value, FName RepositoryName = NAME_None);
    
    /**
     * Get a string value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @param bSuccess Output parameter indicating if the tag had a valid value
     * @return The string value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static FString GetStringTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, const FString& DefaultValue = "", UObject* Context = nullptr);
    
    /**
     * Set a string value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool SetStringTagValue(const UObject* WorldContextObject, FGameplayTag Tag, const FString& Value, FName RepositoryName = NAME_None);
    
    /**
     * Get a transform value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @param bSuccess Output parameter indicating if the tag had a valid value
     * @return The transform value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static FTransform GetTransformTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, const FTransform& DefaultValue, UObject* Context = nullptr);
    
    /**
     * Set a transform value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool SetTransformTagValue(const UObject* WorldContextObject, FGameplayTag Tag, const FTransform& Value, FName RepositoryName = NAME_None);
    
    /**
     * Get a class value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @param bSuccess Output parameter indicating if the tag had a valid value
     * @return The class value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static TSoftClassPtr<UObject> GetClassTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, TSoftClassPtr<UObject> DefaultValue, UObject* Context = nullptr);
    
    /**
     * Set a class value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool SetClassTagValue(const UObject* WorldContextObject, FGameplayTag Tag, TSoftClassPtr<UObject> Value, FName RepositoryName = NAME_None);
    
    /**
     * Get an object value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to get the value for
     * @param DefaultValue The default value to return if the tag is not found
     * @param Context Optional context object that implements UTagValueInterface
     * @param bSuccess Output parameter indicating if the tag had a valid value
     * @return The object value associated with the tag, or DefaultValue if not found
     */
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static TSoftObjectPtr<UObject> GetObjectTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, TSoftObjectPtr<UObject> DefaultValue, UObject* Context = nullptr);
    
    /**
     * Set an object value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to set the value for
     * @param Value The value to set
     * @param RepositoryName Optional repository name to target (uses default if not specified)
     * @return True if the value was set successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool SetObjectTagValue(const UObject* WorldContextObject, FGameplayTag Tag, TSoftObjectPtr<UObject> Value, FName RepositoryName = NAME_None);
    
    /**
     * Remove a value for the given tag
     * @param WorldContextObject World context object
     * @param Tag The tag to remove the value for
     * @param RepositoryName Optional repository name to target (removes from all if not specified)
     * @return True if the value was removed successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool RemoveTagValue(const UObject* WorldContextObject, FGameplayTag Tag, FName RepositoryName = NAME_None);
    
    /**
     * Clear all values for the specified repository
     * @param WorldContextObject World context object
     * @param RepositoryName The repository name to clear (clears all if not specified)
     * @return True if values were cleared successfully
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool ClearAllTagValues(const UObject* WorldContextObject, FName RepositoryName = NAME_None);
};
