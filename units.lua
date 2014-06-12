require "tundra.syntax.glob"
local native = require('tundra.native')

StaticLibrary {
	Name = "lua",

	Sources = Glob {
		Dir = "src/lua-5.2.3",
		Extensions = { ".c" },
	},
}

StaticLibrary {
	Name = "arika_internal",

	Sources = Glob {
		Dir = "src/arika_internal",
		Extensions = { ".c" },
	},
}

StaticLibrary {
	Name = "arika",

	Env = { CPPPATH = { "include", } },

	Sources = Glob {
		Dir = "src/Arika",
		Extensions = { ".c" },
	},
}

SharedLibrary {
	Name = "arika-qt",

	Env = {
        CPPPATH = { 
            "include",
            "$(QT5)/include",
            "$(QT5)/include/QtWidgets",
            "$(QT5)/include/QtGui",
            "$(QT5)/include/QtCore", 
            "$(QT5)/lib/QtWidgets.framework/Headers", 
            "$(QT5)/lib/QtCore.framework/Headers", 
            "$(QT5)/lib/QtGui.framework/Headers", 
        },

        LIBPATH = {
            { "$(QT5)/lib"; Config = { "win32-*-*", "win64-*-*" } },
        },

        CXXOPTS = { { 
        	-- Mark Qt headers as system to silence all the warnings from them
            "-isystem $(QT5)/lib/QtWidgets.framework/Headers", 
            "-isystem $(QT5)/lib/QtCore.framework/Headers", 
            "-isystem $(QT5)/lib/QtGui.framework/Headers", 
            "-isystem $(QT5)/lib/QtWidgets.framework/Versions/5/Headers", 
            "-isystem $(QT5)/lib/QtCore.framework/Versions/5/Headers", 
            "-isystem $(QT5)/lib/QtGui.framework/Versions/5/Headers", 
            "-F$(QT5)/lib",
            "-std=gnu0x",
            "-Wno-c++98-compat-pedantic",
            "-Wno-padded",
            "-Wno-switch-enum",
            "-Wno-undefined-reinterpret-cast", -- Because of crap Qt code gen
            "-Wno-documentation",	-- Because clang warnings in a bad manner even if the doc is correct
            "-std=c++11" ; Config = "macosx-clang-*" },
        },

        SHLIBCOM = { 
            { "-F$(QT5)/lib", "-lstdc++", "-rpath tundra-output$(SEP)macosx-clang-debug-default"; Config = "macosx-clang-*" },
        },

    },

	Sources = Glob {
		Dir = "src/backends/Qt",
		Extensions = { ".cpp" },
	},

	Depends = { "lua", "arika_internal" },

	Frameworks = { "Cocoa", "QtWidgets", "QtGui", "QtCore", "QtConcurrent" },
}

Program {
	Name = "minimal",

	Env = { 
        CPPPATH = { "include" },
    },

	Sources = "examples/minimal/minimal.c",

	Depends = { "arika" },
}


if native.getenv("QT5", "") ~= "" then
	Default "arika-qt"
end

Default "minimal"
