#include "bind_parser.h" 
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

static const char* header = "using System;\nusing System.Runtime.InteropServices;\n\nnamespace Arika\n{\n\tpublic class ArikaRaw\n\t{\n";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Converts name from my_name_like_this -> MyNameLikeThis 

static void genCSName(char* out, const char* name)
{
	bool upperCaseNext = false;
	int len = strlen(name);

	// Always upper case the first char

	*out++ = (char)toupper(name[0]);

	for (int i = 1; i < len; ++i)
	{
		char c = name[i];

		if (upperCaseNext)
		{
			*out++ = (char)toupper(c);
			upperCaseNext = false;
		}
		else
		{
			if (c == '_')
				upperCaseNext = true;
			else
				*out++ = c;
		}
	}

	*out = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void writeType(FILE* f, const char* type)
{
	if (strstr(type, "*"))
		fprintf(f, "IntPtr");
	else
		fprintf(f, "%s", type);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void generateTypes(FILE* f, const BGFunction* func)
{
	fprintf(f, "\t\t[UnmanagedFunctionPointer(CallingConvention.Cdecl)]\n");
	fprintf(f, "\t\tpublic delegate IntPtr InitType();\n\n");

	while (func)
	{
		char csName[512];

		int paramCount = func->parameterCount;

		fprintf(f, "\t\t[UnmanagedFunctionPointer(CallingConvention.Cdecl)]\n");
		fprintf(f, "\t\tpublic delegate ");
		writeType(f, func->returnType);

		genCSName(csName, func->name);

		fprintf(f, " %sType(", csName);

		for (int i = 0; i < paramCount; ++i)
		{
			const BGParameter* param = &func->parameters[i];

			writeType(f, param->type);

			if (i == func->parameterCount - 1)	// handle the end of args where we shouldn't have a ,
				fprintf(f, " %s", param->variable);
			else
				fprintf(f, " %s,", param->variable);
		}

		fprintf(f, ");\n\n");

		func = func->next;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void generateDelegates(FILE* f, const BGFunction* func)
{
	fprintf(f, "\t\tpublic static InitType Init;\n");

	while (func)
	{
		char csName[512];

		fprintf(f, "\t\tpublic static ");

		genCSName(csName, func->name);

		fprintf(f, "%sType %s;\n", csName, csName);

		func = func->next;
	}

	fprintf(f, "\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void generateLoaderCode(FILE* f, const BGFunction* func)
{
	fprintf(f, "\t\tpublic static void ResolveSymbols(ArikaLoader.DllLoadUtils dllUtils, IntPtr dllHandle)\n\t\t{\n");
	fprintf(f, "\t\t\tInit = ArikaLoader.LoadFunction<InitType>(\"ar_init_funcs\", dllUtils, dllHandle);\n");

	while (func)
	{
		char csName[512];

		genCSName(csName, func->name);

		fprintf(f, "\t\t\t%s = ArikaLoader.LoadFunction<%sType>(\"ar_%s\", dllUtils, dllHandle);\n", csName, csName, func->name);

		func = func->next;
	}

	fprintf(f, "\t\t\tInit();\n");
	fprintf(f, "\t\t}\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int generate_cs(const char* filename, const BGFunction* func)
{
	FILE* f = fopen(filename, "wt");

	if (!f)
	{
		printf("Unable to open %s for write!", filename);
		return 0;
	}

	fprintf(f, "%s", header);

	generateTypes(f, func);
	generateDelegates(f, func);
	generateLoaderCode(f, func);

	fprintf(f, "\t}\n}\n");

	return 0;
}

