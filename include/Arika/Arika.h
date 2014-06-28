#ifndef _ARIKA_H_
#define _ARIKA_H_

#ifdef __cplusplus
extern "C" {
#endif

struct ARWidget;
struct ARLayout;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ARFuncs
{
	// Widgets

	struct ARWidget* (*window_create_main)();
	struct ARWidget* (*window_create)();
	struct ARWidget* (*button_create)();
	struct ARWidget* (*list_view_create)();

	// Layout

	struct ARLayout* (*layout_vbox_create)();
	struct ARLayout* (*layout_hbox_create)();

	int (*layout_add)(struct ARLayout* layout, struct ARWidget* widget);
	int (*layout_remove)(struct ARLayout* layout, struct ARWidget* widget);

	// Sizeing

	int (*widget_set_title)(struct ARWidget* widget, const char* title); 
	int (*widget_set_text)(struct ARWidget* widget, const char* text); 
	int (*widget_set_width)(struct ARWidget* widget, int width); 
	int (*widget_set_height)(struct ARWidget* widget, int height); 
	int (*widget_set_layout)(struct ARWidget* widget, struct ARLayout* layout);

	int (*widget_attach)(struct ARWidget* parent, struct ARWidget* widget); 

	int (*update)();

	// private data and functions

	int (*ui_load)(const char* filename);

	void* privateData;

} ARFuncs;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ARFuncs* ar_init(const char* path, const char* sharedLibrary);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ar_button_create(context, parent) context->button_create(context, parent)

#ifdef __cplusplus
}
#endif

#endif
