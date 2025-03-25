#include "GameplayTagValueFunctionLibrary.h"
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

bool UGameplayTagValueFunctionLibrary::GetBoolTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool DefaultValue, UObject* Context, bool& bSuccess)
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

int32 UGameplayTagValueFunctionLibrary::GetIntTagValue(const UObject* WorldContextObject, FGameplayTag Tag, int32 DefaultValue, UObject* Context, bool& bSuccess)
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

float UGameplayTagValueFunctionLibrary::GetFloatTagValue(const UObject* WorldContextObject, FGameplayTag Tag, float DefaultValue, UObject* Context, bool& bSuccess)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        bSuccess = false;
        return DefaultValue;
    }
    
    bSuccess = Subsystem->HasTagValue(Tag, Context);
    return static_cast<float>(Subsystem->GetFloatValue(Tag, DefaultValue, Context));
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

FTransform UGameplayTagValueFunctionLibrary::GetTransformTagValue(const UObject* WorldContextObject, FGameplayTag Tag, const FTransform& DefaultValue, UObject* Context, bool& bSuccess)
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

TSoftClassPtr<UObject> UGameplayTagValueFunctionLibrary::GetClassTagValue(const UObject* WorldContextObject, FGameplayTag Tag, TSoftClassPtr<UObject> DefaultValue, UObject* Context, bool& bSuccess)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        bSuccess = false;
        return DefaultValue;
    }
    
    bSuccess = Subsystem->HasTagValue(Tag, Context);
    FSoftClassPath ClassPath = Subsystem->GetClassValue(Tag, FSoftClassPath(DefaultValue.ToString()), Context);
    return TSoftClassPtr<UObject>(ClassPath.ToString());
}

bool UGameplayTagValueFunctionLibrary::SetClassTagValue(const UObject* WorldContextObject, FGameplayTag Tag, TSoftClassPtr<UObject> Value, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->SetClassValue(Tag, FSoftClassPath(Value.ToString()), RepositoryName);
}

TSoftObjectPtr<UObject> UGameplayTagValueFunctionLibrary::GetObjectTagValue(const UObject* WorldContextObject, FGameplayTag Tag, TSoftObjectPtr<UObject> DefaultValue, UObject* Context, bool& bSuccess)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        bSuccess = false;
        return DefaultValue;
    }
    
    bSuccess = Subsystem->HasTagValue(Tag, Context);
    FSoftObjectPath ObjectPath = Subsystem->GetObjectValue(Tag, FSoftObjectPath(DefaultValue.ToString()), Context);
    return TSoftObjectPtr<UObject>(ObjectPath.ToString());
}

bool UGameplayTagValueFunctionLibrary::SetObjectTagValue(const UObject* WorldContextObject, FGameplayTag Tag, TSoftObjectPtr<UObject> Value, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return false;
    }
    
    return Subsystem->SetObjectValue(Tag, FSoftObjectPath(Value.ToString()), RepositoryName);
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

void UGameplayTagValueFunctionLibrary::ClearAllTagValues(const UObject* WorldContextObject, FName RepositoryName)
{
    UGameplayTagValueSubsystem* Subsystem = GetGameplayTagValueSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        return;
    }
    
    Subsystem->ClearAllValues(RepositoryName);
}
