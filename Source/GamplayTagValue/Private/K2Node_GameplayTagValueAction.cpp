#include "K2Node_GameplayTagValueAction.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"
#include "GameplayTagValueFunctionLibrary.h"
#include "GameplayTagValueSubsystem.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "K2Node_GameplayTagValueAction"

//////////////////////////////////////////////////////////////////////////
// UK2Node_GameplayTagValueAction

void UK2Node_GameplayTagValueAction::AllocateDefaultPins()
{
    // Add execution pins if not pure
    if (!IsNodePure())
    {
        CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
        CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
    }
    
    // Add the world context pin
    UEdGraphPin* WorldContextPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), GetWorldContextPinName());
    WorldContextPin->PinToolTip = TEXT("The world context object");
    
    // Add the tag pin
    UEdGraphPin* TagPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FGameplayTag::StaticStruct(), GetTagPinName());
    TagPin->PinToolTip = TEXT("The gameplay tag to get/set the value for");
    
    if (IsGetNode())
    {
        // For Get nodes, add the default value pin (type will be set based on connections)
        UEdGraphPin* DefaultValuePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, GetDefaultValuePinName());
        DefaultValuePin->PinToolTip = TEXT("The default value to return if the tag is not found");
        
        // Add the context pin
        UEdGraphPin* ContextPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), GetContextPinName());
        ContextPin->PinToolTip = TEXT("Optional context object that implements UTagValueInterface");
        ContextPin->bAdvancedView = true;
        
        // Add the success pin
        UEdGraphPin* SuccessPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, GetSuccessPinName());
        SuccessPin->PinToolTip = TEXT("True if the tag has a valid value");
        
        // Add the value output pin (type will be set based on connections)
        UEdGraphPin* ValuePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, GetValuePinName());
        ValuePin->PinToolTip = TEXT("The value associated with the tag");
    }
    else
    {
        // For Set nodes, add the value input pin (type will be set based on connections)
        UEdGraphPin* ValuePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, GetValuePinName());
        ValuePin->PinToolTip = TEXT("The value to set");
        
        // Add the repository name pin
        UEdGraphPin* RepositoryPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, GetRepositoryPinName());
        RepositoryPin->PinToolTip = TEXT("Optional repository name to target (uses default if not specified)");
        RepositoryPin->bAdvancedView = true;
        RepositoryPin->DefaultValue = TEXT("None");
        
        // Add the success pin
        UEdGraphPin* SuccessPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, GetSuccessPinName());
        SuccessPin->PinToolTip = TEXT("True if the value was set successfully");
    }
}

FText UK2Node_GameplayTagValueAction::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return IsGetNode() ? LOCTEXT("GetGameplayTagValue", "Get Gameplay Tag Value") : LOCTEXT("SetGameplayTagValue", "Set Gameplay Tag Value");
}

FText UK2Node_GameplayTagValueAction::GetTooltipText() const
{
    return IsGetNode() ? 
        LOCTEXT("GetGameplayTagValueTooltip", "Gets a value for the specified gameplay tag with automatic type resolution") : 
        LOCTEXT("SetGameplayTagValueTooltip", "Sets a value for the specified gameplay tag with automatic type resolution");
}

void UK2Node_GameplayTagValueAction::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);
    
    // Find the value pin to determine the function to call
    UEdGraphPin* ValuePin = FindPin(GetValuePinName());
    if (!ValuePin)
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("MissingValuePin", "@@: Value pin is missing").ToString(), this);
        return;
    }
    
    // Check if the value pin is connected
    if (ValuePin->LinkedTo.Num() == 0)
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("NoValueConnection", "@@: Value pin must be connected to determine the type").ToString(), this);
        return;
    }
    
    // Get the function name based on the connected value type
    FName FunctionName = GetFunctionNameForValueType(ValuePin);
    if (FunctionName == NAME_None)
    {
        CompilerContext.MessageLog.Error(*FText::Format(LOCTEXT("UnsupportedValueType", "@@: Unsupported value type: {0}"), 
            FText::FromString(ValuePin->PinType.PinCategory.ToString())).ToString(), this);
        return;
    }
    
    // Create the function call node
    UK2Node_CallFunction* FunctionNode = CreateFunctionCallNode(CompilerContext, SourceGraph, FunctionName);
    if (!FunctionNode)
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("FailedToCreateFunctionNode", "@@: Failed to create function call node").ToString(), this);
        return;
    }
    
    // Connect the common pins
    ConnectCommonPins(CompilerContext, SourceGraph, FunctionNode);
    
    // Move the connections from this node to the function call node
    UEdGraphPin* FunctionValuePin = FunctionNode->FindPin(GetValuePinName());
    if (ValuePin && FunctionValuePin)
    {
        MovePinLinksToIntermediate(CompilerContext, ValuePin, FunctionValuePin);
    }
    
    // Remove this node
    BreakAllNodeLinks();
}

void UK2Node_GameplayTagValueAction::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();
    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner != nullptr);
        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

FText UK2Node_GameplayTagValueAction::GetMenuCategory() const
{
    return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Gameplay);
}

void UK2Node_GameplayTagValueAction::PostReconstructNode()
{
    Super::PostReconstructNode();
    
    // Update the pin types based on connections
    UEdGraphPin* ValuePin = FindPin(GetValuePinName());
    if (ValuePin && ValuePin->LinkedTo.Num() > 0)
    {
        UEdGraphPin* LinkedPin = ValuePin->LinkedTo[0];
        if (LinkedPin)
        {
            // Update the value pin type to match the linked pin
            ValuePin->PinType = LinkedPin->PinType;
            
            // If this is a Get node, also update the default value pin type
            if (IsGetNode())
            {
                UEdGraphPin* DefaultValuePin = FindPin(GetDefaultValuePinName());
                if (DefaultValuePin)
                {
                    DefaultValuePin->PinType = LinkedPin->PinType;
                }
            }
        }
    }
}

void UK2Node_GameplayTagValueAction::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
    Super::NotifyPinConnectionListChanged(Pin);
    
    // If the value pin connection changed, update the pin types
    if (Pin && Pin->PinName == GetValuePinName())
    {
        if (Pin->LinkedTo.Num() > 0)
        {
            UEdGraphPin* LinkedPin = Pin->LinkedTo[0];
            if (LinkedPin)
            {
                // Update the value pin type to match the linked pin
                Pin->PinType = LinkedPin->PinType;
                
                // If this is a Get node, also update the default value pin type
                if (IsGetNode())
                {
                    UEdGraphPin* DefaultValuePin = FindPin(GetDefaultValuePinName());
                    if (DefaultValuePin)
                    {
                        DefaultValuePin->PinType = LinkedPin->PinType;
                    }
                }
                
                // Notify the graph that the node has changed
                GetGraph()->NotifyGraphChanged();
            }
        }
        else
        {
            // Reset to wildcard if disconnected
            Pin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
            Pin->PinType.PinSubCategory = NAME_None;
            Pin->PinType.PinSubCategoryObject = nullptr;
            
            // If this is a Get node, also reset the default value pin type
            if (IsGetNode())
            {
                UEdGraphPin* DefaultValuePin = FindPin(GetDefaultValuePinName());
                if (DefaultValuePin)
                {
                    DefaultValuePin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
                    DefaultValuePin->PinType.PinSubCategory = NAME_None;
                    DefaultValuePin->PinType.PinSubCategoryObject = nullptr;
                }
            }
            
            // Notify the graph that the node has changed
            GetGraph()->NotifyGraphChanged();
        }
    }
}

FName UK2Node_GameplayTagValueAction::GetValuePinName() const
{
    return TEXT("Value");
}

FName UK2Node_GameplayTagValueAction::GetValueTypeFromPin(const UEdGraphPin* Pin) const
{
    if (Pin && Pin->LinkedTo.Num() > 0)
    {
        UEdGraphPin* LinkedPin = Pin->LinkedTo[0];
        if (LinkedPin)
        {
            return GetValueTypeFromPinType(LinkedPin->PinType);
        }
    }
    
    return NAME_None;
}

FName UK2Node_GameplayTagValueAction::GetValueTypeFromPinType(const FEdGraphPinType& PinType) const
{
    // Determine the value type based on the pin type
    if (PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean)
    {
        return TEXT("Bool");
    }
    else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Int)
    {
        return TEXT("Int");
    }
    else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Float)
    {
        return TEXT("Float");
    }
    else if (PinType.PinCategory == UEdGraphSchema_K2::PC_String)
    {
        return TEXT("String");
    }
    else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Name)
    {
        return TEXT("Name");
    }
    else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && PinType.PinSubCategoryObject == FVector::StaticStruct())
    {
        return TEXT("Vector");
    }
    else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && PinType.PinSubCategoryObject == FRotator::StaticStruct())
    {
        return TEXT("Rotator");
    }
    else if (PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && PinType.PinSubCategoryObject == FTransform::StaticStruct())
    {
        return TEXT("Transform");
    }
    else if (PinType.PinCategory == UEdGraphSchema_K2::PC_SoftClass)
    {
        return TEXT("Class");
    }
    else if (PinType.PinCategory == UEdGraphSchema_K2::PC_SoftObject)
    {
        return TEXT("Object");
    }
    
    return NAME_None;
}

UK2Node_CallFunction* UK2Node_GameplayTagValueAction::CreateFunctionCallNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, FName FunctionName) const
{
    UK2Node_CallFunction* FunctionNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    FunctionNode->FunctionReference.SetExternalMember(FunctionName, UGameplayTagValueFunctionLibrary::StaticClass());
    FunctionNode->AllocateDefaultPins();
    return FunctionNode;
}

void UK2Node_GameplayTagValueAction::ConnectCommonPins(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, UK2Node_CallFunction* FunctionNode) const
{
    // Connect execution pins if not pure
    if (!IsNodePure())
    {
        UEdGraphPin* ExecPin = FindPin(UEdGraphSchema_K2::PN_Execute);
        UEdGraphPin* ThenPin = FindPin(UEdGraphSchema_K2::PN_Then);
        UEdGraphPin* FunctionExecPin = FunctionNode->FindPin(UEdGraphSchema_K2::PN_Execute);
        UEdGraphPin* FunctionThenPin = FunctionNode->FindPin(UEdGraphSchema_K2::PN_Then);
        
        if (ExecPin && FunctionExecPin)
        {
            MovePinLinksToIntermediate(CompilerContext, ExecPin, FunctionExecPin);
        }
        
        if (ThenPin && FunctionThenPin)
        {
            MovePinLinksToIntermediate(CompilerContext, ThenPin, FunctionThenPin);
        }
    }
    
    // Connect the world context pin
    UEdGraphPin* WorldContextPin = FindPin(GetWorldContextPinName());
    UEdGraphPin* FunctionWorldContextPin = FunctionNode->FindPin(GetWorldContextPinName());
    if (WorldContextPin && FunctionWorldContextPin)
    {
        MovePinLinksToIntermediate(CompilerContext, WorldContextPin, FunctionWorldContextPin);
    }
    
    // Connect the tag pin
    UEdGraphPin* TagPin = FindPin(GetTagPinName());
    UEdGraphPin* FunctionTagPin = FunctionNode->FindPin(GetTagPinName());
    if (TagPin && FunctionTagPin)
    {
        MovePinLinksToIntermediate(CompilerContext, TagPin, FunctionTagPin);
    }
    
    if (IsGetNode())
    {
        // Connect the default value pin
        UEdGraphPin* DefaultValuePin = FindPin(GetDefaultValuePinName());
        UEdGraphPin* FunctionDefaultValuePin = FunctionNode->FindPin(GetDefaultValuePinName());
        if (DefaultValuePin && FunctionDefaultValuePin)
        {
            MovePinLinksToIntermediate(CompilerContext, DefaultValuePin, FunctionDefaultValuePin);
        }
        
        // Connect the context pin
        UEdGraphPin* ContextPin = FindPin(GetContextPinName());
        UEdGraphPin* FunctionContextPin = FunctionNode->FindPin(GetContextPinName());
        if (ContextPin && FunctionContextPin)
        {
            MovePinLinksToIntermediate(CompilerContext, ContextPin, FunctionContextPin);
        }
        
        // Connect the success pin
        UEdGraphPin* SuccessPin = FindPin(GetSuccessPinName());
        UEdGraphPin* FunctionSuccessPin = FunctionNode->FindPin(TEXT("bSuccess"));
        if (SuccessPin && FunctionSuccessPin)
        {
            MovePinLinksToIntermediate(CompilerContext, SuccessPin, FunctionSuccessPin);
        }
    }
    else
    {
        // Connect the repository name pin
        UEdGraphPin* RepositoryPin = FindPin(GetRepositoryPinName());
        UEdGraphPin* FunctionRepositoryPin = FunctionNode->FindPin(GetRepositoryPinName());
        if (RepositoryPin && FunctionRepositoryPin)
        {
            MovePinLinksToIntermediate(CompilerContext, RepositoryPin, FunctionRepositoryPin);
        }
        
        // Connect the success pin (return value from the function)
        UEdGraphPin* SuccessPin = FindPin(GetSuccessPinName());
        UEdGraphPin* FunctionReturnPin = FunctionNode->GetReturnValuePin();
        if (SuccessPin && FunctionReturnPin)
        {
            MovePinLinksToIntermediate(CompilerContext, SuccessPin, FunctionReturnPin);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// UK2Node_GetGameplayTagValue

FText UK2Node_GetGameplayTagValue::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("GetGameplayTagValue", "Get Gameplay Tag Value");
}

FText UK2Node_GetGameplayTagValue::GetTooltipText() const
{
    return LOCTEXT("GetGameplayTagValueTooltip", "Gets a value for the specified gameplay tag with automatic type resolution");
}

FName UK2Node_GetGameplayTagValue::GetFunctionNameForValueType(const UEdGraphPin* ValuePin) const
{
    if (ValuePin)
    {
        return GetFunctionNameForValueType(GetValueTypeFromPin(ValuePin));
    }
    
    return NAME_None;
}

FName UK2Node_GetGameplayTagValue::GetFunctionNameForValueType(FName ValueType) const
{
    // Map the value type to the corresponding function name
    static const TMap<FName, FName> FunctionMap = {
        { TEXT("Bool"), TEXT("GetBoolTagValue") },
        { TEXT("Int"), TEXT("GetIntTagValue") },
        { TEXT("Float"), TEXT("GetFloatTagValue") },
        { TEXT("String"), TEXT("GetStringTagValue") },
        { TEXT("Name"), TEXT("GetNameTagValue") },
        { TEXT("Vector"), TEXT("GetVectorTagValue") },
        { TEXT("Rotator"), TEXT("GetRotatorTagValue") },
        { TEXT("Transform"), TEXT("GetTransformTagValue") },
        { TEXT("Class"), TEXT("GetClassTagValue") },
        { TEXT("Object"), TEXT("GetObjectTagValue") }
    };
    
    const FName* FunctionName = FunctionMap.Find(ValueType);
    return FunctionName ? *FunctionName : NAME_None;
}

//////////////////////////////////////////////////////////////////////////
// UK2Node_SetGameplayTagValue

FText UK2Node_SetGameplayTagValue::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("SetGameplayTagValue", "Set Gameplay Tag Value");
}

FText UK2Node_SetGameplayTagValue::GetTooltipText() const
{
    return LOCTEXT("SetGameplayTagValueTooltip", "Sets a value for the specified gameplay tag with automatic type resolution");
}

FName UK2Node_SetGameplayTagValue::GetFunctionNameForValueType(const UEdGraphPin* ValuePin) const
{
    if (ValuePin)
    {
        return GetFunctionNameForValueType(GetValueTypeFromPin(ValuePin));
    }
    
    return NAME_None;
}

FName UK2Node_SetGameplayTagValue::GetFunctionNameForValueType(FName ValueType) const
{
    // Map the value type to the corresponding function name
    static const TMap<FName, FName> FunctionMap = {
        { TEXT("Bool"), TEXT("SetBoolTagValue") },
        { TEXT("Int"), TEXT("SetIntTagValue") },
        { TEXT("Float"), TEXT("SetFloatTagValue") },
        { TEXT("String"), TEXT("SetStringTagValue") },
        { TEXT("Name"), TEXT("SetNameTagValue") },
        { TEXT("Vector"), TEXT("SetVectorTagValue") },
        { TEXT("Rotator"), TEXT("SetRotatorTagValue") },
        { TEXT("Transform"), TEXT("SetTransformTagValue") },
        { TEXT("Class"), TEXT("SetClassTagValue") },
        { TEXT("Object"), TEXT("SetObjectTagValue") }
    };
    
    const FName* FunctionName = FunctionMap.Find(ValueType);
    return FunctionName ? *FunctionName : NAME_None;
}

#undef LOCTEXT_NAMESPACE
