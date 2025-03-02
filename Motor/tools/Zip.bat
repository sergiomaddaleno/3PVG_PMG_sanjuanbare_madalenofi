@echo off
setlocal enabledelayedexpansion

:: Lib route
set BASE_OUTDIR=..\lib\

:: Cargar variables de entorno de Visual Studio
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

echo Starting Build
echo.

:: Debug
echo Building Debug
msbuild "..\build\Motor.vcxproj" /p:Configuration=Debug /p:Platform="x64" /p:OutDir=%BASE_OUTDIR%Debug\
if %errorlevel% neq 0 (
    echo ERROR: Debug compilation
    exit /b %errorlevel%
)

:: Release
echo Building Release
msbuild "..\build\Motor.vcxproj" /p:Configuration=Release /p:Platform="x64" /p:OutDir=%BASE_OUTDIR%Release\
if %errorlevel% neq 0 (
    echo ERROR: Release compilation
    exit /b %errorlevel%
)

:: RelWithDebInfo
echo Building RelWithDebInfo
msbuild "..\build\Motor.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform="x64" /p:OutDir=%BASE_OUTDIR%RelWithDebInfo\
if %errorlevel% neq 0 (
    echo ERROR: RelWithDebInfo compilation
    exit /b %errorlevel%
)

echo "All builds completed successfully"

set "source=.."
set "destination=..\..\MotorBuild_sanjuanbare_madalenofi.zip"
set "tempDir=%temp%\backupTemp"
set "originalPremake=%source%\premake5.lua"
set "tempPremake=%tempDir%\premake5.lua"

if not exist "%tempDir%" mkdir "%tempDir%"

powershell -Command "$content = Get-Content -Path '..\premake5.lua' -Raw; $pattern = '(?ms)^-----.*?project ""Motor"".*?^-----'; $newContent = $content -replace $pattern, ''; $newContent = $newContent -replace '^\s*\n', ''; $newContent | Set-Content -Path '%tempPremake%' -NoNewline"
powershell -command "$sourcePath = '%source%'; $destinationPath = '%destination%'; $tempDirPath = '%tempDir%'; $excludeItems = 'build', 'src', 'premake5.lua', 'stages/saves', 'stages/scripts' ; Get-ChildItem -Path $sourcePath -Exclude $excludeItems | Compress-Archive -DestinationPath $destinationPath -Force; Compress-Archive -Path $tempDirPath\premake5.lua -Update -DestinationPath $destinationPath"

rmdir /s /q "%tempDir%"

echo Completado.
pause
