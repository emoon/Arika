#include "bind_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

static bool g_debugPrint = true;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A general notice of this code:
// This is *not* by any means a generic header parser. It's written to handle the input of Arika.h
// an no more no less and if you do "weird" things in it this code will break. It's based around the
// structure of how the header look so if you mess around with that fill too much this code
// needs to be updated.

enum State
{
	STATE_STRUCT,
	STATE_FIND_FUNCTIONS,
	STATE_BLOCK_COMMENT,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum FunctionState
{
	STATE_FIND_RETURN,
	STATE_FIND_FUNC_NAME,
	STATE_FIND_PARAMS,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The way we qualify that something is a function:
// 1. has 2 (...) and another set (...) like : (*foo)(int meh)
// 2. Doesn't have any // before any of part 1

static bool isFunction(const char* line, int length)
{
	int i = 0;
	int left_param = 0;
	int right_param = 0;

	for (i = 0; i < length; ++i)
	{
		char c = line[i];

		if (c == '(')
			left_param++;

		if (c == ')')
			right_param++;

		if (c == '/')
		{
			// this is safe to assume we can read from one forward as a last entry in a line should
			// never end with / anyway

			if (line[i + 1] == '/')
				break;
		}
	}

	return left_param == 2 && right_param == 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int findParantRange(const char* line, int length, int* outStart, int* outEnd)
{
	int i, start = -1, end = -1, len;

	for (i = 0; i < length; ++i)
	{
		if (line[i] == '(')
		{
			start = i;
			break;
		}
	}

	if (start == -1)
		return 0;

	for (i = 0; i < length; ++i)
	{
		if (line[i] == ')')
		{
			end = i;
			break;
		}
	}

	if (end == -1)
		return 0;

	len = end - start;
	*outStart = start;
	*outEnd = end;

	return len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char* findReturn(const char* line, int lineLength)
{
	char* retType = 0;
	int i, start = -1, end = -1;
	int length = 0;

	// find start

	for (i = 0; i < lineLength; ++i)
	{
		char c = line[i];

		if (c != ' ' || c != '\t')
		{
			start = i;
			break;
		}
	}

	if (start == -1)
		return 0;
	
	// find end

	for (i = 0; i < lineLength; ++i)
	{
		if (line[i] == '(')
		{
			end = i;
			break;
		}
	}

	if (end == -1)
		return 0;

	length = end - start;

	assert(length > 0);

	retType = malloc(length);
	memcpy(retType, &line[start], length);

	// we stomp the last char here (which is a ' ') intentionally
	retType[length] = 0;

	return retType;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This assumes functions look like: (*foo)

static char* findFunctionName(const char* line, int length, int* outEnd)
{
	int start, functionLen;
	char* functionName;

	functionLen = findParantRange(line, length, &start, outEnd);

	functionName = malloc(functionLen);
	memcpy(functionName, &line[start + 2], functionLen); // + 2 to skip (*

	functionName[functionLen - 1] = 0; // - 1 to stomp )

	return functionName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void parseParameters(BGFunction* function, const char* line, int length)
{
	char newLine[256];
	int i, start, end, len;
	char* token;
	BGParameter* parameter;
	int paramCount = 0;

	len = findParantRange(line, length, &start, &end);
	memcpy(newLine, line + start, len);
	newLine[len + 1] = 0;

	// Count parameters

	token = strtok(newLine, ",");
	while (token) 
	{
		token = strtok(NULL, ",");
		paramCount++;
	}

	// parse parameters

	function->parameters = parameter = malloc(sizeof(BGParameter) * paramCount);
	function->parmCount = paramCount;

	token = strtok(newLine, ",");
	while (token) 
	{
		int count = 0;
		len = strlen(token);

		// find name

		for (i = len; i > 0; --i)
		{
			if (token[i] == ' ')
			{
				parameter->variable = malloc(count);
				memcpy(parameter->variable, &token[i], count + 1);
				parameter->variable[count + 1] = 0;
			}

			count++;
		}

		// get type

		parameter->type = malloc(len - (count + 1));
		memcpy(parameter->type, token, len - count);
		parameter->type[len - count] = 0;

		token = strtok(NULL, ",");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this code will assume functions look like this;
// int (*myFunc)(struct MyStruct param, struct MyStruct2 param2);

static BGFunction* parseFunction(const char* line, int length)
{
	int startParam;
	BGFunction* function = malloc(sizeof(BGFunction));
	memset(function, 0, sizeof(BGFunction));

	function->returnType = findReturn(line, length);
	function->name = findFunctionName(line, length, &startParam);

	parseParameters(function, line + startParam, length - startParam);

	if (g_debugPrint)
	{
		int i;

		printf("========================================================================\n");
		printf("Function name %s", function->name);
		printf("Function retType %s", function->returnType);

		for (i = 0; i < function->parmCount; ++i)
		{
			printf("Parameter %s - %s\n",
				function->parameters[i].type,
				function->parameters[i].variable);
		}
	}

	return function;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static BGFunction* parseLine(const char* line)
{
	int lineLength = strlen(line);

	if (g_debugPrint)
		printf("Parsing line: %s", line);

	if (!isFunction(line, lineLength))
	{
		if (g_debugPrint)
			printf("Line: %s is not a function", line);

		return 0;
	}

	return parseFunction(line, lineLength);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static BGFunction* parseHeader(const char* filename)
{
	char line[256];
	int state = STATE_STRUCT;
	BGFunction* function = 0;
	BGFunction* firstFunction = 0;

	FILE* f = fopen(filename, "rt");

	if (!f)
	{
		printf("Unable to open %s for reading\n", filename);
		return 0;
	}

	while (fgets(line, sizeof(line), f))
	{
		switch (state)
		{
			case STATE_STRUCT:
			{
				 if (!strcmp(line, "typedef struct ARFuncs"))
				 {
					state = STATE_FIND_FUNCTIONS;

					if (g_debugPrint)
						printf("Switching to looking for functions\n");
				 }

				break;
			}

			case STATE_FIND_FUNCTIONS:
			{
				BGFunction* func;

				if (!strcmp(line, "} ARFuncs;"))
				{
					if (g_debugPrint)
						printf("End of function hunt\n");

					goto end;
				}

				func = parseLine(line);

				if (func)
				{
					if (!firstFunction)
					{
						function = firstFunction = func;
					}
					else
					{
						function->next = func;
						function = func;
					}
				}

				break;
			}

			case STATE_BLOCK_COMMENT:
			{
				break;
			}
		}
	}

end:;

	fclose(f);

	return firstFunction;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
	if (argc < 4)
	{
		printf("usage: input.h <output type> <output file>\nExample: bind_generator foo.h --cs out.cs");
		return 0;
	}

	parseHeader(argv[1]);

	return 0;
}

