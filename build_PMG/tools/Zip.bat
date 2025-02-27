@echo off
setlocal enabledelayedexpansion

:: Definir rutas
set "source=.."
set "destination=..\backup.zip"
set "tempDir=%temp%\backupTemp"

echo Creando archivo ZIP con todo el contenido de la raíz en %destination%, excluyendo las carpetas build y src, y el archivo premake5.lua original...

:: Crear directorio temporal
if not exist "%tempDir%" mkdir "%tempDir%"

:: Crear una copia del archivo premake5.lua con el contenido proporcionado
(
echo conan = {}
echo configs = {'Debug','Release','RelWithDebInfo'}
echo.
echo for i = 1,3 do
echo 	include("build/deps/"..configs[i].."/conanbuildinfo.premake.lua"^)
echo 	conan[configs[i]] = {}
echo 	local cfg = conan[configs[i]]
echo 	cfg["build_type"] = conan_build_type
echo 	cfg["arch"] = conan_arch
echo 	cfg["includedirs"] = conan_includedirs
echo 	cfg["libdirs"] = conan_libdirs
echo 	cfg["bindirs"] = conan_bindirs
echo 	cfg["libs"] = conan_libs
echo 	cfg["system_libs"] = conan_system_libs
echo 	cfg["defines"] = conan_defines
echo 	cfg["cxxflags"] = conan_cxxflags
echo 	cfg["cflags"] = conan_cflags
echo 	cfg["sharedlinkflags"] = conan_sharedlinkflags
echo 	cfg["exelinkflags"] = conan_exelinkflags
echo 	cfg["frameworks"] = conan_frameworks
echo end
echo.
echo function conan_config_exec(^)
echo.
echo 	configs = {'Debug','Release','RelWithDebInfo'}
echo 	for i = 1,3 do
echo 	
echo 		local cfg = conan[configs[i]]
echo 		filter("configurations:"..configs[i]^)
echo 	
echo 		linkoptions { cfg["exelinkflags"] }
echo 		includedirs{ cfg["includedirs"] }
echo 		libdirs{ cfg["libdirs"] }
echo 		links{ cfg["libs"] }
echo 		links{ cfg["system_libs"] }
echo 		links{ cfg["frameworks"] }
echo 		defines{ cfg["defines"] }
echo 	
echo 		filter{}
echo 	end
echo end
echo.
echo function conan_config_lib(^)
echo 	configs = {'Debug','Release','RelWithDebInfo'}
echo 	for i = 1,3 do
echo 	
echo 		local cfg = conan[configs[i]]
echo 		filter("configurations:"..configs[i]^)
echo 	
echo 		linkoptions { cfg["sharedlinkflags"] }
echo 		includedirs{ cfg["includedirs"] }
echo 		defines{ cfg["defines"] }
echo 	
echo 		filter{}
echo 	end
echo end
echo.
echo workspace "Motor"
echo.
echo configurations { "Debug", "Release", "RelWithDebInfo" }
echo architecture "x64"
echo location "build"
echo cppdialect "c++20"
echo startproject "MyMotor"
echo includedirs { "include", "deps/imgui", "deps/imgui/backends", "deps/imGuizmo", "deps/Perlin"} 
echo.
echo filter "configurations:Debug"
echo defines { "DEBUG" }
echo symbols "On"
echo runtime "Debug"
echo.
echo filter "configurations:Release"
echo defines { "NDEBUG" }
echo optimize "On"
echo runtime "Release"
echo.
echo filter "configurations:RelWithDebInfo"
echo.
echo defines { "NDEBUG" }
echo optimize "On"
echo runtime "Release"
echo symbols "On"
echo.
echo filter {}
echo.
echo function createExample(project_name, example_name^)
echo 	project(project_name^)
echo 	
echo 	kind "ConsoleApp"
echo 	language "C++"
echo 	targetdir "build/%%{prj.name}/%%{cfg.buildcfg}"
echo 	libdirs{"lib/%%{cfg.buildcfg}"}
echo 	links "Motor"
echo.
echo 	conan_config_exec("Debug"^)
echo 	conan_config_exec("Release"^)
echo 	conan_config_exec("RelWithDebInfo"^)
echo 	debugargs { _MAIN_SCRIPT_DIR .. "/src" }
echo.
echo 	files { "examples/" ..example_name.. ".cc" }
echo end
echo.
echo createExample("MyMotor", "mymotor"^)
echo createExample("PR0_Window", "PR0_Window_Example"^)
echo createExample("PR1_Triangle", "PR1_Triangle_Example"^)
echo createExample("PR2_MeshLoad", "PR2_MeshLoad_Example"^)
echo createExample("PR3_JobSystem", "PR3_JobSystem_Example"^)
echo createExample("PR4_ECSManager", "PR4_ECSManager_Example"^)
echo createExample("PR5_Scripting", "PR5_Scripting_Example"^)
echo createExample("PR6_Lights", "PR6_Lights_Example"^)
) > "%tempDir%\premake5.lua"

:: Comprimir todo el contenido de la raíz, excluyendo las carpetas build y src, y el archivo premake5.lua original
powershell -command "$sourcePath = '%source%'; $destinationPath = '%destination%'; $tempDirPath = '%tempDir%'; $excludeItems = 'build', 'src', 'premake5.lua'; Get-ChildItem -Path $sourcePath -Exclude $excludeItems | Compress-Archive -DestinationPath $destinationPath -Force; Compress-Archive -Path $tempDirPath\premake5.lua -Update -DestinationPath $destinationPath"

:: Eliminar el directorio temporal
rmdir /s /q "%tempDir%"

echo Completado.
pause
