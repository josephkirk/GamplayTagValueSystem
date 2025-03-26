Created a new inheritance-based tag value system:
Created a base FBaseTagValue struct with virtual GetValueType() and serialization support
Created type-specific derived structs (FBoolTagValue, FIntTagValue, etc.) that inherit from FBaseTagValue
Each derived struct has a single Value field of the appropriate type
Created a FTagValueContainer struct to manage collections of tag values:
Uses TMap<FGameplayTag, TSharedPtr> for storage
Provides type-safe template methods for getting and setting values
Includes utility methods like HasValue, RemoveValue, GetAllTags, etc.
Created a Blueprint Function Library (UTagValueBlueprintLibrary) to make the system accessible from Blueprints:
Type-specific getter and setter functions for each value type
General container management functions
The new implementation has several advantages over the original implementation:

Better type safety through inheritance
More flexible with TSharedPtr for memory management
More maintainable with separate structs for different types
Better organization with clear separation of concerns
Easily extensible for adding new value types
Serialization support built in
I've improved the current implementation in the Source directory based on the suggestions in ScriptStruct.md. Here's a summary of the changes I've made:

1. Created a Type-Safe Inheritance Structure
Created a new FBaseTagValue base struct with virtual type identification and serialization support
Implemented type-specific derived structs (like FBoolTagValue, FFloatTagValue, etc.) that inherit from the base
Each derived type contains a single typed Value property instead of having all value types in one struct
2. Implemented a Generic Tag Value Container
Created FTagValueContainer that uses a TMap<FGameplayTag, TSharedPtr<FBaseTagValue>> for storage
Added type-safe template methods for getting and setting values
Implemented common container operations (HasValue, RemoveValue, GetAllTags, etc.)
3. Added Blueprint Support
Created UTagValueBlueprintLibrary with type-specific getter and setter functions
Implemented support for all the original value types plus a new string type
All functions are properly categorized for easy Blueprint access
The new implementation offers several advantages:

Better type safety through inheritance
Memory management via TSharedPtr
More maintainable with separate structs for each type
Easier to extend with new value types
Serialization support built-in
Cleaner API for both C++ and Blueprint users