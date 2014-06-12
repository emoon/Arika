#include <Arika/Arika.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	ARFuncs* arFuncs = ar_init("t2-output/macosx-clang-debug-default/libarika-qt.dylib");

	if (!arFuncs)
		return 0;

	arFuncs->window_create_main();

	for (;;)
	{
		if (!arFuncs->update())
			break;
	}

	//arFuncs->close();

	return 0;
}
