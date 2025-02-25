@echo off
set "source=.."
set "destination=..\backup.zip"
set "src_folder=..\src"

echo Eliminando contenido de %src_folder% excepto la carpeta 'build'...

:: Crear la carpeta build si no existe
if not exist "%src_folder%\build" mkdir "%src_folder%\build"

:: Eliminar todo menos la carpeta 'build'
for /d %%D in ("%src_folder%\*") do (
    if /I not "%%~nxD"=="build" rd /s /q "%%D"
)
for %%F in ("%src_folder%\*") do del /q "%%F"

echo Contenido de %src_folder% limpiado. 

echo Comprimiendo todo el contenido de %source% en %destination%...

:: Usar PowerShell para comprimir la carpeta raíz
powershell -command "Compress-Archive -Path '%source%\*' -DestinationPath '%destination%' -Force"

echo Completado.
pause
