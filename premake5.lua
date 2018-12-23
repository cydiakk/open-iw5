newoption {
	trigger = "copy-to",
	description = "Optional, copy the EXE to a custom folder after build, define the path here if wanted.",
	value = "PATH"
}

workspace "open-iw5"
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
	editandcontinue "Off"
	warnings "Extra"

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
		"No64BitChecks"
	}

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
			"LinkTimeOptimization",
			"FatalCompileWarnings",
		}

	configuration "Debug"
		optimize "Debug"

		defines {
			"DEBUG",
			"_DEBUG",
		}

	configuration {}

	project "open-iw5"
		kind "WindowedApp"
		language "C++"

		pchheader "std_include.hpp"
		pchsource "src/std_include.cpp"
		
		linkoptions "/IGNORE:4254 /DYNAMICBASE:NO /SAFESEH:NO /LARGEADDRESSAWARE"
		linkoptions "/LAST:.zdata"


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

		if _OPTIONS["copy-to"] then
			postbuildcommands {
				"copy /y \"$(TargetDir)*.exe\" \"" .. _OPTIONS["copy-to"] .. "\""
			}
		end
