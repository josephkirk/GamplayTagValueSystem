#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "TagValueBase.h"
#include "Templates/SharedPointer.h"
#include "TagValueContainer.generated.h"

/**
 * Container for storing and retrieving tag values of different types
 * Provides type-safe access to various data types within the same container
 */
USTRUCT(BlueprintType)
struct GAMPLAYTAGVALUE_API FTagValueContainer
{
	GENERATED_BODY()

	/** Array of tag-value pairs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag Value")
	TArray<FBaseTagValue> Values;

	/** Default constructor */
	FTagValueContainer() {}

	/**
	 * Set a value of a specific type for a gameplay tag
	 * @param Tag - The gameplay tag to set the value for
	 * @param Value - The value to set
	 */
	template<typename T>
	void SetValue(FGameplayTag Tag, const T& Value)
	{
		// First remove any existing value with this tag
		RemoveValue(Tag);
		
		// Create a new value of the appropriate type
		T NewValue(Value);
		NewValue.Tag = Tag;
		Values.Add(NewValue);
	}

	/**
	 * Get a value of a specific type for a gameplay tag
	 * @param Tag - The gameplay tag to get the value for
	 * @param OutValue - The output value if found
	 * @return True if the value was found and is of the correct type
	 */
	template<typename T>
	bool GetValue(FGameplayTag Tag, T& OutValue) const
	{
		for (const FBaseTagValue& Value : Values)
		{
			if (Value.Tag == Tag)
			{
				if (const T* TypedValue = Value.TryCast<T>())
				{
					OutValue = *TypedValue;
					return true;
				}
			}
		}
		return false;
	}

	/**
	 * Check if a value exists for a gameplay tag
	 * @param Tag - The gameplay tag to check
	 * @return True if a value exists for the tag
	 */
	bool HasValue(FGameplayTag Tag) const
	{
		for (const FBaseTagValue& Value : Values)
		{
			if (Value.Tag == Tag)
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * Remove a value for a gameplay tag
	 * @param Tag - The gameplay tag to remove the value for
	 * @return True if a value was removed
	 */
	bool RemoveValue(FGameplayTag Tag)
	{
		for (int32 i = 0; i < Values.Num(); i++)
		{
			if (Values[i].Tag == Tag)
			{
				Values.RemoveAt(i);
				return true;
			}
		}
		return false;
	}

	/**
	 * Get all tags in the container
	 * @return Array of all gameplay tags in the container
	 */
	TArray<FGameplayTag> GetAllTags() const
	{
		TArray<FGameplayTag> Tags;
		for (const FBaseTagValue& Value : Values)
		{
			Tags.Add(Value.Tag);
		}
		return Tags;
	}

	/**
	 * Clear all values from the container
	 */
	void Clear()
	{
		Values.Empty();
	}

	/**
	 * Get the number of values in the container
	 * @return Number of values in the container
	 */
	int32 Num() const
	{
		return Values.Num();
	}
};
