require "tundra.syntax.glob"
local native = require('tundra.native')

DefRule {
	Name = "BindGenerator",
	Pass = "CodeGeneration",
	Command = "$(BINDGEN) $(<) $(@)",
	ImplicitInputs = { "$(BINDGEN)" },

	Blueprint = {
		Source = { Required = true, Type = "string", Help = "Input filename", },
		OutName = { Required = true, Type = "string", Help = "Output filename", },
	},

	Setup = function (env, data)
		return {
			InputFiles    = { data.Source },
			OutputFiles   = { "$(OBJECTDIR)/_generated/" .. data.OutName },
		}
	end,
}

StaticLibrary {
	Name = "lua",

	Env = {
		CCOPTS = {
			{ "/wd4244", "/wd4702"; Config = "win*-*-*" },	-- double -> int64 (as used in LUA for LUA_NUMBER)
		},
	},

	Sources = Glob {
		Dir = "src/lua-5.2.3",
		Extensions = { ".c" },
	},
}

StaticLibrary {
	Name = "arika_internal",
	
	Env = { 
		CPPPATH = { 
			"include", 
			"src/lua-5.2.3/src", 
		}, 

		CCOPTS = {
			{ "/wd4305"; Config = "win*-*-*" },	-- type cast int64 -> void* 
		},
	},

	Sources = {
		"src/arika_internal/arika_internal.c",
		BindGenerator { Source = "include/Arika/Arika.h", OutName = "arika_exports.c" },
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

CSharpLib {
	Name = "Arika.NET",
	Sources = { 
		"src/dotnet/Arika.cs",
		BindGenerator { Source = "include/Arika/Arika.h", OutName = "ArikaRaw.cs" },
	},
}

--SharedLibrary {
SharedLibrary {
	Name = "arika-qt",

	Env = {
        CPPPATH = { 
            "include",
            "src",
            native.getenv("QT5", "") .. "/include",
            native.getenv("QT5", "") .. "/include/QtWidgets",
            native.getenv("QT5", "") .. "/include/QtCore",
            native.getenv("QT5", "") .. "/include/QtGui",
            native.getenv("QT5", "") .. "/lib/QtWidgets.framework/Headers",
            native.getenv("QT5", "") .. "/lib/QtCore.framework/Headers",
            native.getenv("QT5", "") .. "/lib/QtGui.framework/Headers",
        },

        LIBPATH = {
            { native.getenv("QT5", "") .. "/lib"; Config = { "win32-*-*", "win64-*-*" } },
        },

        CXXOPTS = { { 
        	-- Mark Qt headers as system to silence all the warnings from them
       	    "-isystem " .. native.getenv("QT5", "") .. "/lib/QtWidgets.framework/Headers", 
            "-isystem " .. native.getenv("QT5", "") .. "/lib/QtCore.framework/Headers", 
            "-isystem " .. native.getenv("QT5", "") .. "/lib/QtGui.framework/Headers", 
            "-isystem " .. native.getenv("QT5", "") .. "/lib/QtWidgets.framework/Versions/5/Headers", 
            "-isystem " .. native.getenv("QT5", "") .. "/lib/QtCore.framework/Versions/5/Headers", 
            "-isystem " .. native.getenv("QT5", "") .. "/lib/QtGui.framework/Versions/5/Headers", 
            "-F" .. native.getenv("QT5", "") .. "/lib",
            "-Wno-nested-anon-types",
			"-Wno-c99-extensions",
            "-Wno-c++98-compat-pedantic",
            "-Wno-padded",
            "-Wno-switch-enum",
            "-Wno-undefined-reinterpret-cast", -- Because of crap Qt code gen
            "-Wno-documentation"	-- Because clang warnings in a bad manner even if the doc is correct
            ; Config = "macosx-clang-*" },
        },

        SHLIBCOM = { 
            { 
            	"-F" .. native.getenv("QT5", "") .. "/lib",
            	"-lstdc++", "-rpath tundra-output$(SEP)macosx-clang-debug-default"; Config = "macosx-clang-*" },
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

	Env = { CPPPATH = { "include" }, },

	Sources = "examples/minimal/minimal.c",

	Depends = { "arika" },
}

Program {
	Name = "button",

	Env = { CPPPATH = { "include" }, },

	Sources = "examples/button/button.c",

	Depends = { "arika" },
}

Program {
	Name = "vbox_layout",

	Env = { CPPPATH = { "include" }, },

	Sources = "examples/vbox_layout/vbox_layout.c",

	Depends = { "arika" },
}

CSharpExe {
	Name = "csharp",
	Sources = { "examples/csharp/csharp.cs" },
	Depends = { "Arika.NET" },
}

Program {
	Name = "bind_generator",
	Pass = "CompileGenerator",
	Target = "$(BINDGEN)",

	Sources = Glob {
		Dir = "src/bind_generator",
		Extensions = { ".c" },
	},
}

if native.getenv("QT5", "") ~= "" then
 	Default "arika-qt"
end

Default "bind_generator"
Default "csharp"
Default "minimal"
Default "button"
Default "vbox_layout"
Default "Arika.NET"
