# GameplayTag Value System

## Overview

The GameplayTag Value System extends Unreal Engine's GameplayTags with the ability to store arbitrary values. This system combines the flexibility of GameplayTags with type-safe value storage using a subsystem approach and modern C++ design patterns, focusing on performance, maintainability, and seamless engine integration.

## Features

- **Type-safe storage** for associating values with GameplayTags
- **Multiple value types** support including:
  - Primitive types (bool, int, float)
  - Transforms
  - Class references (via FSoftClassPath)
  - Object references (via FSoftObjectPath)
- **Repository pattern** for flexible storage backends
- **Hierarchical tag inheritance** for logical data organization
- **Contextual tag values** via UTagValueInterface for actor-specific overrides
- **Data Table integration** for configuration-driven workflows
- **Blueprint-friendly API** with comprehensive function library
- **Data asset support** for easy management of tag-value collections

## Architecture

### Core Components

#### GameplayTagValueSubsystem
- Central management system (UGameInstanceSubsystem)
- Provides public API for getting/setting values
- Manages repositories and handles tag value inheritance
- Supports event-based notification via delegates

#### TagValueRepository
- Implementation of ITagValueRepository interface
- Handles storage and retrieval of tag values
- Memory-based implementation (FMemoryTagValueRepository)
- Prioritization system for repository chain

#### TagValueHolder
- Type erasure implementation via ITagValueHolder interface
- Template specializations for different value types
- Memory-efficient design with value copying

#### Blueprint Function Library
- Type-safe Blueprint API via UGameplayTagValueFunctionLibrary
- Helper functions for all supported value types
- World context object pattern for easy access

### System Diagram

```
+-------------------------+    +------------------------+
| Game Systems            |    | Editor Systems         |
| (Actors, Components)    |    | (Data Tables, Editor)  |
+----------+--------------+    +----------+-------------+
           |                              |
           v                              v
+----------+------------------------------+-------------+
|                                                      |
|              GameplayTagValueSubsystem               |
|                                                      |
+----------------------+-----------------------------+-+
                       |                             |
            +----------v----------+     +------------v-----------+
            |                     |     |                        |
            | ITagValueRepository |     | TagValue Blueprint API |
            |                     |     |                        |
            +---------------------+     +------------------------+
                       |
                +------v------+
                |             |
                | Memory Repo |
                |             |
                +-------------+
```

## Usage

### C++ Usage

```cpp
// Get the subsystem
UGameplayTagValueSubsystem* Subsystem = UGameplayTagValueSubsystem::Get(GetWorld());

// Set values
Subsystem->SetBoolValue(FGameplayTag::RequestGameplayTag("MyBoolTag"), true);
Subsystem->SetIntValue(FGameplayTag::RequestGameplayTag("MyIntTag"), 10);
Subsystem->SetFloatValue(FGameplayTag::RequestGameplayTag("MyFloatTag"), 3.14f);

// Get values
bool BoolValue;
int IntValue;
float FloatValue;

Subsystem->GetBoolValue(FGameplayTag::RequestGameplayTag("MyBoolTag"), BoolValue);
Subsystem->GetIntValue(FGameplayTag::RequestGameplayTag("MyIntTag"), IntValue);
Subsystem->GetFloatValue(FGameplayTag::RequestGameplayTag("MyFloatTag"), FloatValue);

// Get values
bool BoolValue = Subsystem->GetBoolValue(FGameplayTag::RequestGameplayTag("Character.CanJump"));
int32 MaxHealth = Subsystem->GetIntValue(FGameplayTag::RequestGameplayTag("Character.MaxHealth"));
float JumpHeight = Subsystem->GetFloatValue(FGameplayTag::RequestGameplayTag("Character.JumpHeight"));

// Use context objects
int32 ContextHealth = Subsystem->GetIntValue(
    FGameplayTag::RequestGameplayTag("Character.Health"),
    0, // Default value
    MyCharacter // Context object implementing UTagValueInterface
);
```

### Blueprint Usage

1. Get the GameplayTagValueSubsystem via the function library
2. Use the Blueprint function library to get/set tag values
3. Implement UTagValueInterface on actors or components for contextual values
4. Use data assets to configure and load tag values from data tables

### Data Table Integration

```cpp
// Import values from a data table
UDataTable* MyDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/MyDataTable"));
Subsystem->ImportFromDataTable(MyDataTable);

// Export values to a data table
Subsystem->ExportToDataTable(MyDataTable);
```

## Implementing UTagValueInterface

To provide contextual tag values, implement the UTagValueInterface on your actor or component:

```cpp
UCLASS()
class AMyCharacter : public ACharacter, public ITagValueInterface
{
    GENERATED_BODY()

public:
    // UTagValueInterface implementation
    virtual bool HasTagValue(FGameplayTag Tag) const override;
    virtual bool GetBoolValue(FGameplayTag Tag, bool DefaultValue = false) const override;
    virtual int32 GetIntValue(FGameplayTag Tag, int32 DefaultValue = 0) const override;
    virtual double GetFloatValue(FGameplayTag Tag, double DefaultValue = 0.0) const override;
    virtual FTransform GetTransformValue(FGameplayTag Tag, const FTransform& DefaultValue = FTransform::Identity) const override;
    virtual FSoftClassPath GetClassValue(FGameplayTag Tag, const FSoftClassPath& DefaultValue = FSoftClassPath()) const override;
    virtual FSoftObjectPath GetObjectValue(FGameplayTag Tag, const FSoftObjectPath& DefaultValue = FSoftObjectPath()) const override;
};
```

## Data Flow

### Value Setting Flow

1. User calls SetTagValue through Blueprint or C++
2. Subsystem validates tag and value
3. Value is converted to appropriate TTagValueHolder
4. Value is stored in the target repository
5. Change notification is broadcast to observers

### Value Retrieval Flow

1. User requests value via GetTagValue
2. Subsystem checks if context object implements UTagValueInterface
3. If context object has the value, return it
4. Otherwise, check repositories in priority order
5. If not found, check parent tags (hierarchical inheritance)
6. If still not found, return default value

## Technical Implementation Details

### Type Erasure Pattern

```cpp
class ITagValueHolder
{
public:
    virtual ~ITagValueHolder() = default;
    virtual void* GetValuePtr() = 0;
    virtual FName GetValueTypeName() const = 0;
    virtual TSharedPtr<ITagValueHolder> Clone() const = 0;
    virtual bool IsValid() const = 0;
};

template<typename T>
class TTagValueHolder : public ITagValueHolder
{
public:
    TTagValueHolder(const T& InValue) : Value(InValue) {}
    
    void* GetValuePtr() override { return &Value; }
    FName GetValueTypeName() const override { return TBaseStructure<T>::Get()->GetFName(); }
    TSharedPtr<ITagValueHolder> Clone() const override { return MakeShared<TTagValueHolder<T>>(Value); }
    bool IsValid() const override { return true; }
    
    T Value;
};
```

### Repository Pattern

```cpp
class ITagValueRepository
{
public:
    virtual ~ITagValueRepository() = default;
    virtual bool HasValue(FGameplayTag Tag) const = 0;
    virtual TSharedPtr<ITagValueHolder> GetValue(FGameplayTag Tag) const = 0;
    virtual void SetValue(FGameplayTag Tag, TSharedPtr<ITagValueHolder> Value) = 0;
    virtual void RemoveValue(FGameplayTag Tag) = 0;
    virtual void ClearAllValues() = 0;
    virtual TArray<FGameplayTag> GetAllTags() const = 0;
    virtual FName GetRepositoryName() const = 0;
    virtual int32 GetPriority() const = 0;
};
```

### Hierarchical Inheritance

```cpp
bool UGameplayTagValueSubsystem::GetTagValue(FGameplayTag Tag, TSharedPtr<ITagValueHolder>& OutValue)
{
    // Check for exact match first
    if (TryGetValueFromRepositories(Tag, OutValue))
    {
        return true;
    }
    
    // Check parent tags (hierarchical fallback)
    FGameplayTag ParentTag = Tag.RequestDirectParent();
    while (ParentTag.IsValid())
    {
        if (TryGetValueFromRepositories(ParentTag, OutValue))
        {
            return true;
        }
        ParentTag = ParentTag.RequestDirectParent();
    }
    
    return false;
}
```

### Blueprint Integration

```cpp
class UGameplayTagValueFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
    UFUNCTION(BlueprintPure, Category = "Gameplay Tags|Values", meta = (WorldContext = "WorldContextObject"))
    static bool GetBoolTagValue(const UObject* WorldContextObject, FGameplayTag Tag, bool DefaultValue = false, UObject* Context = nullptr);
    
    // Similar for other types
};
```

## Documentation

### API Documentation
- Public API reference
- Type conversion rules
- Blueprint node documentation
- Repository management

### Usage Guides
- Getting started guide
- Data Table setup guide
- Best practices and performance tips
- Migration guide from primitive systems

### Architecture Documentation
- System overview
- Component diagrams
- Extension points
- Repository implementation guide

## Future Roadmap

- Additional repository implementations (SaveGame, Config, etc.)
- Enhanced editor tools for tag value management
- Runtime replication support for multiplayer
- Integration with Gameplay Ability System
- Performance optimizations for large-scale usage
