// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTags.h"
#include "TagValueBase.h"
#include "TagValueContainer.h"
#include "TagValueInterface.generated.h"

/**
 * Interface for the type erasure pattern that holds different value types
 * This is used to provide a common interface for storing and retrieving
 * values of different types associated with gameplay tags
 */
class GAMPLAYTAGVALUE_API ITagValueHolder
{
public:
    virtual ~ITagValueHolder() = default;
    
    /** Get a pointer to the raw value */
    virtual void* GetValuePtr() = 0;
    
    /** Get the name of the value type */
    virtual FName GetValueTypeName() const = 0;
    
    /** Create a copy of this value holder */
    virtual TSharedPtr<ITagValueHolder> Clone() const = 0;
    
    /** Check if this value holder contains a valid value */
    virtual bool IsValid() const = 0;
};

/**
 * Template implementation of ITagValueHolder for specific types
 * Provides type-safe access to the contained value
 */
template<typename T>
class GAMPLAYTAGVALUE_API TTagValueHolder : public ITagValueHolder
{
public:
    TTagValueHolder(const T& InValue) : Value(InValue) {}
    
    /** Get a pointer to the raw value */
    virtual void* GetValuePtr() override { return &Value; }
    
    /** Get the name of the value type */
    virtual FName GetValueTypeName() const override { return TBaseStructure<T>::Get()->GetFName(); }
    
    /** Create a copy of this value holder */
    virtual TSharedPtr<ITagValueHolder> Clone() const override
    {
        return MakeShared<TTagValueHolder<T>>(Value);
    }
    
    /** Check if this value holder contains a valid value */
    virtual bool IsValid() const override { return true; }
    
    /** The actual value being held */
    T Value;
};

/**
 * Repository interface for storing and retrieving tag values
 * Different implementations can store values in different backends
 * (memory, data tables, save games, etc.)
 */
class GAMPLAYTAGVALUE_API ITagValueRepository
{
public:
    virtual ~ITagValueRepository() = default;
    
    /** Check if a value exists for the given tag */
    virtual bool HasValue(FGameplayTag Tag) const = 0;
    
    /** Get the value for the given tag */
    virtual TSharedPtr<ITagValueHolder> GetValue(FGameplayTag Tag) const = 0;
    
    /** Set the value for the given tag */
    virtual void SetValue(FGameplayTag Tag, TSharedPtr<ITagValueHolder> Value) = 0;
    
    /** Remove the value for the given tag */
    virtual void RemoveValue(FGameplayTag Tag) = 0;
    
    /** Clear all values in this repository */
    virtual void ClearAllValues() = 0;
    
    /** Get all tags in this repository */
    virtual TArray<FGameplayTag> GetAllTags() const = 0;
    
    /** Get the name of this repository */
    virtual FName GetRepositoryName() const = 0;
    
    /** Get the priority of this repository (higher priority repositories are checked first) */
    virtual int32 GetPriority() const = 0;
};

/**
 * Interface for objects that can provide tag values directly
 * This can be implemented by actors, components, etc. to provide
 * contextual tag values specific to that object
 */
UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class GAMPLAYTAGVALUE_API UTagValueInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Implementation class for the UTagValueInterface interface
 */
class GAMPLAYTAGVALUE_API ITagValueInterface
{
    GENERATED_BODY()
    
public:
    /** Check if this object has a value for the given tag */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    virtual bool HasTagValue(FGameplayTag Tag) const = 0;
    
    /** Get a bool value for the given tag */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    virtual bool GetBoolValue(FGameplayTag Tag, bool DefaultValue = false) const = 0;
    
    /** Get an integer value for the given tag */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    virtual int32 GetIntValue(FGameplayTag Tag, int32 DefaultValue = 0) const = 0;
    
    /** Get a float value for the given tag */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    virtual float GetFloatValue(FGameplayTag Tag, float DefaultValue = 0.0f) const = 0;
    
    /** Get a string value for the given tag */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    virtual FString GetStringValue(FGameplayTag Tag, const FString& DefaultValue = "") const = 0;
    
    /** Get a transform value for the given tag */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    virtual FTransform GetTransformValue(FGameplayTag Tag, const FTransform& DefaultValue) const = 0;
    
    /** Get a class value for the given tag */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    virtual TSoftClassPtr<UObject> GetClassValue(FGameplayTag Tag, TSoftClassPtr<UObject> DefaultValue = nullptr) const = 0;
    
    /** Get an object value for the given tag */
    UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
    virtual TSoftObjectPtr<UObject> GetObjectValue(FGameplayTag Tag, TSoftObjectPtr<UObject> DefaultValue = nullptr) const = 0;
};
