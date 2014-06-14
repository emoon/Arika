#include <Arika/Arika.h>
#include <Arika_internal/Arika_internal.h>
#include <QApplication>
#include <QMainWindow>

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

static int widgetSetTitle(ARWidget* arWidget, const char* title)
{
	arWidget->widget->setWindowTitle(title);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int update()
{
	s_application->processEvents();

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ARFuncs s_arFuncs = 
{
	.window_create_main = windowCreateMain,
	.widget_set_title = widgetSetTitle,
	.update = update,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void init()
{
	int argc = 0;
	s_application = new QApplication(argc, (char**)0);

	ar_internal_init(&s_arFuncs);
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
