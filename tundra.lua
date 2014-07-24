require "tundra.syntax.glob"
local native = require('tundra.native')

-- Will setup the enviroment for QT5 and other third party UI libs

local common =
{
	Env = {
		QT5 = native.getenv("QT5", ""),
		BINDGEN = "$(OBJECTDIR)$(SEP)bind_generator$(PROGSUFFIX)",
	},
}

local win32 = {
	Inherit = common,

	Env = {
 		GENERATE_PDB = "1",
		CCOPTS = {
			"/FS",
			"/W4", 
			"/WX", "/I.", "/D_CRT_SECURE_NO_WARNINGS", "\"/DOBJECT_DIR=$(OBJECTDIR:#)\"",
			{ "/Od"; Config = "*-*-debug" },
			{ "/O2"; Config = "*-*-release" },
		},

		PROGOPTS = {
			"/INCREMENTAL:NO"-- Disable incremental linking. It doesn't work properly in our use case (nearly all code in libs) and causes log spam.
		},

	},
}

local macosx = {
	Inherit = common,

	Env = {
		CSOPTS = {
			"-g",
		},

		QT5 = native.getenv("QT5", ""),
		CCOPTS = {
			"-Wpedantic", "-Werror", "-Wall", "-Wno-deprecated-declarations", "-DOBJECT_DIR=\\\"$(OBJECTDIR)\\\"",
			{ "-O0", "-g"; Config = "*-*-debug" },
			{ "-O3"; Config = "*-*-release" },
		},
		CXXOPTS = {
			"-Wno-c99-extensions", "-Wpedantic", "-Werror", "-Wall", "-Wno-deprecated-declarations",
			{ "-O0", "-g"; Config = "*-*-debug" },
			{ "-O3"; Config = "*-*-release" },
		},
	},

	Frameworks = { "Cocoa" },
}

Build {
	IdeGenerationHints = {
		Msvc = {
		PlatformMappings = {
			['win32-msvc'] = 'Win32',
		    ['win64-msvc'] = 'x64',
		},
		FullMappings = {
	        ['release']    = 'Release',
            ['debug']      = 'Debug'
			},
		},
		MsvcSolutions = { ['Arika.sln'] = { } },
	},

	Passes = {
		CompileGenerator = { Name="Compile generator", BuildOrder = 1 },
		CodeGeneration = { Name="Generate sources", BuildOrder = 2 },
	},

	Configs = {

		Config { Name = "win32-msvc", Inherit = win32, Tools = { "msvc", "dotnet" }, SupportedHosts = { "windows" }, },
		Config { Name = "win64-msvc", Inherit = win32, Tools = { "msvc", "dotnet" }, SupportedHosts = { "windows" }, },
		Config { Name = "macosx-clang", Inherit = macosx, Tools = { "clang-osx", "mono" }, SupportedHosts = { "macosx" },},
		Config { Name = "unix-gcc", Inherit = common, Tools = { "gcc" }, SupportedHosts = { "linux", "freebsd" },},
	},

	Units = {
		"units.lua",
	},
}
