#include <Arika/Arika.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	ARFuncs* arFuncs = ar_init(OBJECT_DIR, "arika-qt");

	if (!arFuncs)
		return 0;

	if (!arFuncs->ui_load("examples/vbox_layout/vbox_layout.ar"))
		return 0;

	for (;;)
	{
		if (!arFuncs->update())
			break;
	}

	return 0;
}

