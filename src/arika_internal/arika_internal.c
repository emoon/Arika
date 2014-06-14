#include <Arika/Arika.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>

#define sizeof_array(array) (int)(sizeof(array) / sizeof(array[0]))

static ARFuncs* s_arFuncs;
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

			if (!strcmp(key, "Title"))
				s_arFuncs->widget_set_title(widget, value);
        }
        if (lua_isnumber(state, -1))
        {
			const char* key = lua_tostring(state, -2); 
			LUA_NUMBER value = lua_tonumber(state, -1);

			printf("key = %s value = %d\n", key, (int)value);
		
			if (!strcmp(key, "Width"))
			{
       			s_arFuncs->widget_set_width(widget, (int)value);
       			printf("set width %d\n", (int)value);
  			}
			else if (!strcmp(key, "Height"))
			{
       			s_arFuncs->widget_set_height(widget, (int)value);
       			printf("set height %d\n", (int)value);
  			}
			else if (!strcmp(key, "widget"))
			{
				struct ARWidget* cw = (struct ARWidget*)value;
				s_arFuncs->widget_attach(widget, cw);
			}
        }
		if (lua_istable(state, -1))
        {
        	printf("traversing table..\n");
			traverseTable(widget, state);
        }

        lua_pop(state, 1);
    }

    return 1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int window_main_create(lua_State* state)
{
	s_main_window = s_arFuncs->window_create_main();
	traverseTable(s_main_window, state); 

	return 0; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int pushbutton_create(lua_State* state)
{
	struct ARWidget* widget = s_arFuncs->button_create();
	traverseTable(widget, state); 

	lua_newtable(state);

	lua_pushstring(state, "widget");
	lua_pushnumber(state, (LUA_NUMBER)widget); 
	lua_settable(state, -3);

	return 1; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LuaFuncs s_funcs[] =
{
	{ window_main_create, "MainWindow" },
	{ pushbutton_create, "PushButton" },
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ui_load(const char* filename)
{
	lua_State* state = ((ARInternal*)s_arFuncs->privateData)->luaState;

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

	s_arFuncs = funcs;

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
