## Workflow Documentation: Using the Dynamic Dialogue System

This document outlines the steps for game developers and designers to create and implement dialogue within the game using the integrated State Tree, Smart Object, Gameplay Camera, and Gameplay Tag systems.

**1. Define Gameplay Tags:**

* Open the Project Settings in Unreal Engine.
* Navigate to the "Gameplay Tags" section.
* Define the necessary Gameplay Tags based on your dialogue needs. Consider the following categories:
    * **Character Tags:** Identify individual characters (e.g., `Dialogue.Character.Guard`, `Dialogue.Character.Merchant`).
    * **Topic Tags:** Represent subjects of conversation (e.g., `Dialogue.Topic.Quest.Main`, `Dialogue.Topic.Rumors.Town`).
    * **Choice Tags:** Indicate player decisions (e.g., `Dialogue.Choice.Greeting.Friendly`, `Dialogue.Choice.Quest.Accepted`).
    * **State Tags:** Track the current state of the dialogue (e.g., `Dialogue.State.Greeting`, `Dialogue.State.OfferingQuest`).
    * **Camera Tags:** Define specific camera behaviors (e.g., `Dialogue.Camera.CloseUp`, `Dialogue.Camera.Medium`).

**2. Create Gameplay Camera Blend Data Assets:**

* In the Content Browser, create new assets of the type "Gameplay Camera Blend Data".
* Name them descriptively (e.g., `Camera_CloseUp_Guard`, `Camera_Medium_Both`).
* Configure the blend settings within each asset to define the desired camera behavior (e.g., target offset, rotation, blend time).

**3. Design State Tree Assets:**

* In the Content Browser, create new assets of the type "State Tree".
* Name each State Tree to correspond to a specific dialogue or character (e.g., `Dialogue_Guard_Greeting`, `Dialogue_Merchant_Quest`).
* Open the State Tree editor and build the dialogue flow using the following nodes:
    * **Dialogue Nodes:**
        * Set the "Speaker Tag" to the appropriate character tag.
        * Enter the dialogue text.
        * Select the desired "Camera Blend" asset.
        * Connect the "On Dialogue Ended Event" to the next state.
    * **Choice Nodes:**
        * Add elements to the "Choices" array. For each choice:
            * Enter the "Display Text".
            * Select the "Resulting Tag" to be added upon selection.
            * Connect the choice to the appropriate "Next State".
        * Select the desired "Camera Blend" asset.
    * **Condition Nodes:**
        * Select the "Tag to Check".
        * Choose the "Condition Type" ("Has Tag" or "Does Not Have Tag").
        * Connect the "True Branch" and "False Branch" to the respective next states.
    * **Action Nodes:**
        * Select the "Tag to Modify".
        * Choose the "Modification Type" ("Add Tag" or "Remove Tag").
        * Connect the "Next State".

**4. Create Dialogue Smart Object Definitions:**

* In the Content Browser, create a new Smart Object Definition asset.
* Name it appropriately (e.g., `SmartObject_DialogueCharacter`).
* Add a new Smart Object Slot to the definition.
* In the Slot's details, add the following User-Defined Data:
    * `DialogueStateTree` (Asset Reference to State Tree): Set the type to "State Tree".
    * `InitialDialogueTags` (Gameplay Tag Container): Set the type to "Gameplay Tag Container".

**5. Place Dialogue Smart Objects in the World:**

* Drag and drop an Actor of a character you want to have dialogue onto the level.
* Add a `SmartObjectComponent` to the character Actor.
* In the `SmartObjectComponent` details:
    * Set the "Smart Object Definition" to the `SmartObject_DialogueCharacter` asset created in the previous step.
    * Find the added Slot and in its details:
        * Select the appropriate "Dialogue State Tree" asset for this character.
        * Add any "Initial Dialogue Tags" that this character should start with.

**6. Implement Player Interaction:**

* In your Player Character or Player Controller Blueprint, implement the logic to detect interaction with Smart Objects (e.g., using a collision volume and the "On Component Begin Overlap" event).
* When a dialogue Smart Object is detected:
    * Get the `SmartObjectComponent`.
    * Get the "Dialogue State Tree" and "Initial Dialogue Tags" from the Smart Object's User-Defined Data.
    * Get the State Tree Component on the interacting character (or a dedicated dialogue actor).
    * Call the function to "Initialize State Tree" on the State Tree Component, providing the retrieved State Tree asset and Initial Dialogue Tags.
    * Call the function to "Start State Tree" on the State Tree Component.

**7. Test and Iterate:**

* Run the game and interact with the placed dialogue Smart Objects.
* Observe the dialogue flow, player choices, and camera behavior.
* Use Gameplay Tags to test different dialogue branches and ensure conditions are working as expected.
* If there are issues or desired changes, go back to the relevant step (e.g., editing the State Tree, modifying Gameplay Tags, adjusting camera blends) and iterate until the dialogue system functions as intended.

By following these steps, you can effectively utilize the integrated State Tree, Smart Object, Gameplay Camera, and Gameplay Tag systems to create engaging and dynamic dialogue experiences in your game. Remember to organize your Gameplay Tags and State Tree assets logically for better maintainability and scalability.