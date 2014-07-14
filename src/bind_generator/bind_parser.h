#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct BGParameter
{
	char* type;
	char* variable;
} BGParameter;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct BGFunction
{
	BGParameter* parameters;
	char* name;
	char* returnType;
	int parmCount;
	struct BGFunction* next;

} BGFunction;

