// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_CallFunction.h"
#include "EdGraphSchema_K2.h"
#include "GameplayTags.h"
#include "K2Node_GameplayTagValueAction.generated.h"

/**
 * Base class for K2 nodes that handle gameplay tag values in a generic way.
 * These nodes provide a convenient way to get and set gameplay tag values with automatic type resolution.
 * Supported types include: Bool, Int, Float, String, Name, Vector, Rotator, Transform, Class, and Object.
 */
UCLASS(Abstract)
class GAMPLAYTAGVALUE_API UK2Node_GameplayTagValueAction : public UK2Node
{
    GENERATED_BODY()

public:
    // UEdGraphNode interface
    virtual void AllocateDefaultPins() override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
    virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
    virtual FText GetMenuCategory() const override;
    virtual void PostReconstructNode() override;
    virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
    // End of UEdGraphNode interface

    // UK2Node interface
    virtual bool IsNodePure() const override { return false; }
    virtual bool NodeCausesStructuralBlueprintChange() const override { return false; }
    // End of UK2Node interface

protected:
    /** Determines if this is a Get or Set node */
    virtual bool IsGetNode() const PURE_VIRTUAL(UK2Node_GameplayTagValueAction::IsGetNode, return false;);
    
    /** Gets the function name to call based on the connected value pin type */
    virtual FName GetFunctionNameForValueType(const UEdGraphPin* ValuePin) const PURE_VIRTUAL(UK2Node_GameplayTagValueAction::GetFunctionNameForValueType, return NAME_None;);
    
    /** Gets the function name to call based on the value type */
    virtual FName GetFunctionNameForValueType(FName ValueType) const PURE_VIRTUAL(UK2Node_GameplayTagValueAction::GetFunctionNameForValueType, return NAME_None;);
    
    /** Gets the default value pin name */
    virtual FName GetDefaultValuePinName() const PURE_VIRTUAL(UK2Node_GameplayTagValueAction::GetDefaultValuePinName, return NAME_None;);
    
    /** Gets the value pin name */
    virtual FName GetValuePinName() const;
    
    /** Gets the tag pin name */
    virtual FName GetTagPinName() const { return TEXT("Tag"); }
    
    /** Gets the world context pin name */
    virtual FName GetWorldContextPinName() const { return TEXT("WorldContextObject"); }
    
    /** Gets the context object pin name */
    virtual FName GetContextPinName() const { return TEXT("Context"); }
    
    /** Gets the repository name pin name */
    virtual FName GetRepositoryPinName() const { return TEXT("RepositoryName"); }
    
    /** Gets the success pin name */
    virtual FName GetSuccessPinName() const { return TEXT("bSuccess"); }
    
    /** 
     * Gets the value type from a connected pin 
     * @param Pin The pin to get the value type from
     * @return The value type as a name (e.g., "Bool", "Int", "Float", etc.)
     */
    FName GetValueTypeFromPin(const UEdGraphPin* Pin) const;
    
    /** 
     * Gets the value type from a pin type 
     * @param PinType The pin type to get the value type from
     * @return The value type as a name (e.g., "Bool", "Int", "Float", etc.)
     */
    FName GetValueTypeFromPinType(const FEdGraphPinType& PinType) const;
    
    /** 
     * Creates a function call node for the specified function 
     * @param CompilerContext The compiler context
     * @param SourceGraph The source graph
     * @param FunctionName The name of the function to call
     * @return The created function call node
     */
    UK2Node_CallFunction* CreateFunctionCallNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, FName FunctionName) const;
    
    /** 
     * Connects the common pins between this node and the function call node 
     * @param CompilerContext The compiler context
     * @param SourceGraph The source graph
     * @param FunctionNode The function call node to connect pins to
     */
    void ConnectCommonPins(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, UK2Node_CallFunction* FunctionNode) const;
};

/**
 * K2 node for getting a gameplay tag value in a generic way.
 * This node automatically resolves the type based on the connected output pin.
 * Supported types include: Bool, Int, Float, String, Name, Vector, Rotator, Transform, Class, and Object.
 */
UCLASS()
class GAMPLAYTAGVALUE_API UK2Node_GetGameplayTagValue : public UK2Node_GameplayTagValueAction
{
    GENERATED_BODY()

public:
    // UEdGraphNode interface
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    // End of UEdGraphNode interface
    
    // UK2Node interface
    virtual bool IsNodePure() const override { return true; }
    // End of UK2Node interface
    
protected:
    /** Determines if this is a Get or Set node */
    virtual bool IsGetNode() const override { return true; }
    
    /** Gets the function name to call based on the connected value pin type */
    virtual FName GetFunctionNameForValueType(const UEdGraphPin* ValuePin) const override;
    
    /** Gets the function name to call based on the value type */
    virtual FName GetFunctionNameForValueType(FName ValueType) const override;
    
    /** Gets the default value pin name */
    virtual FName GetDefaultValuePinName() const override { return TEXT("DefaultValue"); }
};

/**
 * K2 node for setting a gameplay tag value in a generic way.
 * This node automatically resolves the type based on the connected input pin.
 * Supported types include: Bool, Int, Float, String, Name, Vector, Rotator, Transform, Class, and Object.
 */
UCLASS()
class GAMPLAYTAGVALUE_API UK2Node_SetGameplayTagValue : public UK2Node_GameplayTagValueAction
{
    GENERATED_BODY()

public:
    // UEdGraphNode interface
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    // End of UEdGraphNode interface
    
protected:
    /** Determines if this is a Get or Set node */
    virtual bool IsGetNode() const override { return false; }
    
    /** Gets the function name to call based on the connected value pin type */
    virtual FName GetFunctionNameForValueType(const UEdGraphPin* ValuePin) const override;
    
    /** Gets the function name to call based on the value type */
    virtual FName GetFunctionNameForValueType(FName ValueType) const override;
    
    /** Gets the default value pin name */
    virtual FName GetDefaultValuePinName() const override { return NAME_None; }
};
