#include "TagValueBase.h"

bool FBaseTagValue::Serialize(FArchive& Ar)
{
	// Base implementation for serialization
	// Override in derived classes as needed
	return true;
}

bool FBaseTagValue::operator==(const FBaseTagValue& Other) const
{
	// Base implementation for equality comparison
	// Default comparison is by type
	return GetValueType() == Other.GetValueType();
}
