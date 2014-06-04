#ifndef _ARIKA_H_
#define _ARIKA_H_

#ifndef __cplusplus
extern "C"
#endif

struct ARWidget;
struct ARLayout;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ARFuncs
{
	// Widgets

	struct ARWidget* (*window_create)(struct ARFuncs* funcs);
	struct ARWidget* (*button_create)(struct ARFuncs* funcs);
	struct ARWidget* (*list_view_create)(struct ARFuncs* funcs);

	// Layout

	struct ARLayout* (*layout_vbox_create)();
	struct ARLayout* (*layout_hbox_create)();

	int (*layout_add)(struct ARWidget* widget);
	int (*layout_remove)(struct ARWidget* widget);

	// Sizeing

	int (*widget_set_width)(struct ARWidget* widget); 
	int (*widget_set_height)(struct ARWidget* widget); 

} ARFuncs;


#define ar_button_create(context, parent) context->button_create(context, parent)

#endif
