classDiagram
    class DialogueNode {
        <<abstract>>
        +SpeakerTag: GameplayTag
        +CameraBlend: GameplayCameraBlendData
    }
    class DialogueNode_Text {
        +DialogueText: String
        +OnDialogueEndedEvent
    }
    class DialogueNode_Choice {
        +Choices: List<Choice>
    }
    class Choice {
        +DisplayText: String
        +ResultingTag: GameplayTag
        +NextState: DialogueNode
    }
    class DialogueNode_Condition {
        +TagToCheck: GameplayTag
        +ConditionType: Enum
        +TrueBranch: DialogueNode
        +FalseBranch: DialogueNode
    }
    class DialogueNode_Action {
        +TagToModify: GameplayTag
        +ModificationType: Enum
        +NextState: DialogueNode
    }
    class SmartObject_Dialogue {
        +DialogueStateTree: StateTree_Dialogue
        +InitialDialogueTags: GameplayTagContainer
    }
    class StateTree_Dialogue {
        +Nodes: List<DialogueNode>
        +StartState: DialogueNode
    }
    class GameplayCameraBlendData {
        +BlendSettings: ...
    }
    class DialogueUIWidget {
        +DisplayDialogue(Speaker: GameplayTag, Text: String)
        +ShowChoices(Choices: List<Choice>)
        +OnChoiceSelected(ChoiceIndex: int)
    }

    DialogueNode <|-- DialogueNode_Text
    DialogueNode <|-- DialogueNode_Choice
    DialogueNode <|-- DialogueNode_Condition
    DialogueNode <|-- DialogueNode_Action
    DialogueNode_Choice --* Choice
    Choice --> DialogueNode : NextState
    DialogueNode_Condition --> DialogueNode : TrueBranch
    DialogueNode_Condition --> DialogueNode : FalseBranch
    DialogueNode_Action --> DialogueNode : NextState
    SmartObject_Dialogue --o StateTree_Dialogue
    DialogueNode --> GameplayCameraBlendData : CameraBlend
    DialogueNode_Text --|> DialogueUIWidget : Uses
    DialogueNode_Choice --|> DialogueUIWidget : Uses

architectureMap
    participant "Player\nCharacter/\nController" as Player
    participant "Smart Object\n(Dialogue\nCharacter)" as SmartObject
    participant "State Tree\nSystem" as StateTree
    participant "Gameplay\nCamera\nSystem" as CameraSystem
    participant "Gameplay Tag\nSystem" as TagSystem
    participant "Dialogue UI" as UI

    Player -> SmartObject : Interact
    SmartObject -> StateTree : Get Dialogue State Tree
    Player -> StateTree : Initialize State Tree
    StateTree -> StateTree : Execute State
    StateTree -> TagSystem : Check/Modify Tags
    StateTree -> CameraSystem : Set Camera Blend
    StateTree -> UI : Display Dialogue/Choices
    UI -> Player : Player Input (Choice)
    UI -> StateTree : Report Choice
    StateTree -> StateTree : Transition State