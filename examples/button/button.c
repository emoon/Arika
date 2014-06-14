#include <Arika/Arika.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	ARFuncs* arFuncs = ar_init(OBJECT_DIR, "arika-qt");

	if (!arFuncs)
		return 0;

	if (!arFuncs->ui_load("examples/button/button.ar"))
		return 0;

	//arFuncs->window_create_main();

	for (;;)
	{
		if (!arFuncs->update())
			break;
	}

	//arFuncs->close();

	return 0;
}

