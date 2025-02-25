@echo OFF
setlocal

:: Ruta base de salida
set BASE_OUTDIR=..\lib\

:: Cargar variables de entorno de Visual Studio
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

echo "Starting Build for Motor"
echo .

:: Compilar con la configuración Debug
echo "Building Debug"
msbuild "..\build\Motor.vcxproj" /p:Configuration=Debug /p:Platform="x64" /p:OutDir=%BASE_OUTDIR%Debug\
if %errorlevel% neq 0 (
    echo "ERROR: La compilación Debug falló"
    exit /b %errorlevel%
)

:: Compilar con la configuración Release
echo "Building Release"
msbuild "..\build\Motor.vcxproj" /p:Configuration=Release /p:Platform="x64" /p:OutDir=%BASE_OUTDIR%Release\
if %errorlevel% neq 0 (
    echo "ERROR: La compilación Release falló"
    exit /b %errorlevel%
)

:: Compilar con la configuración RelWithDebInfo
echo "Building RelWithDebInfo"
msbuild "..\build\Motor.vcxproj" /p:Configuration=RelWithDebInfo /p:Platform="x64" /p:OutDir=%BASE_OUTDIR%RelWithDebInfo\
if %errorlevel% neq 0 (
    echo "ERROR: La compilación RelWithDebInfo falló"
    exit /b %errorlevel%
)

echo "All builds completed successfully"
pause

