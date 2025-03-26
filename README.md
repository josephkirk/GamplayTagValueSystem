# GameplayTag Value System

## Overview

The GameplayTag Value System extends Unreal Engine's GameplayTags with the ability to store arbitrary values. This system combines the flexibility of GameplayTags with type-safe value storage using a subsystem approach and modern C++ design patterns, focusing on performance, maintainability, and seamless engine integration.

## Features

- **Inheritance-based type safety** for associating values with GameplayTags
- **Multiple value types** support including:
  - Primitive types (bool, int, float)
  - Strings
  - Transforms
  - Class references (via TSoftClassPtr)
  - Object references (via TSoftObjectPtr)
- **Repository pattern** for flexible storage backends
- **Actor Component implementation** for easy integration with actors
- **Hierarchical tag inheritance** for logical data organization
- **Contextual tag values** via UTagValueInterface for actor-specific overrides
- **Data Table integration** for configuration-driven workflows
- **Blueprint-friendly API** with comprehensive function library
- **Data asset support** for easy management of tag-value collections
- **Custom Blueprint nodes** for type-safe tag value operations in Blueprints
- **Multi-repository architecture** with priority-based value resolution

## Architecture

### Core Components

#### FBaseTagValue Inheritance System
- Base abstract struct for all tag value types
- Concrete type implementations for each supported value type
- Type-safe access through derived type knowledge
- Virtual methods for type checking and serialization

#### GameplayTagValueSubsystem
- Central management system (UGameInstanceSubsystem)
- Provides public API for getting/setting values
- Manages repositories and handles tag value inheritance
- Supports event-based notification via delegates

#### TagValueRepository
- Implementation of ITagValueRepository interface
- Handles storage and retrieval of tag values
- Memory-based implementation (FMemoryTagValueRepository)
- Component-based implementation (UTagValueRepositoryComponent)
- Prioritization system for repository chain
- Default repositories: Config (200), Default (100), Runtime (50)

#### TagValueContainer
- Type-safe container for holding and managing tag values
- Strongly-typed getters and setters for all value types
- Support for serialization and copying

#### Blueprint Function Library
- Type-safe Blueprint API via UGameplayTagValueFunctionLibrary
- Helper functions for all supported value types
- World context object pattern for easy access

#### Custom Blueprint Nodes
- UK2Node_GetGameplayTagValue for retrieving values with automatic type resolution
- UK2Node_SetGameplayTagValue for setting values with automatic type resolution
- Automatically registered during module startup

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
                       |                             |
        +---------------+---------------+   +--------v---------+
        |               |               |   |                  |
+-------v-------+  +---v---+  +--------v--+ | Custom Blueprint |
|               |  |       |  |           | | Nodes            |
| Config Repo   |  |Default|  | Runtime   | |                  |
| (Priority 200)|  |(100)  |  | (50)      | +------------------+
+---------------+  +-------+  +-----------+
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
Subsystem->SetStringValue(FGameplayTag::RequestGameplayTag("MyStringTag"), "Hello World");

// Get values with default fallbacks
bool BoolValue = Subsystem->GetBoolValue(FGameplayTag::RequestGameplayTag("Character.CanJump"), false);
int32 MaxHealth = Subsystem->GetIntValue(FGameplayTag::RequestGameplayTag("Character.MaxHealth"), 100);
float JumpHeight = Subsystem->GetFloatValue(FGameplayTag::RequestGameplayTag("Character.JumpHeight"), 300.0f);
FString CharacterName = Subsystem->GetStringValue(FGameplayTag::RequestGameplayTag("Character.Name"), "DefaultName");

// Use context objects
int32 ContextHealth = Subsystem->GetIntValue(
    FGameplayTag::RequestGameplayTag("Character.Health"),
    0, // Default value
    MyCharacter // Context object implementing UTagValueInterface
);

// Using the FTagValueContainer directly
FTagValueContainer Container;
Container.SetBoolValue(FGameplayTag::RequestGameplayTag("MyBoolTag"), true);
Container.SetIntValue(FGameplayTag::RequestGameplayTag("MyIntTag"), 42);

bool MyBool = Container.GetBoolValue(FGameplayTag::RequestGameplayTag("MyBoolTag"), false);
int32 MyInt = Container.GetIntValue(FGameplayTag::RequestGameplayTag("MyIntTag"), 0);

// Specify a repository when setting values
Subsystem->SetBoolValue(FGameplayTag::RequestGameplayTag("Config.Feature.Enabled"), true, "Config");
Subsystem->SetIntValue(FGameplayTag::RequestGameplayTag("Runtime.Player.Score"), 1000, "Runtime");
```

### Blueprint Usage

1. Get the GameplayTagValueSubsystem via the function library
2. Use the Blueprint function library to get/set tag values
3. Use the custom "Get Gameplay Tag Value" and "Set Gameplay Tag Value" nodes for automatic type resolution
4. Implement UTagValueInterface on actors or components for contextual values
5. Use data assets to configure and load tag values from data tables
6. Add TagValueRepositoryComponent to actors to store tag values directly

### Custom Blueprint Nodes

The system provides two custom Blueprint nodes for working with tag values:

- **Get Gameplay Tag Value**: Retrieves a value for a specified tag with automatic type resolution based on the connected output pin
- **Set Gameplay Tag Value**: Sets a value for a specified tag with automatic type resolution based on the connected input pin

These nodes automatically determine the value type based on the pins you connect, making it easy to work with tag values of different types without having to select the correct function.

### Using TagValueRepositoryComponent

```cpp
// Add the component to an actor
UTagValueRepositoryComponent* TagRepo = MyActor->CreateDefaultSubobject<UTagValueRepositoryComponent>(TEXT("TagValueRepository"));

// Configure the component
TagRepo->RepositoryName = FName(TEXT("MyActorRepository"));
TagRepo->Priority = 100;
TagRepo->bRegisterToSubsystem = true;

// Add tag values directly
TagRepo->SetBoolValue(FGameplayTag::RequestGameplayTag("MyActor.CanJump"), true);
TagRepo->SetIntValue(FGameplayTag::RequestGameplayTag("MyActor.MaxHealth"), 100);
TagRepo->SetStringValue(FGameplayTag::RequestGameplayTag("MyActor.Name"), "PlayerCharacter");

// Or in Blueprint, add the component and configure tag values in the editor
```

### Data Table Integration

Create a DataTable with the FTagValueDataTableRow structure as the row type, then:

```cpp
// Create a data asset that references the data table
UGameplayTagValueDataAsset* DataAsset = NewObject<UGameplayTagValueDataAsset>(GetTransientPackage());
DataAsset->DataTables.Add(MyDataTable);
DataAsset->RepositoryName = "MyRepository";
DataAsset->bAutoRegister = true; // Will automatically register on game start

// Register with the subsystem manually if needed
DataAsset->RegisterToSubsystem(Subsystem);
```

## Repository System

The system uses a priority-based repository architecture:

1. **Config Repository** (Priority 200): For configuration-based values that should override defaults
2. **Default Repository** (Priority 100): The built-in default repository for general use
3. **Runtime Repository** (Priority 50): For values that change during gameplay

When getting a value, the system checks repositories in order of priority (highest first). When setting a value without specifying a repository, it uses the highest priority repository available.

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
    virtual float GetFloatValue(FGameplayTag Tag, float DefaultValue = 0.0f) const override;
    virtual FString GetStringValue(FGameplayTag Tag, const FString& DefaultValue = "") const override;
    virtual FTransform GetTransformValue(FGameplayTag Tag, const FTransform& DefaultValue = FTransform::Identity) const override;
    virtual TSoftClassPtr<UObject> GetClassValue(FGameplayTag Tag, TSoftClassPtr<UObject> DefaultValue = nullptr) const override;
    virtual TSoftObjectPtr<UObject> GetObjectValue(FGameplayTag Tag, TSoftObjectPtr<UObject> DefaultValue = nullptr) const override;
};
```

## Value Types

The system supports the following value types through inheritance:

```cpp
// Base abstract struct
struct FBaseTagValue
{
    virtual ~FBaseTagValue() {}
    virtual ETagValueType GetType() const = 0;
    virtual bool Equals(const FBaseTagValue& Other) const = 0;
    // Other virtual methods...
};

// Concrete implementations
struct FBoolTagValue : public FBaseTagValue
{
    bool Value;
    // Implementation...
};

struct FIntTagValue : public FBaseTagValue
{
    int32 Value;
    // Implementation...
};

struct FFloatTagValue : public FBaseTagValue
{
    float Value;
    // Implementation...
};

struct FStringTagValue : public FBaseTagValue
{
    FString Value;
    // Implementation...
};

// And so on for other types...
```

## Module Initialization

The module automatically performs the following setup during initialization:

1. Registers custom Blueprint nodes for tag value operations
2. Creates and registers the default repository structure (Config, Default, Runtime)
3. Loads and registers any data assets configured for auto-registration

This ensures the system is ready to use as soon as the game starts without requiring manual setup.
