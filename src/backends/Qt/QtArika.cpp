#include <Arika.h>
#include <QApplication>
#include <QMainWindow>

static ARFuncs s_arFuncs;
static QApplication* s_application;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ARWidget
{
	QWidget* widget;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ARWidget* windowCreateMain()
{
	QMainWindow* mainWindow = new QMainWindow;

	ARWidget* arWidget = new ARWidget; 
	arWidget->widget = mainWindow; 

	mainWindow->show();

	return arWidget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int update()
{
	s_application->processEvents();

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void init()
{
	int argc = 0;
	s_application = new QApplication(argc, (char**)0);

	memset(&s_arFuncs, 0, sizeof(s_arFuncs));

	s_arFuncs.window_create_main = windowCreateMain; 
	s_arFuncs.update = update; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// init

ARFuncs* ar_init_funcs()
{
	init();

	return &s_arFuncs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}