#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "TagValueInterface.h"
#include "TagValueBase.h"
#include "TagValueContainer.h"
#include "TagValueRepositoryComponent.generated.h"

/**
 * Actor Component that implements TagValueRepository interface.
 * Provides a component-based repository for storing and retrieving tag values.
 * Can automatically register with the GameplayTagValueSubsystem on BeginPlay.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class GAMPLAYTAGVALUE_API UTagValueRepositoryComponent : public UActorComponent, public ITagValueRepository
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTagValueRepositoryComponent();

	// Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End UActorComponent interface

	// Begin ITagValueRepository interface
	virtual bool HasValue(FGameplayTag Tag) const override;
	virtual TSharedPtr<ITagValueHolder> GetValue(FGameplayTag Tag) const override;
	virtual void SetValue(FGameplayTag Tag, TSharedPtr<ITagValueHolder> Value) override;
	virtual void RemoveValue(FGameplayTag Tag) override;
	virtual void ClearAllValues() override;
	virtual TArray<FGameplayTag> GetAllTags() const override;
	virtual FName GetRepositoryName() const override;
	virtual int32 GetPriority() const override;
	// End ITagValueRepository interface

	/**
	 * Get the tag value container
	 * @return The tag value container
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	const FTagValueContainer& GetTagValueContainer() const { return TagValueContainer; }

	/**
	 * Set a bool tag value
	 * @param InTag The tag to set
	 * @param InValue The value to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void SetBoolTagValue(FGameplayTag InTag, bool InValue);

	/**
	 * Set an int tag value
	 * @param InTag The tag to set
	 * @param InValue The value to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void SetIntTagValue(FGameplayTag InTag, int32 InValue);

	/**
	 * Set a float tag value
	 * @param InTag The tag to set
	 * @param InValue The value to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void SetFloatTagValue(FGameplayTag InTag, float InValue);

	/**
	 * Set a string tag value
	 * @param InTag The tag to set
	 * @param InValue The value to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void SetStringTagValue(FGameplayTag InTag, const FString& InValue);

	/**
	 * Set a transform tag value
	 * @param InTag The tag to set
	 * @param InValue The value to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void SetTransformTagValue(FGameplayTag InTag, const FTransform& InValue);

	/**
	 * Set a class tag value
	 * @param InTag The tag to set
	 * @param InValue The value to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void SetClassTagValue(FGameplayTag InTag, TSoftClassPtr<UObject> InValue);

	/**
	 * Set an object tag value
	 * @param InTag The tag to set
	 * @param InValue The value to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void SetObjectTagValue(FGameplayTag InTag, TSoftObjectPtr<UObject> InValue);

	/**
	 * Remove a tag value
	 * @param InTag The tag to remove
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void RemoveTagValue(FGameplayTag InTag);

	/**
	 * Clear all tag values
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void ClearTagValues();

	/**
	 * Register this repository with the subsystem
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void RegisterWithSubsystem();

	/**
	 * Unregister this repository from the subsystem
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void UnregisterFromSubsystem();

protected:
	/** The tag value container storing all values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Tags|Values")
	FTagValueContainer TagValueContainer;

	/** Whether to automatically register with the subsystem on BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Tags|Values")
	bool bRegisterToSubsystem;

	/** The name of this repository */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Tags|Values")
	FName RepositoryName;

	/** The priority of this repository (higher priority repositories are checked first) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Tags|Values")
	int32 Priority;

private:
	/** Helper function to convert between FBaseTagValue and ITagValueHolder */
	TSharedPtr<ITagValueHolder> ConvertTagValueToHolder(TSharedPtr<FBaseTagValue> Value) const;

	/** Helper function to convert between ITagValueHolder and appropriate tag value type */
	TSharedPtr<FBaseTagValue> ConvertHolderToTagValue(const TSharedPtr<ITagValueHolder>& Holder) const;

	/** Flag to track if we're registered with the subsystem */
	bool bIsRegistered;
};
