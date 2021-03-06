#include "bind_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

static bool g_debugPrint = false;
extern int generate_exp(const char* filename, const BGFunction* func);
extern int generate_cs(const char* filename, const BGFunction* func);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A general notice of this code:
// This is *not* by any means a generic header parser. It's written to handle the input of Arika.h
// an no more no less and if you do "weird" things in it this code will break. It's based around the
// structure of how the header look so if you mess around with that fill too much this code
// needs to be updated.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The way we qualify that something is a function:
// 1. has 2 (...) and another set (...) like : (*foo)(int meh)
// 2. Doesn't have any // before any of part 1

static bool isFunction(const char* line, int length)
{
	int left_param = 0;
	int right_param = 0;

	for (int i = 0; i < length; ++i)
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
// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.

static char* trimWhitespace(char *str)
{
	char* end;

	// Trim leading space
	while (isspace(*str)) 
		str++;

	if (*str == 0)  // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;

	while (end > str && isspace(*end)) 
		end--;

	// Write new null terminator
	*(end + 1) = 0;

	return str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char* strcpyAndTrim(const char* str, int len)
{
	char* newStr = malloc(len + 256);
	memset(newStr, 0, len + 256);
	memcpy(newStr, str, len + 1);
	newStr[len] = 0;

	return trimWhitespace(newStr);
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
	int i, start = -1, end = -1;
	int length = 0;

	// find start

	for (i = 0; i < lineLength; ++i)
	{
		char c = line[i];

		if (c == ' ' || c == '\t')
			continue;

		start = i;
		break;
	}

	if (start == -1)
		return 0;
	
	// find end

	for ( ; i < lineLength; ++i)
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

	return strcpyAndTrim(&line[start], length);
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

	functionName[functionLen - 2] = 0; // - 1 to stomp )

	*outEnd += 1; // start on next entry

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

	len = findParantRange(line, length, &start, &end) - 1;

	if (len <= 1)
		return;

	memcpy(newLine, line + start + 1, len + 3);
	newLine[len + 0] = ',';
	newLine[len + 1] = 0;

	function->parameters = parameter = malloc(sizeof(BGParameter) * 128);	// ugly but whatever...

	// Count parameters

	token = strtok(newLine, ",");
	while (token) 
	{
		int count = 0;
		len = (int)strlen(token);

		// find name

		for (i = len; i > 0; --i)
		{
			if (token[i] == ' ')
			{
				parameter->variable = strcpyAndTrim(&token[i + 1], count);
				break;
			}

			count++;
		}

		parameter->type = strcpyAndTrim(token, i);

		token = strtok(NULL, ",");

		paramCount++;
		parameter++;

		assert(paramCount < 128);
	}

	function->parameterCount = paramCount;
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

		printf("Function name %s\n", function->name);
		printf("Function retType %s\n", function->returnType);

		for (i = 0; i < function->parameterCount; ++i)
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
	int lineLength = (int)strlen(line);

	if (!isFunction(line, lineLength))
		return 0;

	return parseFunction(line, lineLength);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static BGFunction* parseHeader(const char* filename)
{
	char line[256];
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
		BGFunction* func = parseLine(line);

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

		memset(line, 0, sizeof(line));
	}

	fclose(f);

	return firstFunction;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int getExt(char* dest, const char* line)
{
	int i;
	int len = (int)strlen(line);

	for (i = len; i > 0; --i)
	{
		if (line[i] == '.')
			break;
	}

	if (i == 0)		// no . found
		return 0;

	strcpy(dest, &line[i]);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
	char fileExt[64];
	BGFunction* functions;

	if (argc != 3)
	{
		printf("usage: <input file> <output file>\nExample: bind_generator foo.h out.cs");
		return 0;
	}

	functions = parseHeader(argv[1]);

	if (!getExt(fileExt, argv[2]))
	{
		printf("Unable to find file extension for %s", argv[2]);
		return -1;
	}

	// This is a bit hacky but like this because of extra params handling in tundra. Hopefully
	// this can be fixed by sending is some proper commandline args instead

	if (!strcmp(fileExt, ".c"))
	{
		if (!generate_exp(argv[2], functions))
			return -1;
	}
	else if (!strcmp(fileExt, ".cs"))
	{
		generate_cs(argv[2], functions);
	}

	return 0;
}

