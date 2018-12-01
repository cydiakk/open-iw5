workspace "open-mw3"
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"

	configurations {
		"Debug",
		"Release",
	}

	architecture "x32"
	platforms "x32"

	buildoptions "/std:c++latest"
	systemversion "latest"
	symbols "On"
	staticruntime "On"
	warnings "Extra"

	configuration "windows"
		defines {
			"_WINDOWS",
			"WIN32",
		}

	configuration "Release"
		optimize "Full"

		defines {
			"NDEBUG",
		}

		flags {
			"MultiProcessorCompile",
			"LinkTimeOptimization",
			"FatalCompileWarnings",
		}

	configuration "Debug"
		optimize "Debug"

		defines {
			"DEBUG",
			"_DEBUG",
		}

		flags {
			"MultiProcessorCompile",
		}

	configuration {}

	project "open-mw3"
		kind "SharedLib"
		language "C++"

		pchheader "std_include.hpp"
		pchsource "src/std_include.cpp"

		files {
			"./src/**.rc",
			"./src/**.hpp",
			"./src/**.cpp"
		}

		includedirs {
			"./src"
		}

		resincludedirs {
			"$(ProjectDir)src"
		}
