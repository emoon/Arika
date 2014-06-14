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


void stackDump(lua_State *L)  
{  
    int i;  
    int top = lua_gettop(L);  
    printf("the size of stack is:%d\n",top);  
    for ( i = 1;i <= top;i++ )  
    {  
        int type = lua_type(L, i);  
        switch(type)  
        {  
        case LUA_TSTRING:  
            {  
                printf("%s",lua_tostring(L, i));  
                break;  
            }  
  
        case LUA_TBOOLEAN:  
            {  
                printf(lua_toboolean(L, i)?"true":"false");  
                break;  
            }  
        case LUA_TNUMBER:  
            {  
                printf("%g",lua_tonumber(L, i));  
                break;  
            }  
        case LUA_TTABLE:  
            {  
                printf("this is a table!");  
                break;  
            }  
        default:  
            {  
                printf("%s",lua_typename(L ,i));  
                break;  
            }  
        }  
        printf(" ");  
    }  
    printf("\n");  
}  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int window_main_create(lua_State* state)
{
	//int argc;
	//const char* title = 0;

	s_main_window = s_arFuncs->window_create_main();
	printf("creating main widow from lua!\n");

	// make sure wa have a table as input

	//luaL_checktable(state, 1);

	lua_pushnil(state);

    while(lua_next(state, -2) != 0)
    {
		const char* key = lua_tostring(state, -2);

		if (!strcmp(key, "Title"))
		{
			const char* value = luaL_checkstring(state, -1);
       		s_arFuncs->widget_set_title(s_main_window, value);
		}

        lua_pop(state, 1);
    }

	//printf("count %d\n", argc);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LuaFuncs s_funcs[] =
{
	{ window_main_create, "MainWindow" },
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ui_load(const char* filename)
{
	lua_State* state = ((ARInternal*)s_arFuncs->privateData)->luaState;

	if (luaL_loadfile(state, filename) || lua_pcall(state, 0, 0, 0) != 0)
	{
		printf("Failed to load %s\n", filename);
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
