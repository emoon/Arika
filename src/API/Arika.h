#ifndef _ARIKA_H_
#define _ARIKA_H_

#ifndef __cplusplus
extern "C"
#endif

struct ARControl;
struct ARLayout;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ARFuncs
{
	struct ARControl* (*createButton)(struct ARFuncs* funcs, ARControl* parent, void* userData);

	// Layout

	int (*createVbox)();


	// Sizeing

	int (*setWidth)(struct ARControl* control); 
	int (*setHeight)(struct ARControl* control); 


} ARFuncs;


#define AR_createButton(context, parent, userData) context->createButton(context, parent, userData)

#endif
