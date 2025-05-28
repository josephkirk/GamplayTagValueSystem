// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.#include "GameplayTagValueFunctionLibrary.h"
#include "GameplayTagValueSubsystem.h"
#include "Kismet/GameplayStatics.h"

UGameplayTagValueSubsystem* UGameplayTagValueFunctionLibrary::GetGameplayTagValueSubsystem(const UObject* WorldContextObject)
{
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
    if (!GameInstance)
    {
        return nullptr;
    }
    
    return GameInstance->GetSubsystem<UGameplayTagValueSubsystem>();
}

bool UGameplayTagValueFunctionLibrary::HasTagValue(const UObject* WorldContextObject, FGameplayTag Tag, UObject* Context)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->HasTagValue(Tag, Context);
}

bool UGameplayTagValueFunctionLibrary::GetBoolTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, bool DefaultValue, UObject* Context)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        bSuccess = false;
        return DefaultValue;
    }
    
    bSuccess = Subsystem->HasTagValue(Tag, Context);
    return Subsystem->GetBoolValue(Tag, DefaultValue, Context);
}

bool UGameplayTagValueFunctionLibrary::SetBoolTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool Value, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->SetBoolValue(Tag, Value, RepositoryName);
}

int32 UGameplayTagValueFunctionLibrary::GetIntTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, int32 DefaultValue, UObject* Context)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        bSuccess = false;
        return DefaultValue;
    }
    
    bSuccess = Subsystem->HasTagValue(Tag, Context);
    return Subsystem->GetIntValue(Tag, DefaultValue, Context);
}

bool UGameplayTagValueFunctionLibrary::SetIntTagValue(const UObject* WorldContextObject, FGameplayTag Tag, int32 Value, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->SetIntValue(Tag, Value, RepositoryName);
}

float UGameplayTagValueFunctionLibrary::GetFloatTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, float DefaultValue, UObject* Context)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        bSuccess = false;
        return DefaultValue;
    }
    
    bSuccess = Subsystem->HasTagValue(Tag, Context);
    return Subsystem->GetFloatValue(Tag, DefaultValue, Context);
}

bool UGameplayTagValueFunctionLibrary::SetFloatTagValue(const UObject* WorldContextObject, FGameplayTag Tag, float Value, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->SetFloatValue(Tag, Value, RepositoryName);
}

FString UGameplayTagValueFunctionLibrary::GetStringTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, const FString& DefaultValue, UObject* Context)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        bSuccess = false;
        return DefaultValue;
    }
    
    bSuccess = Subsystem->HasTagValue(Tag, Context);
    return Subsystem->GetStringValue(Tag, DefaultValue, Context);
}

bool UGameplayTagValueFunctionLibrary::SetStringTagValue(const UObject* WorldContextObject, FGameplayTag Tag, const FString& Value, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->SetStringValue(Tag, Value, RepositoryName);
}

FTransform UGameplayTagValueFunctionLibrary::GetTransformTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, const FTransform& DefaultValue, UObject* Context)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        bSuccess = false;
        return DefaultValue;
    }
    
    bSuccess = Subsystem->HasTagValue(Tag, Context);
    return Subsystem->GetTransformValue(Tag, DefaultValue, Context);
}

bool UGameplayTagValueFunctionLibrary::SetTransformTagValue(const UObject* WorldContextObject, FGameplayTag Tag, const FTransform& Value, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->SetTransformValue(Tag, Value, RepositoryName);
}

TSoftClassPtr<UObject> UGameplayTagValueFunctionLibrary::GetClassTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, TSoftClassPtr<UObject> DefaultValue, UObject* Context)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        bSuccess = false;
        return DefaultValue;
    }
    
    bSuccess = Subsystem->HasTagValue(Tag, Context);
    return Subsystem->GetClassValue(Tag, DefaultValue, Context);
}

bool UGameplayTagValueFunctionLibrary::SetClassTagValue(const UObject* WorldContextObject, FGameplayTag Tag, TSoftClassPtr<UObject> Value, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->SetClassValue(Tag, Value, RepositoryName);
}

TSoftObjectPtr<UObject> UGameplayTagValueFunctionLibrary::GetObjectTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool& bSuccess, TSoftObjectPtr<UObject> DefaultValue, UObject* Context)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        bSuccess = false;
        return DefaultValue;
    }
    
    bSuccess = Subsystem->HasTagValue(Tag, Context);
    return Subsystem->GetObjectValue(Tag, DefaultValue, Context);
}

bool UGameplayTagValueFunctionLibrary::SetObjectTagValue(const UObject* WorldContextObject, FGameplayTag Tag, TSoftObjectPtr<UObject> Value, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->SetObjectValue(Tag, Value, RepositoryName);
}

bool UGameplayTagValueFunctionLibrary::RemoveTagValue(const UObject* WorldContextObject, FGameplayTag Tag, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->RemoveTagValue(Tag, RepositoryName);
}

bool UGameplayTagValueFunctionLibrary::ClearAllTagValues(const UObject* WorldContextObject, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    Subsystem->ClearAllValues(RepositoryName);
    return true;
}
