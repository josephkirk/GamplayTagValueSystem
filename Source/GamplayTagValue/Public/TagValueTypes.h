#pragma once

#include "CoreMinimal.h"
#include "TagValueTypes.generated.h"

/**
 * Enum defining the types of values that can be stored in the tag value system
 */
UENUM(BlueprintType)
enum class ETagValueType : uint8
{
    Bool        UMETA(DisplayName = "Boolean"),
    Int         UMETA(DisplayName = "Integer"),
    Float       UMETA(DisplayName = "Float"),
    String      UMETA(DisplayName = "String"),
    Transform   UMETA(DisplayName = "Transform"),
    Class       UMETA(DisplayName = "Class Reference"),
    Object      UMETA(DisplayName = "Object Reference")
};
