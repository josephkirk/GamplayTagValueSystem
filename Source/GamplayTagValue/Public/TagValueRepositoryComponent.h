#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "TagValueInterface.h"
#include "TagValue.h"
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
	 * Get the tag value mappings
	 * @return Array of tag value mappings
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	const TArray<FTagValueMapping>& GetTagValueMappings() const { return TagValueMappings; }

	/**
	 * Set a tag value mapping
	 * @param InTag The tag to set
	 * @param InValue The value to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void SetTagValueMapping(FGameplayTag InTag, FTagValue InValue);

	/**
	 * Remove a tag value mapping
	 * @param InTag The tag to remove
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void RemoveTagValueMapping(FGameplayTag InTag);

	/**
	 * Clear all tag value mappings
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags|Values")
	void ClearTagValueMappings();

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
	/** The tag value mappings stored in this repository */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Tags|Values")
	TArray<FTagValueMapping> TagValueMappings;

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
	/** Helper function to convert between FTagValue and ITagValueHolder */
	TSharedPtr<ITagValueHolder> ConvertTagValueToHolder(const FTagValue& Value) const;

	/** Helper function to convert between ITagValueHolder and FTagValue */
	bool ConvertHolderToTagValue(const TSharedPtr<ITagValueHolder>& Holder, FTagValue& OutValue) const;

	/** Flag to track if we're registered with the subsystem */
	bool bIsRegistered;
};
