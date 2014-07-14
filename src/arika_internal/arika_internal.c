#include <Arika/Arika.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>

#define sizeof_array(array) (int)(sizeof(array) / sizeof(array[0]))

ARFuncs* g_arFuncs = 0;
static struct ARWidget* s_main_window;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct LuaFuncs
{
	lua_CFunction func;
	const char* name;
} LuaFuncs;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ArInternal
{
	struct lua_State* luaState; 

} ARInternal;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int traverseTable(struct ARWidget* widget, lua_State* state)
{
	lua_pushnil(state);

    while (lua_next(state, -2) != 0)
    {
        if (lua_isstring(state, -1))
        {
			const char* key = lua_tostring(state, -2);
			const char* value = lua_tostring(state, -1);

			if (!strcmp(key, "Text"))
				g_arFuncs->widget_set_text(widget, value);
			else if (!strcmp(key, "Title"))
				g_arFuncs->widget_set_title(widget, value);
        }

        if (lua_isnumber(state, -1))
        {
			const char* key = lua_tostring(state, -2); 
			LUA_NUMBER value = lua_tonumber(state, -1);

			if (!strcmp(key, "Width"))
			{
       			g_arFuncs->widget_set_width(widget, (int)value);
  			}
			else if (!strcmp(key, "Height"))
			{
       			g_arFuncs->widget_set_height(widget, (int)value);
  			}
			else if (!strcmp(key, "widget"))
			{
				struct ARWidget* cw = (struct ARWidget*)(unsigned __int64)value;
				g_arFuncs->widget_attach(widget, cw);
			}
			else if (!strcmp(key, "layout"))
			{
				struct ARLayout* layout = (struct ARLayout*)value;
				g_arFuncs->widget_set_layout(widget, layout);
			}
        }

		if (lua_istable(state, -1))
        {
			traverseTable(widget, state);
        }

        lua_pop(state, 1);
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int traverseTableLayout(struct ARLayout* layout, lua_State* state)
{
	lua_pushnil(state);

    while (lua_next(state, -2) != 0)
    {
        if (lua_isnumber(state, -1))
        {
			const char* key = lua_tostring(state, -2); 
			LUA_NUMBER value = lua_tonumber(state, -1);

			if (!strcmp(key, "widget"))
			{
				struct ARWidget* cw = (struct ARWidget*)value;
				g_arFuncs->layout_add(layout, cw);
			}
        }

		if (lua_istable(state, -1))
        {
			traverseTableLayout(layout, state);
        }

        lua_pop(state, 1);
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int window_main_create(lua_State* state)
{
	s_main_window = g_arFuncs->window_create_main();
	traverseTable(s_main_window, state); 

	return 0; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int pushbutton_create(lua_State* state)
{
	struct ARWidget* widget = g_arFuncs->button_create();
	traverseTable(widget, state); 

	lua_newtable(state);

	lua_pushstring(state, "widget");
	lua_pushnumber(state, (LUA_NUMBER)widget); 
	lua_settable(state, -3);

	return 1; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int layout_vbox_create(lua_State* state)
{
	struct ARLayout* layout = g_arFuncs->layout_vbox_create();
	traverseTableLayout(layout, state); 

	lua_newtable(state);

	lua_pushstring(state, "layout");
	lua_pushnumber(state, (LUA_NUMBER)layout); 
	lua_settable(state, -3);

	return 1; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LuaFuncs s_funcs[] =
{
	{ window_main_create, "MainWindow" },
	{ pushbutton_create, "PushButton" },
	{ layout_vbox_create, "VBoxLayout" },
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ui_load(const char* filename)
{
	lua_State* state = ((ARInternal*)g_arFuncs->privateData)->luaState;

	if (luaL_loadfile(state, filename) || lua_pcall(state, 0, 0, 0) != 0)
	{
		printf("Failed to load %s - %s\n", filename, lua_tostring(state, -1));
		return 0;
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ar_internal_init(ARFuncs* funcs)
{
	ARInternal* state;
	int i = 0;
	
	state = malloc(sizeof(ARInternal));

	g_arFuncs = funcs;

	printf("trying to create State\n");

	state->luaState = luaL_newstate();
	luaL_openlibs(state->luaState);

	printf("end libs\n");

	funcs->ui_load = ui_load;
	funcs->privateData = state;

	for (i = 0; i < sizeof_array(s_funcs); ++i)
	{
		lua_pushcfunction(state->luaState, s_funcs[i].func);
		lua_setglobal(state->luaState, s_funcs[i].name);
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void dummy()
{
}
