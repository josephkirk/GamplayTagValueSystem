# Using UScriptStruct for a Generic TagValueSystem in Unreal Engine

## Overview

A TagValueSystem allows for flexible data storage and retrieval using gameplay tags as identifiers. By leveraging UScriptStruct, we can create a type-safe yet generic system for storing and passing various data types within the same container.

## Implementation Strategy

### 1. Core Components

#### Base Value Struct

```cpp
USTRUCT(BlueprintType)
struct FBaseTagValue
{
    GENERATED_BODY()

    FBaseTagValue() {}
    virtual ~FBaseTagValue() {}

    // Virtual function to identify the type
    virtual FName GetValueType() const { return NAME_None; }
};
```

#### Type-Specific Value Structs

```cpp
USTRUCT(BlueprintType)
struct FFloatTagValue : public FBaseTagValue
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Value;

    FFloatTagValue() : Value(0.0f) {}
    FFloatTagValue(float InValue) : Value(InValue) {}

    virtual FName GetValueType() const override { return FName("Float"); }
};

// Similar structs for other data types (int, bool, string, etc.)
```

#### TagValue Container

```cpp
USTRUCT(BlueprintType)
struct FTagValueContainer
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FGameplayTag, TSharedPtr<FBaseTagValue>> Values;

    // Helper functions for setting values
    template<typename T>
    void SetValue(FGameplayTag Tag, const T& Value)
    {
        Values.Add(Tag, MakeShared<T>(Value));
    }

    // Helper functions for getting values
    template<typename T>
    bool GetValue(FGameplayTag Tag, T& OutValue) const
    {
        if (TSharedPtr<FBaseTagValue>* BaseValue = Values.Find(Tag))
        {
            if (TSharedPtr<T> TypedValue = StaticCastSharedPtr<T>(*BaseValue))
            {
                OutValue = *TypedValue;
                return true;
            }
        }
        return false;
    }
};
```

### 2. Serialization Support

To enable proper serialization and deserialization of the TagValue structs:

```cpp
// Add to your struct declarations
USTRUCT(BlueprintType)
struct FBaseTagValue
{
    GENERATED_BODY()

    // ... other code ...

    // Serialization support
    bool Serialize(FArchive& Ar);
    bool operator==(const FBaseTagValue& Other) const;
    friend FArchive& operator<<(FArchive& Ar, FBaseTagValue& Value)
    {
        Value.Serialize(Ar);
        return Ar;
    }
};
```

### 3. Blueprint Support

Create blueprint function libraries to expose the functionality:

```cpp
UCLASS()
class UTagValueBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    UFUNCTION(BlueprintCallable, Category = "TagValue")
    static void SetFloatValue(UPARAM(ref) FTagValueContainer& Container, FGameplayTag Tag, float Value);

    UFUNCTION(BlueprintPure, Category = "TagValue")
    static bool GetFloatValue(const FTagValueContainer& Container, FGameplayTag Tag, float& OutValue);

    // Similar functions for other data types
};
```

## Usage Examples

### C++ Usage

```cpp
// Create a container
FTagValueContainer Container;

// Set values of different types
Container.SetValue<FFloatTagValue>(FGameplayTag::RequestGameplayTag("Stat.Health"), FFloatTagValue(100.0f));
Container.SetValue<FStringTagValue>(FGameplayTag::RequestGameplayTag("Player.Name"), FStringTagValue("Hero"));

// Retrieve values
FFloatTagValue HealthValue;
if (Container.GetValue<FFloatTagValue>(FGameplayTag::RequestGameplayTag("Stat.Health"), HealthValue))
{
    UE_LOG(LogTemp, Display, TEXT("Health: %f"), HealthValue.Value);
}
```

### Blueprint Usage

The Blueprint Function Library provides functions for setting and getting values:

- `SetFloatValue(Container, Tag, Value)`
- `GetFloatValue(Container, Tag, OutValue)`

## Benefits of This Approach

1. **Type Safety**: Strong typing through UScriptStruct inheritance
2. **Blueprint Support**: Full exposure to Blueprint system
3. **Serialization**: Proper support for saving/loading
4. **Flexibility**: Can store any data type in the same container
5. **Performance**: More efficient than using UObject for small data
6. **Extensibility**: Easy to add new data types as needed

## Considerations

1. **Memory Management**: Using TSharedPtr for automatic memory management
2. **Type Checking**: Runtime type checking is necessary when retrieving values
3. **Reflection**: Limited reflection support compared to UObject

## Advanced Implementation Ideas

1. **Property Replication**: Add networking support with custom NetSerialize
2. **Data Binding**: Create a system for binding UI elements to tag values
3. **Data Validation**: Add validation rules per tag or value type
4. **Default Values**: System for providing default values when tags are missing