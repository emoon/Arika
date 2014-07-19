#include <Arika/Arika.h>
#include <Arika_internal/Arika_internal.h>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

static QApplication* s_application;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ARWidget
{
	ARWidget()
	{
		mainWindow = 0;
		widget = 0;
	}

	QWidget* mainWindow;
	QWidget* widget;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ARLayout
{
	QLayout* layout;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T> ARWidget* createWidget()
{
	T* widget = new T;

	ARWidget* arWidget = new ARWidget;
	arWidget->widget = widget; 

	return arWidget; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ARWidget* windowCreateMain()
{
	ARWidget* arWidget = createWidget<QMainWindow>(); 

    QWidget* t = new QWidget(arWidget->widget);
    arWidget->mainWindow = arWidget->widget;
    arWidget->widget = t;

    t->resize(500, 500);
/*

    QPushButton* button = new QPushButton("Test");
    //new QPushButton("Meh", t);

    //button->setParent(t);
    
    QVBoxLayout* layout = new QVBoxLayout(t);
    layout->addWidget(button);

    QPushButton* b0 = new QPushButton("Test");
    QPushButton* b1 = new QPushButton("Test1");
    QPushButton* b2 = new QPushButton("Test2");
    QPushButton* b3 = new QPushButton("Test3");

    layout->addWidget(b0);
    layout->addWidget(b1);
    layout->addWidget(b2);
    layout->addWidget(b3);

	t->setLayout(layout);

	arWidget->mainWindow->resize(1024, 768);
*/
	arWidget->mainWindow->show();

	//printf("exec\n");

	//s_application->exec();

	//printf("end\n");

	//exit(0);

	//printf("after exit\n");

	return arWidget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ARWidget* pushButtonCreate()
{
	ARWidget* arWidget = createWidget<QPushButton>(); 
	//QPushButton* button = (QPushButton*)arWidget->widget;
	//button->setText("Test");

	printf("creating pushbutton\n");

	return arWidget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int widgetSetTitle(ARWidget* arWidget, const char* title)
{
	arWidget->widget->setWindowTitle(title);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int widgetSetText(ARWidget* arWidget, const char* title)
{
	((QAbstractButton*)arWidget->widget)->setText(title);
	return 1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int widgetSetHeight(ARWidget* arWidget, int v)
{
	QWidget* w = arWidget->widget;

	if (arWidget->mainWindow)
		w = arWidget->mainWindow;

	QSize size = w->size(); 
	size.setHeight(v);
	w->resize(size);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int widgetSetWidth(ARWidget* arWidget, int v)
{
	QWidget* w = arWidget->widget;

	if (arWidget->mainWindow)
		w = arWidget->mainWindow;

	QSize size = w->size(); 
	size.setWidth(v);
	w->resize(size);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int widgetAttach(ARWidget* arParent, ARWidget* arWidget)
{
	arWidget->widget->setParent(arParent->widget);
	arWidget->widget->show();
	arParent->widget->update();

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct ARLayout* vboxCreate()
{
	ARLayout* arLayout = new ARLayout;
	QVBoxLayout* layout = new QVBoxLayout;

	arLayout->layout = layout;

	printf("vboxCreate\n");

	return arLayout;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int layoutAdd(struct ARLayout* arLayout, struct ARWidget* arWidget)
{
	arLayout->layout->addWidget(arWidget->widget);

	printf("layout Add\n");

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int widgetSetLayout(struct ARWidget* arWidget, struct ARLayout* arLayout)
{
	arWidget->widget->setLayout(arLayout->layout);

	printf("set layout\n");

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
	// Widgets

	windowCreateMain,
	0,
	pushButtonCreate,
	0,

	// Layout

	vboxCreate,
	0,

	layoutAdd,
	0,

	// Sizeing

	widgetSetTitle,
	widgetSetText,
	widgetSetWidth,
	widgetSetHeight,
	widgetSetLayout,
	widgetAttach,
	update,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void init()
{
	// must be static as QApplication constructor depends on it
	static int argc = 1;
	static const char* dummyStrings[] =
	{
		"dummy",
		"dummy",
		"dummy",
	};

	s_application = new QApplication(argc, (char**)&dummyStrings);

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
