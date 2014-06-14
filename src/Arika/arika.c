#include <Arika/Arika.h>
#include <dlfcn.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ARFuncs* ar_init(const char* path, const char* sharedLib)
{
	char fullPath[4096];

#ifdef __APPLE__
	sprintf(fullPath, "%s/lib%s.dylib", path, sharedLib);
#elif _WIN32
	sprintf(fullPath, "%s/%.dll", path, sharedLib);
#else
	sprintf(fullPath, "%s/%.so", path, sharedLib);
#endif

	void* (*initFunc)();
	void* handle = dlopen(fullPath, RTLD_LOCAL | RTLD_LAZY);

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
