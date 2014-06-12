#include <Arika/Arika.h>
#include <dlfcn.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ARFuncs* ar_init(const char* sharedLib)
{
	void* (*initFunc)();
	void* handle = dlopen(sharedLib, RTLD_LOCAL | RTLD_LAZY);

	if (!handle)
	{
		printf("Unable to open %s\n", sharedLib);
		return 0;
	}

	void* function = dlsym(handle, "ar_init_funcs");

	if (!function)
	{
		printf("Unable to find ar_init_funcs in %s\n", sharedLib);
		return 0;
	}

	*(void **)(&initFunc) = function;
	return initFunc();
}
