
conan = {}
configs = {'Debug','Release','RelWithDebInfo'}

for i = 1,3 do
	include("build/deps/"..configs[i].."/conanbuildinfo.premake.lua")
	conan[configs[i]] = {}
	local cfg = conan[configs[i]]
	cfg["build_type"] = conan_build_type
	cfg["arch"] = conan_arch
	cfg["includedirs"] = conan_includedirs
	cfg["libdirs"] = conan_libdirs
	cfg["bindirs"] = conan_bindirs
	cfg["libs"] = conan_libs
	cfg["system_libs"] = conan_system_libs
	cfg["defines"] = conan_defines
	cfg["cxxflags"] = conan_cxxflags
	cfg["cflags"] = conan_cflags
	cfg["sharedlinkflags"] = conan_sharedlinkflags
	cfg["exelinkflags"] = conan_exelinkflags
	cfg["frameworks"] = conan_frameworks
end

function conan_config_exec()

	configs = {'Debug','Release','RelWithDebInfo'}
	for i = 1,3 do
	
		local cfg = conan[configs[i]]
		filter("configurations:"..configs[i])
	
		linkoptions { cfg["exelinkflags"] }
		includedirs{ cfg["includedirs"] }
		libdirs{ cfg["libdirs"] }
		links{ cfg["libs"] }
		links{ cfg["system_libs"] }
		links{ cfg["frameworks"] }
		defines{ cfg["defines"] }
	
		filter{}
	end
end

function conan_config_lib()
	configs = {'Debug','Release','RelWithDebInfo'}
	for i = 1,3 do
	
		local cfg = conan[configs[i]]
		filter("configurations:"..configs[i])
	
		linkoptions { cfg["sharedlinkflags"] }
		includedirs{ cfg["includedirs"] }
		defines{ cfg["defines"] }
	
		filter{}
	end
end

workspace "Motor"

configurations { "Debug", "Release", "RelWithDebInfo" }
architecture "x64"
location "build"
cppdialect "c++20"
startproject "MyMotor"
includedirs { "include", "deps/imgui", "deps/imgui/backends", "deps/imGuizmo", "deps/Perlin"} 

filter "configurations:Debug"
defines { "DEBUG" }
symbols "On"
runtime "Debug"

filter "configurations:Release"
defines { "NDEBUG" }
optimize "On"
runtime "Release"

filter "configurations:RelWithDebInfo"

defines { "NDEBUG" }
optimize "On"
runtime "Release"
symbols "On"

filter {}

-----
project "Motor"

kind "StaticLib"
targetdir "build/%{cfg.buildcfg}"
includedirs "include"
conan_config_lib()
files {
	"src/**",
	"include/**",
	"scripts/**",
	"stages/**",

	"deps/*.h",
	"deps/imgui/*.h",
	"deps/imgui/*.cpp",
	"deps/imgui/misc/cpp/*.cpp",
	"deps/imgui/misc/cpp/*.h",
	"deps/Perlin/**",
	"deps/imgui/backends/imgui_impl_glfw.h",
	"deps/imgui/backends/imgui_impl_glfw.cpp",
	"deps/imgui/backends/imgui_impl_opengl3.h",
	"deps/imgui/backends/imgui_impl_opengl3.cpp",
	"deps/imgui/backends/imgui_impl_opengl3_loader.h",
	"deps/imGuizmo/*.h",
	"deps/imGuizmo/*.cpp",

	"premake5.lua",
	"src/build/conanfile.txt",
	"src/build/conan.lua",
}

-----

function createExample(project_name, example_name)
	project(project_name)
	
	kind "ConsoleApp"
	language "C++"
	targetdir "build/%{prj.name}/%{cfg.buildcfg}"
	libdirs{"lib/%{cfg.buildcfg}"}
	links "Motor"

	conan_config_exec("Debug")
	conan_config_exec("Release")
	conan_config_exec("RelWithDebInfo")
	debugargs { _MAIN_SCRIPT_DIR .. "/src" }

	files { "examples/" ..example_name.. ".cc" }
end

createExample("MyMotor", "mymotor")
createExample("PR0_Window", "PR0_Window_Example")
createExample("PR1_Triangle", "PR1_Triangle_Example")
createExample("PR2_MeshLoad", "PR2_MeshLoad_Example")
createExample("PR3_JobSystem", "PR3_JobSystem_Example")
createExample("PR4_ECSManager", "PR4_ECSManager_Example")
createExample("PR5_Scripting", "PR5_Scripting_Example")
createExample("PR6_Lights", "PR6_Lights_Example")

